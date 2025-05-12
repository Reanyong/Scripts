#include "pch.h"
#include <crtdbg.h>
#include "token.h"
#include "com.h"
#include "c_engine.h"

#include "c_str_array.h"

#include "c_if_atom.h"
#include "c_assign_atom.h"
#include "c_print_atom.h"
#include "c_input_atom.h"
#include "c_for_atom.h"
#include "c_while_atom.h"
#include "c_label_atom.h"
#include "c_goto_atom.h"
#include "c_dialog_atom.h"
#include "c_dim_atom.h"
#include "c_beep_atom.h"
#include "c_cls_atom.h"
#include "c_routine_entry_atom.h"
#include "c_end_routine_atom.h"
#include "c_call_sub_atom.h"
#include "c_do_atom.h"
#include "c_case_atom.h"
#include "c_exit_atom.h"

bool c_engine::variable_declared(const char* p_name)
{
	if (m_atom_table.is_waiting()) // at run-time
	{
		return m_call_stack.declared(p_name, 0);
	}
	else	// at parse-time
	{
		if (!m_bexplicit) return true;

		int i;

		for (i = 0; i < m_sub_namespace.get_size(); i++)
			if (_stricmp(p_name, m_sub_namespace[i].m_name.get_buffer()) == 0) return true;

		for (i = 0; i < m_global_namespace.get_size(); i++)
			if (_stricmp(p_name, m_global_namespace[i].m_name.get_buffer()) == 0) return true;


		error(CUR_ERR_LINE, "undeclared variable: '%s'.", p_name);
		return false;
	}
}

bool c_engine::parse()
{
	ClearAllErrors();
	bool bResult = true;
	m_func_expressions.reset();
	m_func_names.empty();
	m_line_numbers.reset();
	m_bexplicit = false;
	m_bexplicit_set = false;

	// 컴파일 중에 오류가 발생해도 계속 진행하도록 설정
	bool originalContinueValue = m_bContinueOnError;
	m_bContinueOnError = true;

#ifndef _DEBUG
	try
	{
#endif
		// free sub and global namespace from previous parse --------------------------
		m_sub_namespace.reset(true);
		m_global_namespace.reset(true);
		// find subs and global variables ---------------------------------------------
		if (!_pre_parse()) {
			bResult = false;
			// 오류가 있더라도 계속 진행
		}
		prepare_global_namespace();
		if (!prepare_dll_routines()) {
			bResult = false;
			// 오류가 있더라도 계속 진행
		}
		m_bexplicit_set = false;
		// parse routines -------------------------------------------------------------
		prevtok.reset();
		curtok.reset();
		bool b_errors = false;
		for (int i = 0; i < m_sub_table.get_size(); i++)
		{
			m_char_stream.pos(m_sub_table.get(i).m_position);
			m_sub_namespace.reset(false);
			if (parse_routine(m_sub_table.get(i).b_is_sub) == ERR_) {
				b_errors = true;
				// 오류가 있더라도 계속 진행
			}
		}
		// parse main program ---------------------------------------------------------
				// go to entry pos
		m_pcur_routine_entry = 0;
		m_char_stream.pos(m_nentry_pos);
		m_sub_namespace.reset(false);
		gettok();
		// parse it
		c_vector_table last;
		if (_parse(last) == ERR_) {
			b_errors = true;
			// 오류가 있더라도 계속 진행
		}
		// post-parse -----------------------------------------------------------------
		if (b_errors) {
			bResult = false;
			// 오류가 있더라도 계속 진행
		}
		if (!_post_parse()) {
			bResult = false;
			// 오류가 있더라도 계속 진행
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		error(CUR_ERR_LINE, "runtime exception while parsing source.");
		bResult = false;
	}
#endif

	// 원래 설정으로 복원
	m_bContinueOnError = originalContinueValue;

	// prepare function callers
	if (bResult) {
		m_atom_table.prepare_function_callers();
	}

#ifdef _DEBUG
	if (bResult) {
		m_atom_table.dump();
	}
#endif

	// 오류 또는 경고가 있으면 결과 출력
	if (GetErrorCount() > 0 || GetWarningCount() > 0) {
		PrintAllErrors(true);
		DebugLog("파싱 완료: 오류=%d, 경고=%d", GetErrorCount(), GetWarningCount());
	}

	// 오류가 하나라도 있으면 false 반환
	return GetErrorCount() == 0 && bResult;
}

bool c_engine::_pre_parse()
{
	bool	b_result		= true;
	int		pos;
	bool	b_is_sub		= false;
	int		endsub_pos		= 0;
	int		n_illegal_line;
	c_routine_entry e;

	while(1)
	{
		e.reset();

// find routine entry ---------------------------------------------------------

		pos				= -1;
		n_illegal_line	= -1;

		for (;;)
		{
			gettok();
			if (curtok.m_name[0]=='@')
			{
				if (g_pTagCol==NULL)
				{
					error(CUR_ERR_LINE, "\nTagCollector is not initialized.\nFirst, call the ScriptInit function \nand then call the ScriptCompile function.");
					return false;
				}
				CString tagname = &curtok.m_name[1];
				if (!CheckValidTagName(tagname))
				{
					error(CUR_ERR_LINE, "InValid TagName ["+tagname +"]");
					return false;
				}
			}
			if (curtok.type == token_type::system ||
				curtok.type == token_type::graphic ||
				curtok.type == token_type::object ||
				(curtok.m_name[0] == '$' && _stricmp(curtok.m_name.get_buffer(), "$System") == 0))
			{
				/*
				// $System 관련 구문은 건너뜁니다
				while (curtok.type != token_type::eos && curtok.type != token_type::eof)
					gettok();
				continue;
				*/
			}
			if (curtok.type == token_type::declare_cond)
			{
				while (curtok.type != token_type::eos && curtok.type != token_type::eof) gettok();
				continue;
			}

			if (curtok.type == token_type::sub_cond || curtok.type == token_type::function_cond)
			{
				pos			= m_char_stream.pos();
				b_is_sub	= curtok.type == token_type::sub_cond;

				gettok();

				if (curtok.type != token_type::name)
				{
					_ASSERT(0);
					break;
				}

				e.m_position = pos;
				e.b_is_sub = b_is_sub;
				e.m_name = curtok.m_name;

				_ASSERT(e.m_nargs == 0);

// calc number of arguments ---------------------------------------------------

				bool b_lb = false;
				bool b_rb = false;
				bool b_other = false;
				while (curtok.type != token_type::eof && curtok.type != token_type::eos)
				{
					switch (curtok.type)
					{
					case token_type::lb: b_lb = true;				break;
					case token_type::rb: b_rb = true;				break;
					case token_type::comma: e.m_nargs ++;			break;
					case token_type::as_cond:
						gettok();
						if (b_rb) e.m_type = get_type(curtok.m_name.get_buffer());
						break;

					default: if (b_lb) b_other = true;	break;
					}
					gettok();
				}

				if (b_other && b_lb && b_rb) e.m_nargs++;

//-----------------------------------------------------------------------------

				break;
			}
			else if (curtok.type == token_type::eof) break;			// no subs
			else if (curtok.type == token_type::eos) continue;
			else if (curtok.type == token_type::option_cond)
			{
				if (parse_option() == ERR_) return false;
				continue;
			}
			else
			{
				if (n_illegal_line == -1)
					n_illegal_line = m_char_stream.cur_line();
			}
		}

		if (pos != -1 && n_illegal_line != -1)
		{
			error (n_illegal_line, "illegal statements preceding subs/functions declaration.");
			b_result = false;
		}

		if (pos == -1) break;


		m_sub_table.add(e);

// find end routine -----------------------------------------------------------

		for(;;)
		{
			gettok();
			if (curtok.type == token_type::endsub_cond ||
				curtok.type == token_type::endfunction_cond)
			{
				endsub_pos = m_char_stream.pos();
				break;
			}
			else if (curtok.type == token_type::eof)
			{
				if (b_is_sub)
					error(CUR_ERR_LINE, "unexpected end of file while looking for 'end sub'");
				else
					error(CUR_ERR_LINE, "unexpected end of file while looking for 'end function'");
				return false;
			}
		}
	}

	m_nentry_pos = endsub_pos;
	return b_result;
}

bool c_engine::prepare_dll_routines()
{
	prevtok.reset();
	curtok.reset();
	m_char_stream.pos((unsigned long)0);

	for (;;)
	{
		c_dll_func dll_func;

		gettok();
		if (curtok.type == token_type::eof) break;

		if (curtok.type == token_type::declare_cond)
		{
			// 'function' or 'sub'
			gettok();
			if (curtok.type != token_type::function_cond &&
				curtok.type != token_type::sub_cond)
			{
				error(CUR_ERR_LINE, "'sub' or 'function' expected where '%s' found (after 'declare' statement).", curtok.format());
				return false;
			}

			bool b_is_sub = (curtok.type == token_type::sub_cond);

			// routine name
			gettok();
			if (curtok.type != token_type::name)
			{
				error(CUR_ERR_LINE, "routine name expected where '%s' found (in 'declare' statement).", curtok.format());
				return false;
			}

			dll_func.set_routine_name(curtok.m_name.get_buffer());

			// 'lib'
			gettok();
			if (curtok.type != token_type::lib_cond)
			{
				error(CUR_ERR_LINE, "'lib' expected where '%s' found (in 'declare' statement).", curtok.format());
				return false;
			}

			// lib name
			gettok();
			if (curtok.type != token_type::string)
			{
				error(CUR_ERR_LINE, "string containing lib name expected where '%s' found (in 'declare' statement).", curtok.format());
				return false;
			}

			dll_func.set_dll_name(curtok._string.get_buffer());

			// alias or '('
			//int n_pos = m_char_stream.pos();
			PUSH_CURTOK();

			gettok();
			if (curtok.type == token_type::alias_cond)
			{
				gettok();
				if (curtok.type != token_type::string)
				{
					error(CUR_ERR_LINE, "string containing alias name expected where '%s' found (in 'declare' statement).", curtok.format());
					return false;
				}

				dll_func.set_alias_name(curtok._string.get_buffer());
			}
			else
			{
				dll_func.set_alias_name(dll_func.get_routine_name());
				//m_char_stream.pos(n_pos);
				//curtok = prevtok;
				POP_CURTOK();
			}

			// parse arguments
			c_array<c_dim_entry> args;
			VARENUM				type;

			if (!parse_arg_declaration(	b_is_sub,
										dll_func.get_routine_name(),
										&args,
										&type,
										true))
			{
				return false;
			}

			// parsed OK. save.
			dll_func.set_type(type);

			int i;
			for (i = 0; i < args.get_size(); i++)
				dll_func.add_arg(args[i].m_type, args[i].m_bbyref);

			m_dll_funcs.add(dll_func);

			c_string err;
			if (!dll_func.load(&err))
			{
				error(CUR_ERR_LINE, err.get_buffer());
				return false;
			}
		}
	}

	return true;
}

void c_engine::prepare_global_namespace()
{
	_ASSERT(m_global_namespace.get_size() == 0);

	m_char_stream.pos(m_nentry_pos);
	prevtok.reset();
	curtok.reset();

	c_str_array	names;

	do
	{
		gettok();
		if (curtok.type == token_type::dim_cond)
		{
			while (1)
			{
				gettok();

				switch (curtok.type)
				{
				case token_type::as_cond:
					gettok();
					if (curtok.type == token_type::name)
					{
						int i;
						for (i = 0; i < names.get_size(); i++)
						{
							c_dim_entry e;
							e.m_name = names[i];
							e.m_type = get_type(curtok.m_name);
							m_global_namespace.add(e);
						}

						names.empty();
					}

					break;

				case token_type::name:
					names.add(curtok.m_name);
					break;

				case token_type::eof:
				case token_type::eos: goto _done;
				}
			}
_done:;
		}
	}
	while (curtok.type != token_type::eof);
}

DWORD c_engine::_parse(c_vector_table& last, DWORD stop_at)
{
	DWORD n;

	while (curtok.type != token_type::eof)
	{
		while (curtok.type == token_type::eos) gettok();

		switch (curtok.type)
		{

// rudiments --------------------------

		case token_type::next_cond:
			return parse_next(last, stop_at);

		case token_type::else_cond:
			return parse_else(last, stop_at);

		case token_type::endif_cond:
			return parse_endif(last, stop_at);

		case token_type::wend_cond:
			return parse_wend(last, stop_at);

		case token_type::case_cond:
			return parse_case(last, stop_at);

		case token_type::endselect_cond:
			return parse_endselect(last, stop_at);

		case token_type::declare_cond:
			n = parse_declare(last, stop_at);
			break;

// normal -----------------------------

		case token_type::eof:
			return EOF_TOKEN;

		case token_type::if_cond:
			n = parse_if(last, stop_at);
			break;

		case token_type::end_cond:
			n = parse_end(last, stop_at);
			break;

		case token_type::print_cond:
			n = parse_print(last, stop_at);
			break;

		case token_type::input_cond:
			n = parse_input(last, stop_at);
			break;

		case token_type::for_cond:
			n = parse_for(last, stop_at);
			if (n == ERR_ && m_bContinueOnError)
			{
				gettok();
				n = TO_GO;
			}
			break;

		case token_type::while_cond:
			n = parse_while(last, stop_at);
			break;

		case token_type::do_cond:
			n = parse_do(last, stop_at);
			break;

		case token_type::loop_cond:
			return parse_loop(last, stop_at);

		case token_type::dialog_cond:
			n = parse_dialog(last, stop_at);
			break;

		case token_type::dim_cond:
			n = parse_dim(last, stop_at);
			break;

		case token_type::select_cond:
			n = parse_select(last, stop_at);
			break;

		case token_type::beep_cond:
			n = parse_beep(last, stop_at);
			break;

		case token_type::cls_cond:
			n = parse_cls(last, stop_at);
			break;

		case token_type::name:
			if (is_ext_sub(curtok.m_name))			n = parse_ext_sub(last, stop_at);
			else if (is_dll_routine(curtok.m_name))	n = parse_dll_sub(last, stop_at);
			else if (is_ext_func(curtok.m_name))	n = parse_ext_sub(last, stop_at);// function called in sub context
			else if (is_sub(curtok.m_name))			n = parse_call_routine(last, stop_at, true);
			else if (is_function(curtok.m_name))
			{
				// c_variable inside function that has the same name as function
				if (m_pcur_routine_entry && _stricmp(curtok.m_name, m_pcur_routine_entry->get_var_desc()->get_name()) == 0)
					n = parse_assign(last, stop_at);

				// function called from other routine
				else
					n = parse_call_routine(last, stop_at, false);
			}
			else if (is_ext_object(curtok.m_name))
			{
				bool b_var;
				n = parse_disp_set(&b_var, last, stop_at, 0);
				if (n == ERR_ && b_var)
				{
					if (variable_declared(curtok.m_name.get_buffer()))
					{
						//gettok();
						n = parse_assign(last, stop_at);
					}
					else
						_ASSERT(0); // todo: report error
				}
			}
			else if (is_ext_const(curtok.m_name))
			{
				error(CUR_ERR_LINE, "constant '%s' unexpected here.", curtok.m_name);
				return ERR_;
			}
			else
			{
				CComPtr<IDispatch> lpd = get_default_parent(curtok.m_name.get_buffer());
				if (lpd)
				{
					CComPtr<ITypeInfo> pti = get_type_info(lpd);
					bool b_var;
					n = parse_disp_set(&b_var, last, stop_at, pti);
					_ASSERT(!b_var); // I don't expect it here
				}
				else
				{
					CLSID clsid = {0};
					GUID  libid = {0};
					if (is_object_variable(curtok.m_name.get_buffer(), &clsid, &libid))
					{
						USES_CONVERSION;

						_ASSERT(!is_NULL_GUID(&libid));
						ITypeLib* ptl = load_typelib(&libid);
						if (ptl)
						{
							ITypeInfo* pti = 0;
							HRESULT hr = ptl->GetTypeInfoOfGuid(clsid, &pti);
							_ASSERT(!FAILED(hr) && pti);

							bool b_var;
							n = parse_disp_set(&b_var, last, stop_at, pti);
							if (n == ERR_ && b_var)
							{
								if (variable_declared(curtok.m_name.get_buffer()))
								{
									//gettok();
									n = parse_assign(last, stop_at);
								}
								else
									_ASSERT(0); // todo: report error?
							}

							pti->Release();
							ptl->Release();
						}
						else
						{
							error(CUR_ERR_LINE, "cannot load type library for object '%s'.", curtok.m_name.get_buffer());
							return ERR_;
						}
					}
					else
 						n = parse_assign(last, stop_at);
				}
			}
			break;

		case token_type::goto_cond:
			n = parse_goto(last, stop_at);
			break;

		case token_type::label:
			n = parse_label(last, stop_at);
			break;

// sub/function -----------------------

		case token_type::sub_cond:
			{
				error (CUR_ERR_LINE, "unexpected 'sub'.");
				return ERR_;
			}
			break;

		case token_type::endsub_cond:
			return parse_endsub(last, stop_at);

		case token_type::endfunction_cond:
			return parse_endfunction(last, stop_at);

// exit -------------------------------

		case token_type::exit_sub_cond:
			n = parse_exit_sub(last, stop_at);
			break;

		case token_type::exit_function_cond:
			n = parse_exit_function(last, stop_at);
			break;

		case token_type::exit_for_cond:
			n = parse_exit_for(last, stop_at);
			break;

		case token_type::exit_while_cond:
			n = parse_exit_while(last, stop_at);
			break;

		case token_type::exit_do_cond:
			n = parse_exit_do(last, stop_at);
			break;

		case token_type::option_cond:
			n = parse_option();
			break;
// System -----------------------------

		case token_type::system:
			// $System 토큰 처리
			n = parse_system_command(last, stop_at);
			break;
		/*
		case token_type::graphic:
			// Graphic 토큰 처리
			n = parse_graphic_object(last, stop_at);
			break;

		case token_type::object:
			// Object 토큰 처리
			n = parse_object(last, stop_at);
			break;

		case token_type::property:
			// property 토큰 처리
			n = parse_property(last, stop_at);
			break;
		*/
// errors -----------------------------

		case token_type::then_cond:
			error (CUR_ERR_LINE, "unexpected 'then'.");
			return ERR_;

		case token_type::as_cond:
			error (CUR_ERR_LINE, "unexpected 'as'.");
			return ERR_;

		case token_type::exit_cond:
			error (CUR_ERR_LINE, "illegal single 'exit'. 'sub', 'function', etc. expected.");
			return ERR_;

		default:
			{
				error (CUR_ERR_LINE, "%s unexpected here", curtok.format());
				return ERR_;
			}
		}

		if (n != TO_GO) return n;
	}

	return TO_GO;
}

DWORD c_engine::parse_do (c_vector_table& last, DWORD stop_at)
{
	if (stop_at == SINGLE_ATOM)
	{
		error (CUR_ERR_LINE, "'do' statement must start from new line.");
		return ERR_;
	}

	c_do_atom* p_atom = new c_do_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());
	m_atom_table.add(p_atom);

	gettok();
	switch (curtok.type)
	{
	case token_type::eos:
	case token_type::eof: goto _do_cond_done;	// no condition here

	case token_type::while_cond:
		p_atom->is_while(true);
		break;

	case token_type::until_cond:
		p_atom->is_while(false);
		break;

	default:
		error(CUR_ERR_LINE, "'while', 'until' or end of line expected after 'do'.");
		return ERR_;
	}

	{
		gettok();
		c_expression* p_expr = _expr();
		if (p_expr == 0)
		{
			if (p_atom->is_while())
				error(CUR_ERR_LINE, "cannot parse expression after 'while'.");
			else
				error(CUR_ERR_LINE, "cannot parse expression after 'until'.");

			return ERR_;
		}
		p_atom->set_do_expr(p_expr);
	}

_do_cond_done:

// parse loop contents ----------------

	gettok();

	c_vector_table inside_loop;

	inside_loop.push(&p_atom->m_ploop);

	DWORD result = _parse(inside_loop, LOOP);
	if (result != LOOP)
	{
		if (result != ERR_)
			error (p_atom->m_nline, "'do' without 'loop'.");
		return ERR_;
	}

// loop -------------------------------

	_ASSERT(curtok.type == token_type::loop_cond);
	switch((token_type)gettok())
	{
	case token_type::eos:
	case token_type::eof: goto _loop_cond_done;		// no condition next to 'loop'

	case token_type::while_cond:
		if (p_atom->do_expression_present())
		{
			error(CUR_ERR_LINE, "condition next to both 'do' and 'loop' not allowed.");
			return ERR_;
		}
		p_atom->is_while(true);
		break;

	case token_type::until_cond:
		if (p_atom->do_expression_present())
		{
			error(CUR_ERR_LINE, "condition next to both 'do' and 'loop' not allowed.");
			return ERR_;
		}
		p_atom->is_while(false);
		break;
	}

	{
		gettok();
		c_expression* p_expr = _expr();
		if (p_expr == 0)
		{
			if (p_atom->is_while())
				error(CUR_ERR_LINE, "cannot parse expression after 'while'.");
			else
				error(CUR_ERR_LINE, "cannot parse expression after 'until'.");

			return ERR_;
		}
		p_atom->set_loop_expr(p_expr);
	}

_loop_cond_done:

// now join ---------------------------

	while(c_atom** p_last = inside_loop.pop())
		*p_last = p_atom;

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pexit);

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_loop (c_vector_table& last, DWORD stop_at)
{
	if (stop_at & LOOP)
	{
		// just return; parse_do will continue;
		return LOOP;
	}
	else
	{
		error(CUR_ERR_LINE, "'loop' without 'do'.");
		return ERR_;
	}
}

DWORD c_engine::parse_call_routine(c_vector_table& last, DWORD stop_at, bool b_is_sub)
{
	c_call_sub_atom* p_atom = new c_call_sub_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	c_string sub_name;
	sub_name = curtok.m_name;

	p_atom->set_sub_name(sub_name);

	c_ptr_array<c_expression> expressions;

	if (!parse_arg_list(expressions,
					  curtok.m_name,
					  0,
					  true))
	{
		delete p_atom;
		return ERR_;
	}

	if (expressions.get_size() != get_routine_arg_count(sub_name.get_buffer()))
	{
		error(CUR_ERR_LINE, "sub '%s' does not get %d arguments.", sub_name.get_buffer(), expressions.get_size());
		delete p_atom;
		return ERR_;
	}

	for (int i = 0; i < expressions.get_size(); i++)
		p_atom->add_expression(expressions[i]);

//-------------------------------------

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	if (stop_at == SINGLE_ATOM) return SUCCESS;

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_routine (bool b_is_sub)
{
	gettok();		// routine name

	if (!can_be_identifier(curtok.m_name, true)) return ERR_;
	if (m_atom_table.get_routine_entry(curtok.m_name, true) != 0)
//	if (is_sub(curtok.m_name))
	{
		error(CUR_ERR_LINE, "sub '%s' already declared.", (LPSTR)(LPCSTR)curtok.m_name);
		return ERR_;
	}
	else if (m_atom_table.get_routine_entry(curtok.m_name, false) != 0)
//	else if (is_function(curtok.m_name))
	{
		error(CUR_ERR_LINE, "function '%s' already declared.", (LPSTR)(LPCSTR)curtok.m_name);
		return ERR_;
	}

	if (curtok.type != token_type::name)
	{
		if (b_is_sub) error(CUR_ERR_LINE, "sub name expected after 'sub'.");
		else error(CUR_ERR_LINE, "function name expected after 'function'.");
		return ERR_;
	}

	c_routine_entry_atom* p_atom = new c_routine_entry_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());
	p_atom->is_sub(b_is_sub);
	m_pcur_routine_entry = p_atom;

	c_string sub_name;
	sub_name = curtok.m_name;

	p_atom->set_name(sub_name);

	// parse arguments
	c_array<c_dim_entry> args;

	VARENUM				type;

	if (!parse_arg_declaration(	b_is_sub,
								sub_name.get_buffer(),
								&args,
								&type))
	{
		delete p_atom;
		return ERR_;
	}

	int i;
	for (i = 0; i < args.get_size(); i++)
	{
		p_atom->add_arg(args[i]);
		m_sub_namespace.add(args[i]);
	}

	c_dim_entry e;
	e.m_type = type;
	e.m_name = sub_name;

	p_atom->set_dim_entry(&e);

	m_sub_namespace.add(e);

	m_atom_table.add(p_atom);
	c_vector_table sub_table;
	sub_table.push(&p_atom->m_pnext);

	gettok();

	DWORD ret;
	if (b_is_sub) ret = _parse(sub_table, ENDSUB);	// go into sub
	else ret = _parse(sub_table, ENDFUNCTION);

#ifdef _DEBUG
	if (!m_str_error.get_length())
		_ASSERT(sub_table.pop() == 0);	// should be empty unless error occured
#endif

	return ret;
}

bool c_engine::parse_arg_declaration(bool b_sub,
									 const char*		p_name,
									 c_array<c_dim_entry>* p_args,
									 VARENUM*			p_type,
									 bool				b_dll/*=false*/)
{
	VARENUM type;
	bool strict;
	bool b_byref;
	c_string varname;

	*p_type = VT_EMPTY;

	gettok();
	if (curtok.type == token_type::eof || curtok.type == token_type::eos) goto _end;	// no args - done

	if (curtok.type != token_type::lb)
	{
		if (b_sub)
			error (CUR_ERR_LINE, "'(' expected after sub '%s'.", p_name);
		else
			error (CUR_ERR_LINE, "'(' expected after function '%s'.", p_name);

		return false;
	}

	for (;;)
	{
		// reset argument parameters
		type	= VT_EMPTY;
		strict	= false;
		b_byref = false;
		varname.empty();
		CLSID clsid = {0};
		GUID libid = {0};

		gettok();						// argument name or byref/byval

		switch(curtok.type)
		{
		case token_type::rb:
			gettok();
			goto _end;
			break;

		case token_type::byref_cond:
			b_byref = true;
			gettok();
			break;

		case token_type::byval_cond:
			b_byref = false;
			gettok();
			break;

		case token_type::name:
			break;

		default:
			error (CUR_ERR_LINE, "variable name or byref/byval expected where '%s' found.", curtok.format());
			return false;
		}

		if (curtok.type != token_type::name)
		{
			error (CUR_ERR_LINE, "variable name expected where '%s' found.", curtok.format());
			return false;
		}

		varname = curtok.m_name;

		gettok();						// ',' or 'as' here

		switch (curtok.type)
		{
		case token_type::comma: case token_type::rb: goto _arg_done;
		case token_type::as_cond: break;
		default:
			{
				error (CUR_ERR_LINE, "'as' or ',' expected after '%s'", varname);
				return false;
			}
		}

		gettok();						// argument type here;

		if (curtok.type != token_type::name)
		{
			error (CUR_ERR_LINE, "type name expected where '%s' found.", curtok.format());
			return false;
		}

		type = get_type(curtok.m_name);

		if (type == -1)
		{
			// try object
			c_string type_name = curtok.m_name;
			gettok();
			if (curtok.type == token_type::dot)
			{
				gettok();
				if (curtok.type == token_type::name)
				{
					type_name += '.';
					type_name += curtok.m_name;

					if (get_clsid_libid(type_name.get_buffer(), &clsid, &libid, 0))
						type = VT_DISPATCH;
					else
					{
						error(CUR_ERR_LINE, "'%s' class is not registered in the system.", type_name.get_buffer());
						return false;
					}
				}
			}
			else
			{
				error(CUR_ERR_LINE, "unknown type: '%s'", type_name.get_buffer());
				return false;
			}
		}

		strict = true;

		gettok();
_arg_done:

		c_dim_entry* p_new = p_args->add_new();

		p_new->m_clsid = clsid;
		p_new->m_libid = libid;
		p_new->m_name = varname;
		p_new->m_type = type;
		p_new->m_bbyref = b_byref;

		if (!b_dll && b_byref && (type != VT_VARARRAY && type != VT_EMPTY))
		{
			error (CUR_ERR_LINE, "byref argument '%s' must be typeless.", varname.get_buffer());
			return false;
		}

		if (curtok.type == token_type::comma)   continue;
		else if (curtok.type == token_type::rb) break;
		else
		{
			error (CUR_ERR_LINE, "',' or ')' expected");
			return false;
		}
	}


	gettok();		// eos or 'as'
_end:

	if (is_declared_local_var(p_name))
	{
		error(CUR_ERR_LINE, "attempt to redeclare variable '%s'.", p_name);
		return false;
	}

	switch (curtok.type)
	{
	case token_type::as_cond:
		{
			if (b_sub)
			{
				error (CUR_ERR_LINE, "sub does not have return type.");
				return false;
			}

			gettok();
			if (curtok.type != token_type::name)
			{
				error (CUR_ERR_LINE, "type name expected after 'as' in function declaration.");
				return false;
			}

			*p_type = get_type(curtok.m_name.get_buffer());
		}
		break;

	case token_type::eof:
	case token_type::eos:
		break; // ok, no type

	default:
		if (b_sub)
			error (CUR_ERR_LINE, "end of line expected after sub declaration.");
		else
			error (CUR_ERR_LINE, "end of line expected after function declaration.");
		return false;
	}

	return true;
}

DWORD c_engine::parse_endsub(c_vector_table& last, DWORD stop_at)
{
	if (stop_at & ENDSUB)		// we are here if 'sub' is empty
	{
		c_end_routine_atom* p_atom = new c_end_routine_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

		m_atom_table.add(p_atom);

		while(c_atom** p_last = last.pop())
			*p_last = p_atom;

		gettok();				// ommit 'endsub'
		return ENDSUB;
	}
	else
	{
		error(CUR_ERR_LINE, "'endsub' without 'sub'.");
		return ERR_;
	}
}

DWORD c_engine::create_exit_atom(c_vector_table& last,
								 DWORD stop_at,
								 exit_type type)
{
	c_exit_atom* p_atom = new c_exit_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());
	p_atom->set_type(type);

	gettok();

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	if (stop_at == SINGLE_ATOM) return SUCCESS;
	return TO_GO;
}

DWORD c_engine::parse_exit_sub (c_vector_table& last, DWORD stop_at)
{
	if (m_atom_table.is_parsing_sub())
		return create_exit_atom(last, stop_at, exit_type::exit_routine);
	else
	{
		error(CUR_ERR_LINE, "'exit sub' illegal outside 'sub ... end sub'.");
		return ERR_;
	}
}

DWORD c_engine::parse_exit_function (c_vector_table& last, DWORD stop_at)
{
	if (m_atom_table.is_parsing_function())
		return create_exit_atom(last, stop_at, exit_type::exit_routine);
	else
	{
		error(CUR_ERR_LINE, "'exit function' illegal outside 'function ... end function'.");
		return ERR_;
	}
}

DWORD c_engine::parse_exit_for (c_vector_table& last, DWORD stop_at)
{
	if (m_atom_table.is_parsing_for())
		return create_exit_atom(last, stop_at, exit_type::exit_for);
	else
	{
		error(CUR_ERR_LINE, "'exit for' illegal outside 'for ... next'.");
		return ERR_;
	}
}

DWORD c_engine::parse_exit_while (c_vector_table& last, DWORD stop_at)
{
	if (m_atom_table.is_parsing_while())
		return create_exit_atom(last, stop_at, exit_type::exit_while);
	else
	{
		error(CUR_ERR_LINE, "'exit while' illegal outside 'while ... wend'.");
		return ERR_;
	}
}

DWORD c_engine::parse_exit_do (c_vector_table& last, DWORD stop_at)
{
	if (m_atom_table.is_parsing_do())
		return create_exit_atom(last, stop_at, exit_type::exit_do);
	else
	{
		error(CUR_ERR_LINE, "'exit do' illegal outside 'do ... loop'.");
		return ERR_;
	}
}

DWORD c_engine::parse_endfunction(c_vector_table& last, DWORD stop_at)
{
	if (stop_at & ENDFUNCTION)		// we are here if 'function' is empty
	{
		c_end_routine_atom* p_atom = new c_end_routine_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

		m_atom_table.add(p_atom);

		while(c_atom** p_last = last.pop())
			*p_last = p_atom;

		gettok();				// ommit 'endsub'
		return ENDFUNCTION;
	}
	else
	{
		error(CUR_ERR_LINE, "'endfunction' without 'function'.");
		return ERR_;
	}
}

DWORD c_engine::parse_cls(c_vector_table& last, DWORD stop_at)
{
	c_cls_atom* p_atom = new c_cls_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	gettok();

	if (curtok.type != token_type::eof && curtok.type != token_type::eos)
	{
		error (CUR_ERR_LINE, "end of line expected after 'cls'.");
		delete p_atom;
		return ERR_;
	}

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	if (stop_at == SINGLE_ATOM) return SUCCESS;
	return TO_GO;
}

DWORD c_engine::parse_beep  (c_vector_table& last, DWORD stop_at)
{
	c_beep_atom* p_atom = new c_beep_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	gettok();

	if (stop_at == SINGLE_ATOM) return SUCCESS;

	if (curtok.type != token_type::eof && curtok.type != token_type::eos)
	{
		error (CUR_ERR_LINE, "end of line expected after 'beep'.");
		delete p_atom;
		return ERR_;
	}

	m_atom_table.add(p_atom);
	return TO_GO;
}

DWORD c_engine::parse_dim(c_vector_table& last, DWORD stop_at)
{
	bool b_bad_varnames = false;

	if (stop_at == SINGLE_ATOM)
	{
		error(CUR_ERR_LINE, "'dim' unexpected here.");
		return ERR_;
	}

	c_dim_atom* p_atom = new c_dim_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());
	VARENUM type;
	bool b_new;
	int size;
	c_str_array varnames;
	CLSID clsid;
	GUID libid;
	GUID iid;

	while (curtok.type != token_type::eos && curtok.type != token_type::eof)
	{
		size = 0;
		type = VT_EMPTY;
		b_new = false;
		memset(&clsid, 0, sizeof(clsid));
		memset(&libid, 0, sizeof(libid));
		memset(&iid, 0, sizeof(iid));

		gettok();					// variable name
		if (curtok.type != token_type::name)
		{
			error(CUR_ERR_LINE, "variable name expected after 'dim' (where %s found).", curtok.format());
			delete p_atom;
			return ERR_;
		}

		varnames.add(curtok.m_name);

		gettok();
		if (curtok.type == token_type::comma) continue;
		if (curtok.type == token_type::as_cond)
		{
			gettok(); // type/class or "new"
			if (curtok.type == token_type::new_cond)
			{
				b_new = true;
				gettok();
			}
			type = get_type(curtok.m_name);

			if (type == -1)
			{
				// try object
				c_string type_name = curtok.m_name;
				gettok();
				if (curtok.type == token_type::dot)
				{
					gettok();
					if (curtok.type == token_type::name)
					{
						type_name += '.';
						type_name += curtok.m_name;

						if (get_clsid_libid(type_name.get_buffer(), &clsid, &libid, &iid))
							type = VT_DISPATCH;
						else
						{
							error(CUR_ERR_LINE, "'%s' class is not registered in the system.", type_name.get_buffer());
							delete p_atom;
							return ERR_;
						}
					}
				}
				else
				{
					error(CUR_ERR_LINE, "unknown type: '%s'", type_name.get_buffer());
					delete p_atom;
					return ERR_;
				}
			}
			else if (b_new)
			{
				error(CUR_ERR_LINE, "'new' keyword can be used only with classes.");
				delete p_atom;
				return ERR_;
			}
		}
		else if (curtok.type != token_type::eos && curtok.type != token_type::eof)
		{
			error(CUR_ERR_LINE, "'as' expected after variable name.");
			delete p_atom;
			return ERR_;
		}

		if (type == VT_VARARRAY)
		{
			PUSH_CURTOK();

			gettok();
			if (curtok.type == token_type::lb)
			{
				gettok();
				if (curtok.type != token_type::integer)
				{
					error(CUR_ERR_LINE, "integer Array size expected where %s found.", curtok.format());
					delete p_atom;
					return ERR_;
				}
				size = curtok._int;

				gettok();
				if (curtok.type != token_type::rb)
				{
					error(CUR_ERR_LINE, "integer Array size expected where %s found.", curtok.format());
					delete p_atom;
					return ERR_;
				}
			}
			else
				POP_CURTOK();
		}

		c_dim_entry e;
		for (int i = 0; i < varnames.get_size(); i++)
		{
			if (!can_be_identifier(varnames[i], true)) b_bad_varnames = true;
			else if (is_declared_local_var(varnames[i]))
			{
				error(CUR_ERR_LINE, "attempt to redeclare variable '%s'.", varnames[i]);
				delete p_atom;
				return ERR_;
			}
			else
			{
				e.m_name		= varnames[i];
				e.m_type		= type;
				e.m_nsize		= size;
				e.m_bnew		= b_new;
				e.m_clsid		= clsid;
				e.m_libid		= libid;
				e.m_iid			= iid;

				p_atom->add(e);
				m_sub_namespace.add(e);
			}
		}

		varnames.empty();
		if (curtok.type == token_type::eos || curtok.type == token_type::eof) break;
		gettok();

		if (curtok.type != token_type::eos && curtok.type != token_type::eof && curtok.type != token_type::comma)
		{
			error(CUR_ERR_LINE, "',' or end of line expected after type in dim statement.");
			delete p_atom;
			return ERR_;
		}
	}

	if (b_bad_varnames)
	{
		// no need to send errors, because it is already done by 'can_be_var_name'
		delete p_atom;
		return ERR_;
	}

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	if (stop_at == SINGLE_ATOM) return SUCCESS;

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_select(c_vector_table& last, DWORD stop_at)
{
	gettok();
	if (curtok.type != token_type::case_cond)
	{
		error(CUR_ERR_LINE, "'case' expected after 'select'.");
		return ERR_;
	}

	gettok();
	c_expression* p_expr = _expr();
	if (!p_expr)
	{
		error(CUR_ERR_LINE, "cannot parse expression after 'select case'.");
		return ERR_;
	}

	if (curtok.type != token_type::eos)
	{
		error(CUR_ERR_LINE, "end of line expected after 'select case' expression.");
		delete p_expr;
		return ERR_;
	}

	gettok();
	if (curtok.type != token_type::case_cond)
	{
		error(CUR_ERR_LINE, "'case' expected where '%s' found.", curtok.format());
//		delete p_atom;
		return ERR_;
	}

	c_case_atom* p_atom = new c_case_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line() - 1);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	m_atom_table.add(p_atom);

	p_atom->set_expr(p_expr);

	c_case_expr_ptr_array expressions;
	bool b_else;
	int i, j;
	for (i = 0; ; i++)
	{
		expressions.reset();

		b_else = false;

		gettok();
		if (curtok.type == token_type::else_cond) // 'case else'
		{
			b_else = true;
			gettok(); // ommit 'else'
		}
		else
		{
			for (j = 0; ; j++)
			{
				c_expression* p_expr = _expr();
				if (!p_expr)
				{
					error(CUR_ERR_LINE, "canot parse expression after 'case'.");
//					expressions.delete_items();
//					delete p_atom;
					return ERR_;
				}

				c_case_expr* p_case_expr = new c_case_expr;
				p_case_expr->m_e1 = p_expr;
				expressions.add(p_case_expr);

				if (curtok.type == token_type::to_cond) // need to parse second expression of the range
				{
					gettok(); // ommit 'to'
					c_expression* p_expr2 = _expr();
					if (!p_expr2)
					{
						error(CUR_ERR_LINE, "canot parse expression after 'to'.");
//						expressions.delete_items();
//						delete p_atom;
						return ERR_;
					}

					p_case_expr->m_e2 = p_expr2;
				}
				if (curtok.type != token_type::comma) break;

				gettok(); // for next _expr()
			}
		}

		if (curtok.type != token_type::eos)
		{
			error(CUR_ERR_LINE, "end of line expected after case contitions.");
			return ERR_;
		}

		// parse branch statements
		c_atom* p_branch = 0;
		c_vector_table branch;
		branch.push(&p_branch);

		unsigned int n = _parse(branch, CASE | ENDSELECT);
		switch (n)
		{
		case CASE:
		case ENDSELECT:
			if (!p_branch)
			{
				error(CUR_ERR_LINE, "empty case'.");
				return ERR_;
			}

			if (b_else) p_atom->add_def_branch(p_branch);
			else p_atom->add_branch(p_branch, &expressions);
			last.append(&branch);
			last.push(&p_atom->m_pnext);

			if (n == ENDSELECT) goto _done;
			break;

		case ERR_:
//			delete p_atom;
//			expressions.delete_items();
			return ERR_;

		case EOF_TOKEN:
			error(CUR_ERR_LINE, "unexpected end of file while looking for 'case' or 'end select'.");
//			delete p_atom;
//			expressions.delete_items();
			return ERR_;

		default:_ASSERT(false);		// what else could stop us !?
		}

	}
_done:

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_dialog(c_vector_table& last, DWORD stop_at)
{
	c_dialog_atom* p_atom = new c_dialog_atom(&m_atom_table,
											  &m_call_stack,
											  this,
											  m_char_stream.cur_line(),
											  m_pInputDialog);

	gettok();
	if (curtok.type != token_type::string && curtok.type != token_type::eos)
	{
		error(CUR_ERR_LINE, "string or end of line expected after 'dialog'.");
		delete p_atom;
		return ERR_;
	}

	if (curtok.type == token_type::string)			// dialog's title
	{
		p_atom->set_title(curtok._string);

		gettok();
		if (curtok.type != token_type::eos)
		{
			error(CUR_ERR_LINE, "end of line expected where '%s' found", curtok.format());
			delete p_atom;
			return ERR_;
		}
	}

	for(;;)
	{
		do gettok();
		while (curtok.type == token_type::eos);

		c_string title;

		if (curtok.type == token_type::enddialog_cond) break;
		if (curtok.type == token_type::eof)
		{
			error(CUR_ERR_LINE, "unexpected end of file while looking for 'enddialog'.");
			delete p_atom;
			return ERR_;
		}

		if (curtok.type != token_type::string)
		{
			error(CUR_ERR_LINE, "string expected.");
			delete p_atom;
			return ERR_;
		}

		title = curtok._string;

		gettok();
		if (curtok.type != token_type::comma)
		{
			error(CUR_ERR_LINE, "',' expected.");
			delete p_atom;
			return ERR_;
		}

		gettok();
		if (curtok.type != token_type::name)
		{
			error(CUR_ERR_LINE, "variable name expected.");
			delete p_atom;
			return ERR_;
		}

		if (curtok.type == token_type::eof)
		{
			error(CUR_ERR_LINE, "unexpected end of file while looking for 'enddialog'.");
			delete p_atom;
			return ERR_;
		}

		p_atom->add(title, curtok.m_name);

		gettok();
		if (curtok.type != token_type::eos && curtok.type != token_type::eof)
		{
			error(CUR_ERR_LINE, "end of line expected after variable '%s'.", curtok.m_name.get_buffer());
			delete p_atom;
			return ERR_;
		}

	}

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	if (stop_at == SINGLE_ATOM) return SUCCESS;

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_while   (c_vector_table& last, DWORD stop_at)
{
	if (stop_at == SINGLE_ATOM)
	{
		error (CUR_ERR_LINE, "'while' statement must start from new line.");
		return ERR_;
	}

	c_while_atom* p_atom = new c_while_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	gettok();
	c_expression* p_expr = _expr();

	if (p_expr == 0)
	{
		error(CUR_ERR_LINE, "cannot parse expression after 'while'.");
		delete p_atom;
		return ERR_;
	}

	if (curtok.type != token_type::eos)
	{
		error (CUR_ERR_LINE, "end of line expected after 'while' condition.");
		delete p_atom;
		return ERR_;
	}

	p_atom->set_expr(p_expr);

	m_atom_table.add(p_atom);

	c_vector_table inside_loop;

	inside_loop.push(&p_atom->m_ploop);

	DWORD result = _parse(inside_loop, WEND);
	if (result != WEND)
	{
		if (result != ERR_)
			error (p_atom->m_nline, "'while' without 'wend'.");
		return ERR_;
	}

// now join ---------------------------

	while(c_atom** p_last = inside_loop.pop())
		*p_last = p_atom;

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pexit);

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_wend  (c_vector_table& last, DWORD stop_at)
{
	if (stop_at & WEND)		// we are here if 'while' is empty
	{
		gettok();			// ommit 'wend'
		if (curtok.type != token_type::eos && curtok.type != token_type::eof)
		{
			error (CUR_ERR_LINE, "end of line expected after 'wend'.");
			return ERR_;
		}

		return WEND;
	}
	else
	{
		error(CUR_ERR_LINE, "'wend' without 'while'");
		return ERR_;
	}
}

DWORD c_engine::parse_declare(c_vector_table& last, DWORD stop_at)
{
// Declare statements are parsed on prepare_dll_routines() stage,
// so just ommit all tokens till end of line.

	while (curtok.type != token_type::eos && curtok.type != token_type::eof)
		gettok();

	return TO_GO;
}

DWORD c_engine::parse_for(c_vector_table& last, DWORD stop_at)
{
	if (stop_at == SINGLE_ATOM)
	{
		error (CUR_ERR_LINE, "'for' statement must start from new line.");
		return ERR_;
	}

	c_for_atom* p_atom = new c_for_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());
	c_string varname;

	gettok();
	if (curtok.type != token_type::name)
	{
		error(CUR_ERR_LINE, "varible name expected after 'for'.");
		delete p_atom;
		return ERR_;
	}

	varname = curtok.m_name;

	if (!can_be_identifier(curtok.m_name, true)) return ERR_;
	if (!variable_declared(curtok.m_name.get_buffer())) return ERR_;

	gettok();		// ommit variable name
	if (curtok.type != token_type::assign)
	{
		error(CUR_ERR_LINE, "'=' expected after variable name");
		delete p_atom;
		return ERR_;
	}

	gettok();		// ommit '='
	c_expression* p_init_expr = _expr();

	if (p_init_expr == 0)
	{
		error(CUR_ERR_LINE, "cannot parse expression after 'for'");
		delete p_atom;
		return ERR_;
	}

	if (curtok.type != token_type::to_cond)
	{
		error(CUR_ERR_LINE, "'to' expected");
		delete p_atom;
		return ERR_;
	}

	gettok();
	c_expression* p_final_expr = _expr();

	if (p_final_expr == 0)
	{
		error(CUR_ERR_LINE, "cannot parse expression after 'to'");
		delete p_atom;		// this deletes p_init_expr
		return ERR_;
	}

	c_expression* p_step_expr = 0;

	if (curtok.type == token_type::step_cond)
	{
		gettok();
		p_step_expr = _expr();

		if (p_step_expr == 0)
		{
			error(CUR_ERR_LINE, "cannot parse expression after 'step'");
			delete p_atom;		// this deletes p_init_expr and p_final_expr
			return ERR_;
		}
	}

	if (curtok.type != token_type::eos)
	{
		error(CUR_ERR_LINE, "end of line expected");
		delete p_atom;		// this deletes p_init_expr, p_final_expr and p_step_expr
		return ERR_;
	}

	p_atom->set_var_name(varname);
	p_atom->set_init_expr(p_init_expr);
	p_atom->set_final_expr(p_final_expr);
	p_atom->set_step_expr(p_step_expr);

	m_atom_table.add(p_atom);

	c_vector_table inside_loop;
	char msg[MAX_ERR_LENGTH + 1];
	// @태그명 jkh 추가
	if (curtok.m_name[0] == '@')
	{
		if(!CheckValidTagName(&curtok.m_name.get_buffer()[1]))
		{
			_snprintf(msg, MAX_ERR_LENGTH, "태그명 '%s'가 존재하지 않습니다.", (char*)(LPCSTR)curtok.m_name);
			error(CUR_ERR_LINE, msg);
			delete p_atom;
			return ERR_;
		}
		return ERR_;
	}



	inside_loop.push(&p_atom->m_ploop);

	DWORD result = _parse(inside_loop, NEXT);
	if (result != NEXT)
	{
		if (result != ERR_)
			error (CUR_ERR_LINE, "'for' without 'next'", p_atom->m_nline);
//		delete p_atom;		// will be deleted in ~c_atom_table()
		return ERR_;
	}



// now join ---------------------------

	while(c_atom** p_last = inside_loop.pop())
		*p_last = p_atom;

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pexit);

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_next(c_vector_table& last, DWORD stop_at)
{
	if (stop_at & NEXT)		// we are here if 'for' is empty
	{
		gettok();			// ommit 'next'
		if (curtok.type != token_type::eos && curtok.type != token_type::eof)
		{
			error (CUR_ERR_LINE, "end of line expected after 'next'.");
			return ERR_;
		}
		return NEXT;
	}
	else
	{
		error(CUR_ERR_LINE, "'next' without 'for'");
		return ERR_;
	}
}

DWORD c_engine::parse_option()
{
	gettok();
	if (curtok.type != token_type::explicit_cond)
	{
		error (CUR_ERR_LINE, "'explicit' expected after 'option'.");
		return ERR_;
	}

	if (!m_atom_table.is_empty())
	{
		error (CUR_ERR_LINE, "no statements allowed before 'option explicit'.");
		return ERR_;
	}

	if (m_pcur_routine_entry)
	{
		error (CUR_ERR_LINE, "'option' cannot be used inside routines.");
		return ERR_;
	}

	if (m_bexplicit_set)
	{
		error (CUR_ERR_LINE, "duplicate 'explicit' option.");
		return ERR_;
	}

	m_bexplicit = true;
	m_bexplicit_set = true;

	gettok();

	if (curtok.type != token_type::eos && curtok.type != token_type::eof)
	{
		error (CUR_ERR_LINE, "end of line expected after 'option' statement.");
		return ERR_;
	}

	return TO_GO;
}

DWORD c_engine::parse_if(c_vector_table& last, DWORD stop_at)
{
	if (stop_at == SINGLE_ATOM)
	{
		error (CUR_ERR_LINE, "'if' statement must start from new line.");
		return ERR_;
	}

	c_if_atom* p_atom = new c_if_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	gettok();
	c_expression* p_expr = _expr();

	if (p_expr == 0)
	{
		error (CUR_ERR_LINE, "cannot parse expression after 'if'.");
		delete p_atom;
		return ERR_;
	}

	p_atom->m_pexpression = p_expr;

	if (curtok.type != token_type::then_cond)
	{
		error(CUR_ERR_LINE, "'then' expected.");
		delete p_atom;
		return ERR_;
	}

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	m_atom_table.add(p_atom);

	c_vector_table last_true;
	c_vector_table last_false;

	last_true.push(&p_atom->m_ptrue);
	last_false.push(&p_atom->m_pfalse);

	gettok();					//eos for multyline if

	if (curtok.type == token_type::eos)		// multyline if
	{

		switch (_then(last_true,  p_atom))
		{
		case ELSE:
			switch(_else(last_false, p_atom))
			{
			case EOF_TOKEN:
				error(CUR_ERR_LINE, "unexpected end of file while looking for 'endif'.");
				return ERR_;

			case ERR_:
			case ENDIF: break;

			default:_ASSERT(false);	// what was it ?!...
			}
			break;

		case EOF_TOKEN:
			error(CUR_ERR_LINE, "unexpected end of file while looking for 'else' or 'endif'.");
			return ERR_;

		case ERR_:
		case ENDIF:break;

		default:_ASSERT(false);		// what else could stop us !?
		}
	}
	else							// single line if
	{
		_parse(last_true, SINGLE_ATOM);

		switch (curtok.type)
		{
		case token_type::else_cond:
			gettok();			// ommit else
			_parse(last_false, SINGLE_ATOM);
			if (curtok.type != token_type::eos && curtok.type != token_type::eof)
			{
				error (CUR_ERR_LINE, "end of line expected.");
				return ERR_;
			}

		case token_type::eof:
		case token_type::eos : break;
		default  :
			error(CUR_ERR_LINE, "'else' or end of line expected where '%s' found.", curtok.format());
			return ERR_;
		}
	}

	last.append(&last_true);
	last.append(&last_false);

	gettok();
	return TO_GO;
}

DWORD c_engine::_then(c_vector_table& last, c_if_atom* p_if)
{
	return _parse(last, ELSE | ENDIF);
}

DWORD c_engine::_else(c_vector_table& last, c_if_atom* p_if)
{
	return _parse(last, ENDIF);
}

DWORD c_engine::parse_else(c_vector_table& last, DWORD stop_at)
{
	if (stop_at & ELSE)
	{
		gettok();			// ommit 'else'
		return ELSE;
	}
	else
	{
		error(CUR_ERR_LINE, "'else' without 'if'.");
		return ERR_;
	}
}

DWORD c_engine::parse_endif(c_vector_table& last, DWORD stop_at)
{
	if (stop_at & ENDIF)
	{
		gettok();			// ommit 'endif'
		return ENDIF;
	}
	else
	{
		error(CUR_ERR_LINE, "'endif' without 'if'.");
		return ERR_;
	}
}

DWORD c_engine::parse_case(c_vector_table& last, DWORD stop_at)
{
	if (stop_at & CASE)
	{
//		gettok();
		return CASE;
	}
	else
	{
		error(CUR_ERR_LINE, "'case' without 'select'.");
		return ERR_;
	}
}

DWORD c_engine::parse_endselect(c_vector_table& last, DWORD stop_at)
{
	if (stop_at & ENDSELECT)
	{
		gettok();
		return ENDSELECT;
	}
	else
	{
		error(CUR_ERR_LINE, "'end select' without 'select'.");
		return ERR_;
	}
}

bool c_engine::parse_dims(c_ptr_array<c_expression>& dims)
{
	_ASSERT(dims.get_size() == 0);

	while(1)
	{
		gettok();
		c_expression* p_expr = _expr();
		if (!p_expr)
		{
			dims.delete_items();
			return false;
		}

		dims.add(p_expr);

		switch (curtok.type)
		{
		case token_type::comma:
			break;

		case token_type::srb:
			if (!dims.get_size())
			{
				error(CUR_ERR_LINE, "no expressions inside '[]'.");
				dims.delete_items();
				return false;
			}
//			gettok();
			return true;

		default:
			error(CUR_ERR_LINE, "',' or ']' expected where '%s' found.", curtok.format());
			dims.delete_items();
			return false;
		}
	}

	return true;
}

DWORD c_engine::parse_assign(c_vector_table& last, DWORD stop_at)
{
	if (!can_be_identifier(curtok.m_name, true)) return ERR_;
	if (!variable_declared(curtok.m_name.get_buffer())) return ERR_;

	c_string varname;
	varname = curtok.m_name;
	char msg[MAX_ERR_LENGTH + 1];

	c_assign_atom* p_atom = new c_assign_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	if (curtok.m_name[0] == '@')
	{
		if (!CheckValidTagName(&curtok.m_name.get_buffer()[1]))
		{
			_snprintf(msg, MAX_ERR_LENGTH, "태그명 '%s'가 존재하지 않습니다.", (char*)(LPCSTR)curtok.m_name);
			error(CUR_ERR_LINE, msg);
			delete p_atom;
			return ERR_;
		}
	}

	gettok();

	switch (curtok.type)
	{
	case token_type::assign:
		break;

	case token_type::slb:
		{
			if (!is_declared_array(varname.get_buffer()))
			{
				error(CUR_ERR_LINE, "illegal []. variable '%s' is not Array.", varname.get_buffer());
				delete p_atom;
				return ERR_;
			}

			c_ptr_array<c_expression> dims;
			if (!parse_dims(dims))
			{
				delete p_atom;
				return ERR_;
			}

			gettok(); // ommit ']'

			if (curtok.type != token_type::assign)
			{
				error(CUR_ERR_LINE, "'=' expected after variable '%s'.", varname);
				delete p_atom;
				return ERR_;
			}

			int i;
			for (i = 0; i < dims.get_size(); i++)
				p_atom->add_dim(dims[i]);
		}
		break;

	default:
		//error(CUR_ERR_LINE, "'=' expected after variable '%s'.", varname);
		error(CUR_ERR_LINE, "unknown procedure '%s' or invalid assignment to variable '%s'.",
			varname.get_buffer(), varname.get_buffer());
		delete p_atom;
		return ERR_;
	}

	gettok();
	c_expression* p_expr = _expr();

	if (p_expr == 0)
	{
		error(CUR_ERR_LINE, "cannot parse expression after variable '%s'.", varname.get_buffer());
		delete p_atom;
		return ERR_;
	}

	if (stop_at != SINGLE_ATOM)
		if (curtok.type != token_type::eof && curtok.type != token_type::eos)
		{
			error(CUR_ERR_LINE, "%s unexpected here.", curtok.format());
			delete p_atom;
			return ERR_;
		}

	p_atom->set_expression(p_expr);
	p_atom->set_var_name(varname);

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	if (stop_at == SINGLE_ATOM) return SUCCESS;

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_end(c_vector_table& last, DWORD stop_at)
{
	while(c_atom** p_last = last.pop())
		*p_last = 0;

	gettok();
	if (stop_at == SINGLE_ATOM) return SUCCESS;

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_label (c_vector_table& last, DWORD stop_at)
{
	c_label_atom* p_atom = new c_label_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	p_atom->set_label_name(curtok.m_name);

	if (m_pcur_routine_entry)
		p_atom->set_func_name(m_pcur_routine_entry->get_var_desc()->get_name());

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_goto (c_vector_table& last, DWORD stop_at)
{
	c_goto_atom* p_atom = new c_goto_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	if ((token_type)gettok() != token_type::name)
	{
		error(CUR_ERR_LINE, "label name expected after 'goto'.");
		delete p_atom;
		return ERR_;
	}

	c_string label_name;
	label_name = curtok.m_name;

	p_atom->set_label_name(label_name);

	if (m_pcur_routine_entry)
		p_atom->set_func_name(m_pcur_routine_entry->get_var_desc()->get_name());

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

//	last.push(&p_atom->m_pnext);		// goto will be resolved in _post_parse();

	gettok();
	if (stop_at == SINGLE_ATOM) return SUCCESS;

	if (curtok.type != token_type::eos && curtok.type != token_type::eof)
	{
		error(CUR_ERR_LINE, "end of line expected after label '%s'.", label_name);
//		delete p_atom;					// will be deleted in ~c_atom_table()
		return ERR_;
	}
	return TO_GO;
}

DWORD c_engine::parse_print(c_vector_table& last, DWORD stop_at)
{
	c_print_atom* p_atom = new c_print_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());

	for (;;)
	{
		gettok();

		if (curtok.type == token_type::eos || curtok.type == token_type::eof ||
			curtok.type == token_type::comma || curtok.type == token_type::semi)
			goto _end_args;

		c_expression* p_expr = _expr();
		if (p_expr == 0)
		{
			delete p_atom;
			return ERR_;
		}

		p_atom->add(p_expr);

		switch (curtok.type)
		{
		case token_type::eos:
		case token_type::eof: goto _end_args;

		case token_type::comma:
		case token_type::semi: continue;

		case token_type::string:
		case token_type::integer:
		case token_type::floating:
		case token_type::name:
		case token_type::dot:
			{
				error(CUR_ERR_LINE, "%s unexpected here.", curtok.format());
				delete p_atom;
				return ERR_;
			}

		default: goto _end_args;
		}
	}

_end_args:

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	if (stop_at == SINGLE_ATOM) return SUCCESS;

	gettok();
	return TO_GO;
}

DWORD c_engine::parse_input(c_vector_table& last, DWORD stop_at)
{
	c_input_atom* p_atom = new c_input_atom(&m_atom_table,
											&m_call_stack,
											this,
											m_char_stream.cur_line());

	gettok();		// first token - string or c_variable

	switch(curtok.type)
	{
	case token_type::string:
		p_atom->set_string(curtok._string);
		break;

	case token_type::name:
		p_atom->add_var(curtok.m_name);
		break;

	case token_type::eof:
	case token_type::eos: goto _end_of_loop;

	default:
		error(CUR_ERR_LINE, "string or variable name expected after 'input'.");
		delete p_atom;
		return ERR_;
	}

	gettok();

	if (curtok.type != token_type::eos && curtok.type != token_type::eof)
		for (;;)
		{
			switch(curtok.type)
			{
			case token_type::comma:
			case token_type::semi:
				break;

			case token_type::eof:
			case token_type::eos: goto _end_of_loop;

			case token_type::name:
				p_atom->add_var(curtok.m_name);
				break;

			default:
				error(CUR_ERR_LINE, "variable name expected in 'input'.");
				delete p_atom;
				return ERR_;

			}
			gettok();
		}

_end_of_loop:

	m_atom_table.add(p_atom);

	while(c_atom** p_last = last.pop())
		*p_last = p_atom;

	last.push(&p_atom->m_pnext);

	if (stop_at == SINGLE_ATOM) return SUCCESS;

	gettok();
	return TO_GO;
}

// expression parsing ---------------------------------------------------------

c_expression* c_engine::_primary()
{
	c_expression* left = new c_expression(&m_call_stack, &m_atom_table, this);
	CComPtr<IDispatch> lpd;

	switch(curtok.type)
	{
	case token_type::integer:
		left->m_action = c_action::_const;
		left->m_constant = curtok._int;
		gettok();				// ommit number
		break;

	case token_type::floating:
		left->m_action = c_action::_const;
		left->m_constant = curtok._float;
		gettok();				// ommit number
		break;

	case token_type::string:
		left->m_action = c_action::_const;
		left->m_constant = curtok._string;
		gettok();				// ommit string
		break;

	case token_type::name:
		lpd = get_default_parent(curtok.m_name);
		if (lpd)
		{
			bool b_var;
			CComPtr<ITypeInfo> pti = get_type_info(lpd);
			c_disp_get* p_disp_get = disp_get(b_var, pti);

			if (!p_disp_get)
			{
				delete left;
				left = 0;
			}
			else if (p_disp_get->is_sub())
			{
				error(CUR_ERR_LINE, "sub cannot be used in expression.");
				delete left;
				left = 0;
			}
			else
			{
				left->m_action = c_action::_disp_get;
				left->set_disp_get(p_disp_get);
			}
		}
		else if (is_ext_object(curtok.m_name))
		{
			bool b_var;
			c_disp_get* p_disp_get = disp_get(b_var, 0);

			if (!p_disp_get)
			{
				if (b_var)
				{
//					if (can_be_identifier(curtok.m_name, true))
//					{
						if (variable_declared(curtok.m_name.get_buffer()))
						{
							left->m_action = c_action::_variable;
							left->set_var_name(curtok.m_name);
						}
						else
						{
							delete left;
							left = 0;
						}
//					}
//					else
//					{
//						delete left;
//						left = 0;
//					}
				}
				else
				{
					delete left;
					left = 0;
				}
			}
			else
			{
				left->m_action = c_action::_disp_get;
				left->set_disp_get(p_disp_get);
			}
		}
		else if (is_ext_const(curtok.m_name))
		{
			left->m_action = c_action::_ext_const;
			int n_index = m_ext_consts.find(curtok.m_name);
			_ASSERT(n_index != -1);
			left->m_pext_const = m_ext_consts.vars.get_ptr(n_index);
			gettok();				// ommit constant name
			break;
		}
		else if (is_ext_func(curtok.m_name))
		{
			c_ext_func* p_ext_func = _ext_func();

			if (p_ext_func == 0)
			{
				delete left;
				left = 0;
			}
			else
			{
				left->m_action = c_action::_ext_function;
				left->set_ext_func(p_ext_func);
			}
		}
		else if (is_dll_func(curtok.m_name))
		{
			c_dll_caller* p_caller = _dll_caller();

			if (p_caller == 0)
			{
				delete left;
				left = 0;
			}
			else
			{
				left->m_action = c_action::_dll_function;
				left->set_dll_caler(p_caller);
			}
		}
		else if (is_function(curtok.m_name))
		{
			c_string func_name;
			func_name = curtok.m_name;

			//int n_pos = m_char_stream.pos();
			PUSH_CURTOK();
			gettok();	// see if '(' is the next token

			// this is not a function call. variable name is used in expression.
			if (curtok.type != token_type::lb)
			{
				if (m_atom_table.get_routine_name() &&
					!_stricmp(func_name.get_buffer(), m_atom_table.get_routine_name()))
				{
					left->m_action = c_action::_variable;
					left->set_var_name(func_name.get_buffer());

					if (curtok.type == token_type::slb)
					{
						if (!is_declared_array(func_name.get_buffer()))
						{
							error(CUR_ERR_LINE, "illegal []. variable '%s' is not Array.", func_name.get_buffer());
							delete left;
							left = 0;
						}
						else
						{
							c_ptr_array<c_expression> dims;
							if (!parse_dims(dims))
							{
								delete left;
								left = 0;
							}
							else
							{
								_ASSERT(curtok.type == token_type::srb); // parse_dims must do this check and return false if not.
								gettok(); // ommit '['

								int i;
								for (i = 0; i < dims.get_size(); i++)
									left->add_dim(dims[i]);
							}
						}
					}
				}
				else
				{
					error(CUR_ERR_LINE, "'(' expected after function '%s'", func_name.get_buffer());
					delete left;
					left = 0;
				}

				break;
			}
			// OK, this is function all
			else
			{
				left->m_action = c_action::_function;

				// go back
				//m_char_stream.pos(n_pos);
				//curtok = prevtok;
				POP_CURTOK();

				// and parse arguments
				c_ptr_array<c_expression> expressions;

				if (!parse_arg_list(expressions,
								  curtok.m_name,
								  0,
								  false))
				{
					delete left;
					left = 0;
				}
				else
				{
					for (int i = 0; i < expressions.get_size(); i++)
						left->add_func_expr(expressions[i]);

					// try to see if there are [] after argument list
					// try to get next token
					//n_pos = m_char_stream.pos();
					PUSH_CURTOK();
					gettok();

					if (curtok.type == token_type::slb)
					{
						// make sure function is of Array type
						if (get_routine_type(func_name.get_buffer()) != VT_VARARRAY)
						{
							error(CUR_ERR_LINE, "[] illegal here. function '%s' is not of Array type.", func_name.get_buffer());
							delete left;
							left = 0;
						}
						else
						{
							gettok();
							c_expression* p_dim = _expr();
							if (!p_dim)
							{
								error(CUR_ERR_LINE, "cannot parse expression inside [] after '%s' function.", func_name.get_buffer());
								delete left;
								left = 0;
							}
							else
							{
								if (curtok.type != token_type::srb)
								{
									error(CUR_ERR_LINE, "']' expected where '%s' found. (after '%s' function)", curtok.format(), func_name.get_buffer());
									delete left;
									delete p_dim;
									left = 0;
								}
								else
									left->add_dim(p_dim);
							}
						}
					}
					else
					{
						// go back
						POP_CURTOK();
					}
				}

				if (left)
				{
					m_func_expressions.add(left);
					m_func_names.add(func_name);
					m_line_numbers.add_const(m_char_stream.cur_line());

					int n_args = get_routine_arg_count(func_name);
					_ASSERT(n_args != -1);
					if (n_args != left->get_func_expr_count())
					{
						error(CUR_ERR_LINE, "function '%s' does takes %d argument(s).",
							func_name.get_buffer(), n_args);
						delete left;
						left = 0;
					}

/*
					c_routine_entry_atom* p_entry = m_atom_table.get_routine_entry(func_name, false);
					if (!p_entry)
					{
						error(CUR_ERR_LINE, "function '%s' must be declared before it is used.", func_name.get_buffer());
						delete left;
						left = 0;
					}
					else
					{
						left->set_func_entry(p_entry);

						if (left->get_func_expr_count() != p_entry->get_arg_count())
						{
							error(CUR_ERR_LINE, "function '%s' does not take %d argument(s).",
								func_name.get_buffer(), left->get_func_expr_count());
							delete left;
							left = 0;
						}
					}
*/
				}
			}

		}
		else if (can_be_identifier(curtok.m_name, true))
		{
			CLSID clsid = {0};
			GUID  libid = {0};

			if (is_object_variable(curtok.m_name.get_buffer(), &clsid, &libid))
			{
				USES_CONVERSION;

				_ASSERT(!is_NULL_GUID(&libid));
				ITypeLib* ptl = load_typelib(&libid);
				if (ptl)
				{
					ITypeInfo* pti = 0;
					HRESULT hr = ptl->GetTypeInfoOfGuid(clsid, &pti);
					_ASSERT(!FAILED(hr) && pti);

					bool b_var;
					c_disp_get* p_disp_get = disp_get(b_var, pti);

					if (b_var && !p_disp_get)
					{
						if (variable_declared(curtok.m_name.get_buffer()))
						{
							left->m_action = c_action::_variable;
							left->set_var_name(curtok.m_name);
						}
						else
						{
							delete left;
							left = 0;
							_ASSERT(0); // todo: report error?
						}
					}
					else if (!p_disp_get)
					{
						delete left;
						left = 0;
					}
					else if (p_disp_get->is_sub())
					{
						error(CUR_ERR_LINE, "sub cannot be used in expression.");
						delete left;
						left = 0;
					}
					else
					{
						left->m_action = c_action::_disp_get;
						left->set_disp_get(p_disp_get);
					}

					if (pti) pti->Release();
					if (ptl) ptl->Release();
				}
				else
				{
					error(CUR_ERR_LINE, "cannot load type library for object '%s'.", curtok.m_name.get_buffer());
					delete left;
					left = 0;
				}
			}
			else if (variable_declared(curtok.m_name.get_buffer()))
			{
				left->m_action = c_action::_variable;
				left->set_var_name(curtok.m_name);

				// try []

				//int n_pos = m_char_stream.pos();
				PUSH_CURTOK();

				gettok();
				if (curtok.type != token_type::slb) // no, this is not '['. rewind.
				{
					//curtok = prevtok;
					//m_char_stream.pos(n_pos);
					POP_CURTOK();
				}
				else					// yes, it's '['. parse indexes
				{
					if (!is_declared_array(curtok.m_name.get_buffer()))
					{
						error(CUR_ERR_LINE, "illegal []. variable '%s' is not Array.", curtok.m_name.get_buffer());
						delete left;
						left = 0;
					}
					else
					{
						c_ptr_array<c_expression> dims;
						if (!parse_dims(dims))
						{
							delete left;
							left = 0;
						}
						else
						{
							int i;
							for (i = 0; i < dims.get_size(); i++)
								left->add_dim(dims[i]);
						}
					}
				}
			}
			else
			{
				delete left;
				left = 0;
			}
		}
		else
		{
			if (curtok.m_name[0] == '@' )
			{
				if (!CheckValidTagName(&curtok.m_name.get_buffer()[1]))
				{
					char msg[MAX_ERR_LENGTH + 1] = { 0, };

					error(CUR_ERR_LINE, "태그명 '%s'가 존재하지 않습니다.", curtok.m_name.get_buffer());
					delete left;
					return NULL;
				}

				left->m_action = c_action::_variable;
				left->set_var_name(curtok.m_name);
				/*
				left->m_constant.nDrv = nDrv;
				left->m_constant.integer = nType;
				left->m_constant.floating = (float)nIndex;
				left->m_constant.mode = nMode;
				left->m_constant.mode2 = nMode2;
				left->m_constant.m_fromEngine = fromEngine;

				left->m_constant.nType = nType;
				left->m_constant.nPos = nIndex;
				*/
			}
			else
			{
				delete left;
				left = 0;
			}
		}
		gettok();

		break;

	case token_type::lb:
		delete left;
		gettok();			// ommit '('
		left = _expr();		// expression inside '('

		if(curtok.type!=(token_type)')')
		{
			error(CUR_ERR_LINE, "missing ')'");
			delete left;
			left = 0;
		}
		else gettok();
		break;

	case token_type::minus:								// negative value
		{
			gettok();						// ommit '-'
			c_expression* p_expr = _primary();	// expression after '-'
			if (p_expr == 0)				// bad expression
			{
				delete left;
				left = 0;
				break;
			}

			left->m_pright = p_expr;
			left->m_action = c_action::_neg;
		}
		break;

	case token_type::plus:					// plus before digit
		gettok();				// ommit '+'
		delete left;			// ignore '+' and
		left = _primary();			// just try again
		break;

	case token_type::not_op:
		{
			gettok();						// ommit 'not'
			c_expression* p_expr = _primary();	// expression after 'not'
			if (p_expr == 0)				// bad expression
			{
				delete left;
				left = 0;
				break;
			}

			left->m_pright = p_expr;
			left->m_action = c_action::_not;
		}
		break;

	case token_type::system:

		break;

	case token_type::eos:
		error(CUR_ERR_LINE, "unexpected end of line in expression.");
		delete left;
		left = 0;
		break;

	case token_type::eof:
		error(CUR_ERR_LINE, "unexpected end of file in expression.");
		delete left;
		left = 0;
		break;

	default:
		error(CUR_ERR_LINE, "'%s' unexpected here.", curtok.format());
		delete left;
		left = 0;
	}

	return left;
}

c_expression* c_engine::_term()
{
	c_expression* left = _primary();
	if (left == 0) return left;

	for(;;)
		switch(curtok.type)
		{
		case token_type::multiply:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _primary();
				expr->m_action = c_action::_mul;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		case token_type::divide:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _primary();
				expr->m_action = c_action::_div;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		case token_type::reminder:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _primary();
				expr->m_action = c_action::_mod;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		case token_type::power:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _primary();
				expr->m_action = c_action::_pow;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		default: return left;
		}
}

c_expression* c_engine::_mathem()
{
	c_expression* left = _term();
	if (left == 0) return left;

	for(;;)
		switch(curtok.type)
		{
		case token_type::plus:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _term();
				expr->m_action = c_action::_plus;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		case token_type::minus:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _term();
				expr->m_action = c_action::_minus;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		case token_type::cat:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _term();
				expr->m_action = c_action::_cat;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		default: return left;
		}
}

c_expression* c_engine::_bool()
{
	c_expression* left = _mathem();
	if (left == 0) return left;

	for(;;)
		switch(curtok.type)
		{
		case token_type::greater:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _mathem();
				expr->m_action = c_action::_greater;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		case token_type::less:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _mathem();
				expr->m_action = c_action::_less;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		case token_type::greatere:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _mathem();
				expr->m_action = c_action::_greatere;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		case token_type::lesse:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _mathem();
				expr->m_action = c_action::_lesse;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		case token_type::equal:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _mathem();
				expr->m_action = c_action::_equal;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		case token_type::notequal:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _mathem();
				expr->m_action = c_action::_notequal;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
			break;

		default: return left;
		}
}

c_expression* c_engine::_expr_and()
{
	c_expression* left = _bool();
	if (left == 0) return left;

	for(;;)
		switch(curtok.type)
		{
		case token_type::and_op:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _bool();
				expr->m_action = c_action::_and;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
		    break;

		default: return left;
	 }
}

c_expression* c_engine::_expr()
{
	c_expression* left = _expr_and();
	if (left == 0) return left;

	for(;;)
		switch(curtok.type)
		{
		case token_type::xor_op:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _expr_and();
				expr->m_action = c_action::_xor;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
		    break;

		case token_type:: or_op:
			{
				gettok();
				c_expression* expr = new c_expression(&m_call_stack, &m_atom_table, this);
				expr->m_pleft = left;
				expr->m_pright = _expr_and();
				expr->m_action = c_action::_or;
				left = expr;
				if (left->m_pright == 0 || left->m_pleft == 0)
				{
					delete left;
					left = 0;
				}
			}
		    break;

		default:
			{
				// this should not follow expression
				if (curtok.type == token_type::lb ||
					curtok.type == token_type::slb ||
					curtok.type == token_type::string ||
					curtok.type == token_type::integer ||
					curtok.type == token_type::name ||
					curtok.type == token_type::dot ||
					curtok.type == token_type::floating)
				{
					error(CUR_ERR_LINE, "%s unexpected here.", curtok.format());
					delete left;
					left = 0;
				}
				return left;
			}
	 }
}

//-----------------------------------------------------------------------------
// _post_parse ----------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_engine::_post_parse()
{
	bool b_result = true;
	int i;
	int j;

// look for duplicate labels --------------------------------------------------

	for (i = 0; i < m_atom_table.get_count(); i++)
	{
		c_atom* p_atom = m_atom_table.get_atom(i);
		if (p_atom->m_type == atom_type::label_atom)
		{
			c_label_atom* p_label_atom = (c_label_atom*) p_atom;

			for (j = 0; j < m_atom_table.get_count(); j++)
			{
				if (j == i) continue;

				p_atom = m_atom_table.get_atom(j);
				if (p_atom->m_type == atom_type::label_atom)
				{
					c_label_atom* p_label_atom2 = (c_label_atom*) p_atom;

					if (strcmp(p_label_atom->m_func_name, p_label_atom2->m_func_name) == 0 &&
						strcmp(p_label_atom->m_label_name, p_label_atom2->m_label_name) == 0)
					{
						b_result = false;
						error(p_label_atom->m_nline, "duplicate label name : '%s'.", p_label_atom->m_label_name);
					}
				}
			}
		}
	}

// join gotos and labels ------------------------------------------------------

	for (i = 0; i < m_atom_table.get_count(); i++)
	{
		c_atom* p_atom = m_atom_table.get_atom(i);
		if (p_atom->m_type == atom_type::goto_atom)
		{
			c_goto_atom* p_goto_atom = (c_goto_atom*) p_atom;

			for (j = 0; j < m_atom_table.get_count(); j++)
			{
				p_atom = m_atom_table.get_atom(j);
				if (p_atom->m_type == atom_type::label_atom)
				{
					c_label_atom* p_label_atom = (c_label_atom*) p_atom;
					if (strcmp(p_goto_atom->m_func_name, p_label_atom->m_func_name) == 0 &&
						strcmp(p_goto_atom->m_label_name, p_label_atom->m_label_name) == 0)
					{
						p_goto_atom->m_pnext = p_label_atom;
					}
				}
			}

			if (p_goto_atom->m_pnext == 0)
			{
				error(p_goto_atom->m_nline, "label '%s' not found (or not in this routine).", p_goto_atom->m_label_name);
				b_result = false;
				continue;
			}
		}
	}

// join exit atoms ------------------------------------------------------------

	for (i = 0; i < m_atom_table.get_count(); i++)
	{
		c_atom* p_atom = m_atom_table.get_atom(i);
		if (p_atom->m_type == atom_type::exit_atom)
		{
			c_exit_atom* p_exit_atom = (c_exit_atom*)p_atom;
			int n_exit_line = p_atom->m_nline;

			switch (p_exit_atom->get_type())
			{
			case exit_type::exit_routine:
				for (j = i; j < m_atom_table.get_count(); j++)
				{
					p_atom = m_atom_table.get_atom(j);
					if (p_atom->m_type == atom_type::end_routine_atom)
					{
						//_ASSERT(!p_exit_atom->m_pnext);
						p_exit_atom->m_pnext = p_atom;
						break;
					}
				}
				break;

			case exit_type::exit_for:
				{
					c_for_atom* p_for_atom = (c_for_atom*)m_atom_table.find_prev_atom(atom_type::for_atom, n_exit_line);
					_ASSERT(p_for_atom); // THIS MUST NOT FAIL!!!
					p_exit_atom->m_pnext = p_for_atom->m_pexit;
					p_exit_atom->m_pfor = p_for_atom;
				}
				break;

			case exit_type::exit_while:
				{
					c_while_atom* p_while_atom = (c_while_atom*)m_atom_table.find_prev_atom(atom_type::while_atom, n_exit_line);
					_ASSERT(p_while_atom); // THIS MUST NOT FAIL!!!
					p_exit_atom->m_pnext = p_while_atom->m_pexit;
				}
				break;

			case exit_type::exit_do:
				{
					c_do_atom* p_do_atom = (c_do_atom*)m_atom_table.find_prev_atom(atom_type::do_atom, n_exit_line);
					_ASSERT(p_do_atom); // THIS MUST NOT FAIL!!!
					p_exit_atom->m_pnext = p_do_atom->m_pexit;
				}
				break;

			default:
				_ASSERT(0);
				break;
			}
		}
	}

// join func expressions ------------------------------------------------------

	_ASSERT(m_func_expressions.get_size() == m_func_names.get_size());
	_ASSERT(m_func_expressions.get_size() == m_line_numbers.get_size());

	for (i = 0; i < m_func_expressions.get_size(); i++)
	{
		c_expression* p_expression = m_func_expressions[i];
		_ASSERT(p_expression);
		_ASSERT(p_expression->m_action == c_action::_function);

		c_routine_entry_atom* p_entry = m_atom_table.get_routine_entry(m_func_names[i], false);
		if (!p_entry)
		{
			_ASSERT(0);
			error(NO_ERR_LINE, "unexpected error joining function call to '%s'.", m_func_names[i]);
			b_result = false;
		}
		else
		{
			p_expression->set_func_entry(p_entry);

			if (p_expression->get_func_expr_count() != p_entry->get_arg_count())
			{
				error(m_line_numbers[i], "function '%s' does not take %d argument(s).",
					m_func_names[i], p_expression->get_func_expr_count());
				b_result = false;
			}
		}
	}

	m_func_expressions.reset();
	m_func_names.empty();
	m_line_numbers.reset();

// join calls and subs --------------------------------------------------------

	for (i = 0; i < m_atom_table.get_count(); i++)
	{
		c_atom* p_atom = m_atom_table.get_atom(i);

		if (p_atom->m_type == atom_type::call_sub_atom)
		{
			c_call_sub_atom*  p_call_sub_atom = (c_call_sub_atom*) p_atom;
			c_routine_entry_atom* p_sub_entry_atom = m_atom_table.get_routine_entry(p_call_sub_atom->m_sub_name, true);
			if (!p_sub_entry_atom)p_sub_entry_atom = m_atom_table.get_routine_entry(p_call_sub_atom->m_sub_name, false);

			if (p_sub_entry_atom != 0)
			{
				p_call_sub_atom->set_sub_entry(p_sub_entry_atom);

				if (p_sub_entry_atom->get_arg_count() < p_call_sub_atom->get_arg_count())
				{
					error (p_call_sub_atom->m_nline, "too many parameters in sub/function call.");
					b_result = false;
				}

				if (p_sub_entry_atom->get_arg_count() > p_call_sub_atom->get_arg_count())
				{
					error (p_call_sub_atom->m_nline, "not enough parameters in sub/function call.");
					b_result = false;
				}
			}
			else
			{
				error (p_call_sub_atom->m_nline, "sub '%s' not found", p_call_sub_atom->m_sub_name);
				b_result = false;
			}
		}
	}

// connect bodies to routines -------------------------------------------------

	for (i = 0; i < m_atom_table.get_count(); i++)
	{
		c_atom* p_atom = m_atom_table.get_atom(i);

		if (p_atom->m_type == atom_type::routine_entry_atom)
		{
			c_routine_entry_atom* p_entry = (c_routine_entry_atom*) p_atom;

			for (j = i + 1; j < m_atom_table.get_count(); j++)
			{
				c_atom* p_atom = m_atom_table.get_atom(j);

				if (p_atom->m_type == atom_type::end_routine_atom)
				{
					c_end_routine_atom* p_end = (c_end_routine_atom*) p_atom;
					p_end->set_entry(p_entry);
					break;
				}
				else
				{
					p_entry->add_body(p_atom);
				}
			}
		}
	}

// find entry atom ------------------------------------------------------------

	c_atom* p_entry_atom = 0;
	bool b_subs_are_present = false;

	int n_atoms = m_atom_table.get_count();

	if (n_atoms == 0)
	{
		error(NO_ERR_LINE, "source is empty. nothing to execute.");
		b_result = false;
	}

	_ASSERT(p_entry_atom == 0);

	for (i = 0; i < n_atoms; i++)
	{
		c_atom* p_atom = m_atom_table.get_atom(i);

		if (p_atom->m_type == atom_type::routine_entry_atom)
			b_subs_are_present = true;							// we have subs in our source

		if (p_atom->m_type == atom_type::end_routine_atom)
			if (n_atoms - i > 1)								// is there is something after 'endsub'
				p_entry_atom = m_atom_table.get_atom(i + 1);	// entry is after last 'endsub'
	}
/*
	if (b_subs_are_present)
	{
		if (p_entry_atom == 0)
		{
			error(NO_ERR_LINE, "cannot find entry after last 'endsub'.");
			b_result = false;
		}
		else
			m_atom_table.m_pentry_atom = p_entry_atom;
	}
*/
	return b_result;
}

// engine_parse.cpp에 추가할 함수들

DWORD c_engine::parse_system_command(c_vector_table& last, DWORD stop_at)
{
	// $System으로 시작하는 명령 처리
	c_string command = curtok.m_name;
	DebugLog("$System 명령 파싱: %s", command.get_buffer());

	bool is_assignment = false;
	c_string left_side;
	c_string right_side;

	int equals_pos = -1;
	for (int i = 0; i < command.get_length(); i++) {
		if (command.get_buffer()[i] == '=') {
			equals_pos = i;
			break;
		}
	}

	bool is_function_call = (strstr(command.get_buffer(), ".ResetData()") != NULL ||
		strstr(command.get_buffer(), ".Resetdata()") != NULL ||
		strstr(command.get_buffer(), ".resetdata()") != NULL ||
		strstr(command.get_buffer(), ".RESETDATA()") != NULL);

	if (equals_pos >= 0) {
		is_assignment = true;
		DebugLog("할당 명령 감지됨");

		char left_buffer[1024] = { 0 };
		strncpy_s(left_buffer, command.get_buffer(), equals_pos);
		left_side = left_buffer;

		right_side = command.get_buffer() + equals_pos + 1;
		while (*right_side.get_buffer() && isspace(*right_side.get_buffer())) {
			right_side = right_side.get_buffer() + 1;
		}

		DebugLog("좌변: %s, 우변: %s", left_side.get_buffer(), right_side.get_buffer());

		// c_assign_atom 생성
		c_assign_atom* p_atom = new c_assign_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());
		p_atom->set_system_path(left_side.get_buffer());
		p_atom->is_system_object(true);
		p_atom->set_var_name(left_side.get_buffer());

		// 여기서 중요한 디버그 로그 추가
		DebugLog("생성된 atom 정보: 타입=%d, 라인=%d, 주소=%p",
			p_atom->m_type, p_atom->m_nline, p_atom);

		// 표현식 처리
		c_expression* p_expr = parse_system_expression(right_side.get_buffer(), left_side.get_buffer());
		if (p_expr) {
			p_atom->set_expression(p_expr);
		}

		class VariantGuard {
		public:
			VariantGuard() { VariantInit(&var); }
			~VariantGuard() { VariantClear(&var); }
			VARIANT var;
		} vg;

		//System_SetProperty(left_side.get_buffer(), &vg.var);

		//p_expr->m_constant.vt = VT_BOOL;

		c_variable expr_value;
		p_expr->exec(&expr_value);

		// 이제 실제 값을 사용
		switch (expr_value.vt)
		{
		case VT_BOOL:
			vg.var.vt = VT_BOOL;
			vg.var.boolVal = expr_value.boolVal ? VARIANT_TRUE : VARIANT_FALSE;
			break;
		case VT_I4:
			vg.var.vt = VT_I4;
			vg.var.lVal = expr_value.lVal;
			break;
		case VT_BSTR:
			vg.var.vt = VT_BSTR;
			vg.var.bstrVal = SysAllocString(expr_value.bstrVal);
			break;
		default:
			DebugLog("오류: 지원되지 않는 표현식 타입 (타입: %d)", expr_value.vt);
			error(m_char_stream.cur_line(), "지원되지 않는 속성 값 타입이 사용되었습니다");
			delete p_atom;
			return ERR_;
		}

		m_atom_table.add(p_atom);

		while (c_atom** p_last = last.pop())
			*p_last = p_atom;

		last.push(&p_atom->m_pnext);
	}

	else if (is_function_call) {
		// 함수 호출 명령 처리 (예: $System.Graphic("그래픽명").Object("객체명").Resetdata())
		DebugLog("함수 호출 명령 감지됨: %s", command.get_buffer());

		// c_assign_atom 생성 (함수 호출용)
		c_assign_atom* p_atom = new c_assign_atom(&m_atom_table, &m_call_stack, this, m_char_stream.cur_line());
		p_atom->set_system_path(command.get_buffer());
		p_atom->is_system_object(true);
		p_atom->set_var_name(command.get_buffer());

		// 함수 호출에 대한 표현식 생성
		c_expression* p_expr = parse_system_expression("", command.get_buffer());
		if (p_expr) {
			p_atom->set_expression(p_expr);
		}

		m_atom_table.add(p_atom);

		while (c_atom** p_last = last.pop())
			*p_last = p_atom;

		last.push(&p_atom->m_pnext);
	}

	else {
		DebugLog("비할당 명령 감지됨 - 처리 안함");
		return ERR_;    // 오류 반환
	}

	gettok();
	return TO_GO;
}

c_expression* c_engine::parse_system_expression(const char* expr_str, const char* prop_path)
{
	if (!expr_str || !prop_path) {
		error(CUR_ERR_LINE, "Invalid null expression or property path");
		return nullptr;
	}

	c_expression* p_expr = new c_expression(&m_call_stack, &m_atom_table, this);

	const char* trimmed_expr = expr_str;
	while (*trimmed_expr && isspace(*trimmed_expr)) {
		trimmed_expr++;
	}

	// 함수 호출인지 확인 (ResetData()와 같은 형식)
	if (strstr(prop_path, ".Resetdata()") != NULL ||
		strstr(prop_path, ".ResetData()") != NULL ||
		strstr(prop_path, ".resetdata()") != NULL)
	{
		// Resetdata()는 표현식이 없는 함수 호출
		p_expr->m_action = c_action::_const;
		p_expr->m_constant = 1;  // 성공 시 1 반환
		return p_expr;
	}

	// 변수 참조 여부 확인
	bool is_variable = false;
	if (isalpha(trimmed_expr[0]) || trimmed_expr[0] == '_' || trimmed_expr[0] == '@') {
		is_variable = true;

		// 예약어 확인
		if (_strnicmp(trimmed_expr, "true", 4) == 0 &&
			(trimmed_expr[4] == '\0' || isspace(trimmed_expr[4]))) {
			is_variable = false;
		}
		else if (_strnicmp(trimmed_expr, "false", 5) == 0 &&
			(trimmed_expr[5] == '\0' || isspace(trimmed_expr[5]))) {
			is_variable = false;
		}
	}

	if (is_variable) {
		p_expr->m_action = c_action::_variable;
		p_expr->set_var_name(trimmed_expr);
		return p_expr;
	}

	if (trimmed_expr[0] == '"')
	{
		size_t len = strlen(trimmed_expr);

		if(len > 2 && trimmed_expr[len - 1] == '"')
		{
			char* unquoted = new char[len - 1];
			strncpy_s(unquoted, len - 1, trimmed_expr + 1, len - 2);
			unquoted[len - 2] = '\0';

			p_expr->m_action = c_action::_const;
			p_expr->m_constant = unquoted;

			delete[] unquoted;
			return p_expr;
		}
	}

	if (strstr(prop_path, ".Visible") != NULL)
	{
		p_expr->m_action = c_action::_const;

		if (_strnicmp(trimmed_expr, "true", 4) == 0 ||
			(trimmed_expr[0] == '1' && (trimmed_expr[1] == '\0' || isspace(trimmed_expr[1])))) {
			p_expr->m_constant = true;
		}
		else if (_strnicmp(trimmed_expr, "false", 5) == 0 ||
			(trimmed_expr[0] == '0' && (trimmed_expr[1] == '\0' || isspace(trimmed_expr[1])))) {
			p_expr->m_constant = false;
		}
		else {
			char* endptr;
			long value = strtol(trimmed_expr, &endptr, 10);
			if (endptr != trimmed_expr) {
				p_expr->m_constant = (value != 0);
			}
			else {
				error(CUR_ERR_LINE, "Visible 속성에는 'true', 'false', '0', '1' 또는 정수 값만 허용됩니다 (입력값: '%s')", trimmed_expr);
			}
		}
	}
	else if (strstr(prop_path, ".AddString") != NULL)
	{
		if (trimmed_expr[0] == '"')
		{
			size_t len = strlen(trimmed_expr);

			if (len > 2 && trimmed_expr[len - 1] == '"')
			{
				char* unquoted = new char[len - 1];
				strncpy_s(unquoted, len - 1, trimmed_expr + 1, len - 2);
				unquoted[len - 2] = '\0';

				p_expr->m_action = c_action::_const;
				p_expr->m_constant = unquoted;

				delete[] unquoted;
				return p_expr;
			}
		}
	}

	else if (strstr(prop_path, ".SetCurStr") != NULL)
	{
		if (trimmed_expr[0] == '"')
		{
			size_t len = strlen(trimmed_expr);

			if (len > 2 && trimmed_expr[len - 1] == '"')
			{
				char* unquoted = new char[len - 1];
				strncpy_s(unquoted, len - 1, trimmed_expr + 1, len - 2);
				unquoted[len - 2] = '\0';

				p_expr->m_action = c_action::_const;
				p_expr->m_constant = unquoted;

				delete[] unquoted;
				return p_expr;
			}
		}
	}
	else if (strstr(prop_path, ".SetCurSel") != NULL)
	{
		char* endptr;
		long value = strtol(trimmed_expr, &endptr, 10);

		if (endptr != trimmed_expr) {
			p_expr->m_action = c_action::_const;
			p_expr->m_constant = (int)value;
		}
		else {
			error(CUR_ERR_LINE, "SetCurSel 속성에는 숫자 값만 허용됩니다 (입력값: '%s')", trimmed_expr);
		}
	}

	else
	{
		p_expr->m_action = c_action::_const;
		p_expr->m_constant = trimmed_expr;
	}

	return p_expr;
}