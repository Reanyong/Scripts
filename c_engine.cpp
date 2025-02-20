#include "pch.h"

#include "c_engine.h"
#include "c_input_atom.h"
#include "c_dialog_atom.h"

int c_engine::gettok()
{
	prevtok = curtok;

	m_char_stream.gettok(curtok);

	return (int)(token_type)curtok.type;
}

int c_engine::get_routine_arg_count(const char* p_name)
{
	int i;
	for (i = 0; i < m_sub_table.get_size(); i++)
	{
		if (_stricmp(m_sub_table[i].m_name.get_buffer(), p_name) == 0)
			return m_sub_table[i].m_nargs;
	}

	return -1;
}

bool c_engine::is_routine(const char* p_name, bool b_sub)
{
	int i;
	for (i = 0; i < m_sub_table.get_size(); i++)
	{
		if (m_sub_table[i].b_is_sub == b_sub)
			if (_stricmp(m_sub_table[i].m_name.get_buffer(), p_name) == 0)
				return true;
	}

	return false;
}

VARENUM c_engine::get_routine_type(const char* p_name)
{
	int i;
	for (i = 0; i < m_sub_table.get_size(); i++)
	{
		if (!m_sub_table[i].b_is_sub)
			if (!_stricmp(m_sub_table[i].m_name.get_buffer(), p_name))
				return m_sub_table[i].m_type;
	}

	return VT_EMPTY;
}

bool c_engine::is_sub(const char* p_name)
{
	return is_routine(p_name, true);
//	return m_atom_table.get_routine_entry(p_name, true) != 0;
}

bool c_engine::is_function(const char* p_name)
{
	return is_routine(p_name, false);
//	return m_atom_table.get_routine_entry(p_name, false) != 0;
}

bool c_engine::is_ext_const (const char* p_name)
{
	for (int i = 0; i < m_ext_consts.get_size(); i++)
		if (m_ext_consts.contains(p_name))
			return true;

	return false;
}

bool c_engine::is_ext_object (const char* p_name)
{
	return m_ext_objects.find(p_name) != -1;
}

bool c_engine::is_ext_func(const char* p_name)
{
	for (int i = 0; i < m_ext_funcs.get_size(); i++)
		if (_stricmp(m_ext_funcs[i].m_name, p_name) == 0)
			return true;

	return false;
}

bool c_engine::is_ext_sub(const char* p_name)
{
	for (int i = 0; i < m_ext_subs.get_size(); i++)
		if (_stricmp(m_ext_subs[i].m_name, p_name) == 0)
			return true;

	return false;
}

c_dll_func* c_engine::get_dll_routine(const char* p_name, bool b_sub)
{
	for (int i = 0; i < m_dll_funcs.get_size(); i++)
		if (_stricmp(m_dll_funcs[i].get_routine_name(), p_name) == 0)
			if (m_dll_funcs[i].is_sub() == b_sub)
				return m_dll_funcs.get_ptr(i);

	return 0;
}

bool c_engine::is_dll_func(const char* p_name)
{
	return get_dll_routine(p_name, false) != 0;
}

bool c_engine::is_dll_sub(const char* p_name)
{
	return get_dll_routine(p_name, true) != 0;
}

bool c_engine::is_dll_routine(const char* p_name)
{
	return is_dll_sub(p_name) || is_dll_func(p_name);
}

bool c_engine::is_type(const char* p_name)
{
	int i;

	for (i = 0; i < sizeof(types) / sizeof(type_map); i++)
		if (_stricmp(types[i]._name, p_name) == 0)
			return true;

	return false;
}

bool c_engine::is_object_variable(const char* p_name,
								  CLSID* p_clsid,
								  GUID* p_libid)
{
	int i;

	for (i = 0; i <	m_sub_namespace.get_size(); i++)
	{
		c_dim_entry* p_e = m_sub_namespace.get_ptr(i);

		if (_stricmp(p_e->m_name, p_name) == 0)
		{
			if (p_e->m_type == VT_DISPATCH)
			{
				if (p_clsid) *p_clsid = p_e->m_clsid;
				if (p_libid) *p_libid = p_e->m_libid;
				return true;
			}
		}
	}

	return false;
}

void c_engine::capitalize_keyword(char* p)
{
	int n_length = 0;
	c_string str;
	int i = 0;
	for ( i = 0; p[i] && (isalnum(p[i]) || p[i] == '_'); i++)
	{
		str += p[i];
		n_length ++;
	}

	if (!str.get_length()) return;

	for (i = 0; i < sizeof(reserved_w) / sizeof(const char*); i++)
		if (_stricmp(reserved_w[i], str.get_buffer()) == 0)
		{
			strncpy(p, reserved_w[i], n_length);
			return;
		}

	for (i = 0; i < sizeof(types) / sizeof(type_map); i++)
		if (_stricmp(types[i]._name, str.get_buffer()) == 0)
		{
			strncpy(p, types[i]._name, n_length);
			return;
		}

	for (i = 0; i < m_ext_consts.get_size(); i++)
		if (_stricmp(m_ext_consts.names[i], str.get_buffer()) == 0)
		{
			strncpy(p, m_ext_consts.names[i], n_length);
			return;
		}

	for (i = 0; i < m_ext_subs.get_size(); i++)
		if (_stricmp(m_ext_subs[i].m_name.get_buffer(), str.get_buffer()) == 0)
		{
			strncpy(p, m_ext_subs[i].m_name.get_buffer(), n_length);
			return;
		}

	for (i = 0; i < m_ext_funcs.get_size(); i++)
		if (_stricmp(m_ext_funcs[i].m_name.get_buffer(), str.get_buffer()) == 0)
		{
			strncpy(p, m_ext_funcs[i].m_name.get_buffer(), n_length);
			return;
		}
}

bool c_engine::is_keyword(const char* p_str, int n_length)
{
	c_string str;
	int i = 0;
	for (i = 0; i < n_length; i++) str += p_str[i];	

	for (i = 0; i < sizeof(reserved_w) / sizeof(const char*); i++)
		if (_stricmp(reserved_w[i], str.get_buffer()) == 0) return true;

	for (i = 0; i < sizeof(types) / sizeof(type_map); i++)
		if (_stricmp(types[i]._name, str.get_buffer()) == 0) return true;

	for (i = 0; i < m_ext_consts.get_size(); i++)
		if (_stricmp(m_ext_consts.names[i], str.get_buffer()) == 0)
			return true;

	if (n_length >= 11)
		if (_strnicmp(p_str, "independent", 11) == 0) return true;

	return false;
}

bool c_engine::is_routine(const char* p_str, int n_length)
{
	c_string str;
	for (int i = 0; i < n_length; i++) str += p_str[i];	
	if (is_ext_func(str) || is_ext_sub(str)) return true;

	return get_default_parent(str.get_buffer()) != 0;
}

int c_engine::set_breakpoint(int n_line, bool b_from_dbgwnd)
{
	int n = m_atom_table.set_breakpoint(n_line);
	//if (!b_from_dbgwnd && m_pwnd) m_pwnd->SetBreakpoint(n);
	return n;
}

int c_engine::remove_breakpoint(int n_line, bool b_from_dbgwnd)
{
	int n = m_atom_table.remove_breakpoint(n_line);
	//if (!b_from_dbgwnd && m_pwnd) m_pwnd->RemoveBreakpoint(n);
	return n;
}

int c_engine::toggle_breakpoint(int n_line, bool* p_bset, bool b_from_dbgwnd)
{
	int n = m_atom_table.toggle_breakpoint(n_line, p_bset);
	//if (!b_from_dbgwnd && m_pwnd) m_pwnd->ToggleBreakpoint(n);
	return n;
}

int c_engine::get_breakpoint_count()
{
	return m_atom_table.get_breakpoint_count();
}

int c_engine::get_breakpoint(int n_index)
{
	return m_atom_table.get_breakpoint(n_index);
}

void c_engine::remove_all_breakpoints()
{
	m_atom_table.remove_all_breakpoints();
}

bool c_engine::can_be_identifier(const char* p_name, bool b_verbose)
{
	if (!p_name || !strlen(p_name))
	{
		error(CUR_ERR_LINE, "identifier cannot be empty string.", p_name);
		return false;
	}

	if (isdigit(*p_name))
	{
		error(CUR_ERR_LINE, "identifier '%s' is invalid. first char must not be a digit.", p_name);
		return false;
	}

	const char* p = p_name;
	while(*p)
	{
		// 특수문자 추가 jkh
		if (!isalnum(*p) && *p != '_' && *p != '$' && *p != '@' && *p != '-')
		{
 			if (b_verbose)
				error(CUR_ERR_LINE, "identifier '%s' contains invalid characters.", p_name);
			return false;
		}

		p ++;
	}

	if (is_ext_object(p_name))
	{
		if (b_verbose)
			error(CUR_ERR_LINE, "attempt to redeclare object '%s'.", p_name);
		return false;
	}
	if (is_ext_const(p_name))
	{
		if (b_verbose)
			error(CUR_ERR_LINE, "attempt to redeclare constant '%s'.", p_name);
		return false;
	}
	else if (is_ext_sub(p_name))
	{
		if (b_verbose)
			error(CUR_ERR_LINE, "attempt to redeclare sub '%s'.", p_name);
		return false;
	}
	else if (is_ext_func(p_name))
	{
		if (b_verbose)
			error(CUR_ERR_LINE, "attempt to redeclare function '%s'.", p_name);
		return false;
	}
	else if (is_dll_func(p_name))
	{
		if (b_verbose)
			error(CUR_ERR_LINE, "attempt to redeclare DLL function '%s'.", p_name);
		return false;
	}
	else if (is_dll_sub(p_name))
	{
		if (b_verbose)
			error(CUR_ERR_LINE, "attempt to redeclare DLL sub '%s'.", p_name);
		return false;
	}
	else
	{
		LPDISPATCH lpd = get_default_parent(p_name);
		if (lpd)
		{
			error(CUR_ERR_LINE, "attempt to redeclare object member '%s'.", p_name);
			return false;
		}

		for (int i = 0; i < sizeof(reserved_w) / sizeof(const char*); i++)
			if (_stricmp(reserved_w[i], p_name) == 0)
			{
				if (b_verbose)
					error(CUR_ERR_LINE, "attempt to use reserved word '%s' as identifier.", reserved_w[i]);
				return false;
			}
		
		if (is_type(p_name))
		{
			if (b_verbose)
				error(CUR_ERR_LINE, "attempt to use type '%s' as identifier.", p_name);
			return false;
		}
	}

	return true;
}

LPDISPATCH c_engine::get_object(const char* p_name)
{
	int n = m_ext_objects.find(p_name);
	if (n == -1) return 0;
	return m_ext_objects.vars[n].pdispVal;
}

bool c_engine::is_valid_hint(const char* p_name)
{
	if (m_atom_table.is_waiting())
		if (m_call_stack.declared(p_name, 0)) return true;

	int i;
	for (i = 0; i < m_ext_consts.names.get_size(); i++)
		if (!_stricmp(p_name, m_ext_consts.names[i])) return true;

	return false;
}

bool c_engine::is_declared_array(const char* p_name)
{
	int i;
	if (m_atom_table.is_waiting()) // at run-time
	{
		return m_call_stack.declared_array(p_name);
	}
	else
	{
		for (i = 0; i <	m_sub_namespace.get_size(); i++)
			if (_stricmp(m_sub_namespace[i].m_name, p_name) == 0)
				if (m_sub_namespace[i].m_type == VT_VARARRAY) return true;

		for (i = 0; i <	m_global_namespace.get_size(); i++)
			if (_stricmp(m_global_namespace[i].m_name, p_name) == 0)
				if (m_global_namespace[i].m_type == VT_VARARRAY) return true;
	}

	return false;
}

bool c_engine::is_declared_local_var(const char* p_name)
{
	for (int i = 0; i <	m_sub_namespace.get_size(); i++)
		if (_stricmp(m_sub_namespace[i].m_name, p_name) == 0)
			return true;

	return false;
}

VARENUM c_engine::get_expr_type(c_expression* p_expr)
{
	if (p_expr->m_action == c_action::_variable)
	{
		int i = 0;
		for (i = 0; i <	m_sub_namespace.get_size(); i++)
			if (_stricmp(m_sub_namespace[i].m_name, p_expr->get_var_desc()->get_name()) == 0)
				return m_sub_namespace[i].m_type;

		for (i = 0; i <	m_global_namespace.get_size(); i++)
			if (_stricmp(m_global_namespace[i].m_name, p_expr->get_var_desc()->get_name()) == 0)
				return m_global_namespace[i].m_type;
	}
	else if (p_expr->m_action == c_action::_const)
	{
		return (VARENUM)p_expr->m_constant.vt;
	}
	return VT_EMPTY;
}

bool c_engine::get_ext_constant(const char* p_name, c_variable& var)
{
	int n = m_ext_consts.find(p_name);
	if (n == -1) return false;

	var = m_ext_consts.vars[n];
	return true;

}

bool c_engine::get_ext_function(const char* p_name, extension_function& func)
{
	for(int i = 0; i < m_ext_funcs.get_size(); i++)
		if (_stricmp(m_ext_funcs[i].m_name, p_name) == 0)
		{
			func = m_ext_funcs[i];
			return true;
		}

	return false;
}

bool c_engine::get_ext_sub(const char* p_name, extension_sub& sub)
{
	for(int i = 0; i < m_ext_subs.get_size(); i++)
		if (_stricmp(m_ext_subs[i].m_name, p_name) == 0)
		{
			sub = m_ext_subs[i];
			return true;
		}

	return false;
}

bool c_engine::add_object(const char* p_name, LPDISPATCH p_o, bool b_default)
{
	_ASSERT(p_o);

	if (!can_be_identifier(p_name, false))
	{
#ifdef _DEBUG
		MessageBox(0, "Trying to add object with illegal name (cannot be identifier)", "DEBUG", 0);
#endif
		return false;
	}

	// todo: check member names conflicts if b_default

	m_ext_objects.add(p_name, p_o);
	m_ext_obj_flags.add(b_default);

	return true;
}

bool c_engine::is_added_object(IDispatch* pd)
{
	int i;
	for (i = 0; i < m_ext_objects.get_size(); i++)
		if (m_ext_objects.vars[i].pdispVal == pd) return true;

	return false;
}

LPDISPATCH c_engine::get_default_parent(const char* p_member)
{
	int i, j;
	c_string str_name;
	LPDISPATCH lpd;
	HRESULT hr;
	BSTR pb_name;

	ITypeInfo *pti = 0;
	TYPEATTR* pta = 0;
	FUNCDESC* pfd =0;

	for (i = 0; i < m_ext_objects.get_size(); i++)
	{
		if (!m_ext_obj_flags[i]) continue;

		lpd = m_ext_objects.vars[i].pdispVal;

		// get type info from the IDispatch

		hr = lpd->GetTypeInfo(0, 0, &pti);
		if (FAILED(hr) || !pti)
		{
			//_ASSERT(0);
			return 0;
		}

		// get TYPEATTR from ITypeInfo

		pta = 0;
		hr = pti->GetTypeAttr(&pta);
		if (FAILED(hr) || !pta)
		{
			pti->Release();
			continue;
		}

		if (pta->typekind == TKIND_INTERFACE) // Get the dual
		{
			ITypeInfo* pti_temp;
			HREFTYPE hRef;
			hr = pti->GetRefTypeOfImplType(-1, &hRef);
			if (!FAILED(hr))
			{
				hr = pti->GetRefTypeInfo(hRef, &pti_temp);
				_ASSERT(!FAILED(hr));

				pti->ReleaseTypeAttr(pta);
				pti->Release();
				pti = pti_temp;
				pti->GetTypeAttr(&pta);
			}
		}

		// iterate through functions to check name

		_ASSERT(pta->cFuncs);
		for (j = 0; j < pta->cFuncs; j++)
		{
			pfd = 0;
			pti->GetFuncDesc(j, &pfd);

			if (pfd->wFuncFlags & (FUNCFLAG_FRESTRICTED | FUNCFLAG_FHIDDEN))
			{
				pti->ReleaseFuncDesc(pfd);
				continue;
			}

			pti->GetDocumentation(pfd->memid, &pb_name, 0, 0, 0);
			str_name = pb_name;
			SysFreeString(pb_name);

			if (_stricmp(str_name.get_buffer(), p_member) == 0)
			{
				pti->ReleaseFuncDesc(pfd);
				pti->ReleaseTypeAttr(pta);
				pti->Release();
				return lpd;
			}

			pti->ReleaseFuncDesc(pfd);
		}

		pti->ReleaseTypeAttr(pta);
		pti->Release();
	}

	return 0;
}

void c_engine::on_terminal_enter()
{
	if (!m_atom_table.m_pcur_atom) return;
	if (m_atom_table.m_pcur_atom->m_type != atom_type::input_atom) return;
	((c_input_atom*)m_atom_table.m_pcur_atom)->on_terminal_enter();
}

void c_engine::on_window_enter()
{
	if (!m_atom_table.m_pcur_atom) return;
	if (m_atom_table.m_pcur_atom->m_type != atom_type::dialog_atom) return;
	((c_dialog_atom*)m_atom_table.m_pcur_atom)->on_window_enter();
}

VARENUM c_engine::get_type(const char* pstr)
{
	int i;
	for (i = 0; i < sizeof(types) / sizeof(type_map); i++)
		if (_stricmp(pstr, types[i]._name) == 0) return types[i]._type;

	return (VARENUM)-1;
}

bool c_engine::eval_watch(const char* p_str, c_variable* p_val)
{
//	if (m_brun) return true;

	bool b;
	bool b_retval = true;
	c_variable v;
//	_ASSERT(m_char_stream.pos() == 0);

	if (!p_str) return false;
	if (!strlen(p_str)) return false;

// set temp buffer
	m_char_stream.set_temp_buffer(p_str);

// parse expression
	prevtok.reset();
	curtok.reset();
	gettok();
	c_expression* p_expr = _expr();
	if (!p_expr)
	{
		*p_val = "*syntax error*";
		m_str_error.empty();
		b_retval = false;
		goto _end;
	}

// check if this expression valid

	if (p_expr->calls_user_routines())
	{
		*p_val = "routine calls not allowed";
		b_retval = false;
		goto _end;
	}

// eval expression

	b = m_bruntime;
	m_bruntime = false;
	p_expr->exec(p_val);
	m_bruntime = b;

_end:
	delete p_expr;
	m_char_stream.free_temp_buffer();
	return b_retval;
}

void _sort_watch(c_str_array* p_members,
				 c_array<c_variable>* p_values)
{
	int i;
	bool b_swap;
	do
	{
		b_swap = false;

		for (i = 0; i < p_members->get_size() - 1; i++)
		{
			if (_stricmp(p_members->get(i), p_members->get(i + 1)) > 0)
			{
				p_members->swap(i, i + 1);
				p_values->swap(i, i + 1);

				b_swap = true;
			}
		}
	}
	while (b_swap);
}

void c_engine::watch_obj_members(LPDISPATCH lpd,
								 c_str_array* p_members,
								 c_array<c_variable>* p_values)
{
	if (!lpd) return;

	int i;
	c_string str_name;
	HRESULT hr;

	// get type info from the IDispatch

	ITypeInfo *pti;
	hr = lpd->GetTypeInfo(0, 0, &pti);
	_ASSERT(!FAILED(hr));
	_ASSERT(pti);

	// get TYPEATTR from ITypeInfo

	TYPEATTR* pta;
	hr = pti->GetTypeAttr(&pta);
	if (FAILED(hr) || !pta)
	{
		pti->Release();
		return;
	}

	if (pta->typekind == TKIND_INTERFACE) // Get the dual
	{
		ITypeInfo* pti_temp;
		HREFTYPE hRef;
		hr = pti->GetRefTypeOfImplType(-1, &hRef);
		if (!FAILED(hr))
		{
			hr = pti->GetRefTypeInfo(hRef, &pti_temp);
			_ASSERT(!FAILED(hr));

			pti->ReleaseTypeAttr(pta);
			pti = pti_temp;
			pti->GetTypeAttr(&pta);
		}
	}

	// get all members

	_ASSERT(pta->cFuncs);
	for (i = 0; i < pta->cFuncs; i++)
	{
		FUNCDESC* pfd;
		pti->GetFuncDesc(i, &pfd);

		if (pfd->wFuncFlags & (FUNCFLAG_FRESTRICTED | FUNCFLAG_FHIDDEN) ||
			(pfd->invkind != DISPATCH_PROPERTYGET) ||
			(pfd->cParams > 0))
		{
			pti->ReleaseFuncDesc(pfd);
			continue;
		}

		switch (pfd->elemdescFunc.tdesc.vt)
		{
		case VT_USERDEFINED:
		case VT_EMPTY:
		case VT_NULL:
		case VT_I2:
		case VT_I4:
		case VT_R4:
		case VT_R8:
		case VT_CY:
		case VT_DATE:
		case VT_BSTR:
		case VT_ERROR:
		case VT_BOOL:
		case VT_VARIANT:
		case VT_DECIMAL:
		case VT_I1:
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
		case VT_VOID:
		case VT_HRESULT:
		case VT_DISPATCH:
		case VT_PTR:
			{
//				pti->GetDocumentation(pfd->memid, &pb_name, 0, 0, 0);
//				str_name = pb_name;
//				SysFreeString(pb_name);

//

				unsigned int n_error;
				DISPPARAMS disp_params;
				EXCEPINFO exc;
				c_variable result;
				memset(&disp_params, 0, sizeof(disp_params));

				PUSH_RESERVED(&result);

				hr = lpd->Invoke(
					pfd->memid,
					IID_NULL,
					LOCALE_USER_DEFAULT,
					DISPATCH_PROPERTYGET | DISPATCH_METHOD,
					&disp_params,
					(VARIANT*)&result,
					&exc,
					&n_error);

				POP_RESERVED(&result);

				// save results

				if (!FAILED(hr))
				{
					p_members->add(str_name.get_buffer());
					p_values->add(result);
				}
			}
		}

		pti->ReleaseFuncDesc(pfd);
	}

	pti->ReleaseTypeAttr(pta);
	_ASSERT(p_values->get_size() == p_members->get_size());
	_sort_watch(p_members, p_values);
}

bool c_engine::set_bp_condition(int n_line, const char* p_expr, bool b_from_dbgwnd)
{
	if (!p_expr) return false;
	if (!strlen(p_expr)) return false;

// parse expression

	m_char_stream.set_temp_buffer(p_expr);
	gettok();
	c_expression* p_expression = _expr();
	m_char_stream.free_temp_buffer();

	if (!p_expression) return false;

	if (p_expression->calls_user_routines())
	{
		delete p_expression;
		return false;
	}

// pass it to the atom

	//if (!b_from_dbgwnd && m_pwnd) m_pwnd->SetBPCondition(n_line, p_expr);

	return m_atom_table.set_bp_condition(n_line, p_expression);
}

void c_engine::get_line_context(int n, int* p_nline, c_string* p_routine)
{
	m_char_stream.get_line_context(n, p_nline, p_routine);
}

void c_engine::get_error_context(int* p_nline, c_string* p_routine)
{
	_ASSERT(!p_routine->get_length());

	if (!m_str_error.get_length()) return;
	if (_strnicmp(m_str_error.get_buffer(), "line: ", 6)) return;

	const char* p_number = m_str_error.get_buffer() + 6;
	int n = atoi(p_number);

	m_char_stream.get_line_context(n, p_nline, p_routine);
}

void c_engine::get_breakpoint_context(int n,				// [in] breakpoint number
									  int* p_nline,			// [out] breakoint line in routine
									  c_string* p_routine)	// [out] routine name
{
	_ASSERT(!p_routine->get_length());
	int n_line = get_breakpoint(n);

	if (n_line != -1)
		m_char_stream.get_line_context(n, p_nline, p_routine);
}

void c_engine::format_bp_list(c_str_array* p_list)
{
	p_list->empty();
	//if (m_pwnd) m_pwnd->FormatBPList(p_list);
}

void c_engine::set_bp_list(c_str_array* p_list)
{
	c_str_array items;
	int n_routine_line;
	int n_actual_line;
	c_string routine;
	c_string condition;

	int i;
	for (i = 0; i < p_list->get_size(); i++)
	{
		items.split(p_list->get(i), ';', true);
		_ASSERT(items.get_size() >= 2);

		n_routine_line = atoi(items[0]);
		routine = items[1];
		if (items.get_size() == 3)
			condition = items[2];
		else
			condition.empty();

		m_char_stream.get_line_context(n_routine_line, routine.get_buffer(), &n_actual_line);
		if (n_actual_line != -1)
		{
			set_breakpoint(n_actual_line, false);
			if (condition.get_length())
				set_bp_condition(n_actual_line, condition.get_buffer(), false);
		}
	}
}

run_mode c_engine::get_run_mode()
{
	return m_atom_table.get_run_mode();
}

void get_types_and_pointers(const char* p_name,
							SAFEARRAY* parray,
							c_array<VARENUM>* p_types,
							c_ptr_array<c_variable>* p_vars)
{
	ASSERT(!p_types->get_size());
	ASSERT(!p_vars->get_size());
	if (!parray) return;

// prepare type and pointer arrays --------------------------------------------

	int n = parray->rgsabound[0].cElements;

	p_types->alloc(n);
	p_vars->alloc(n);

	HRESULT hr = SafeArrayLock(parray);
	ASSERT(!FAILED(hr));

	c_variable* p_element = 0;
	long i;
	for (i = 0; i < n; i++)
	{
		hr = SafeArrayPtrOfIndex(parray, &i, (void**)&p_element);
		ASSERT(!FAILED(hr));
		ASSERT(p_element);

		p_vars->set_at(i, p_element);
		(*p_types)[i] = (VARENUM)p_element->vt;
	}

	hr = SafeArrayUnlock(parray);
	ASSERT(!FAILED(hr));
}

bool call_disp_routine(IDispatch* lpd,
					   MEMBERID memid,
					   SAFEARRAY* parray,
					   c_engine* p_engine,
					   c_variable &result)
{
	int i;
	HRESULT hr;
	unsigned int n_error;
	DISPPARAMS disp_params;
	EXCEPINFO exc;

	int n = 0;
	if (parray) n = parray->rgsabound[0].cElements;
	c_array<c_variable> args;
	args.alloc(n);
	for (i = 0; i < n; i++)
	{
		args[n - i - 1] = ((c_variable*)parray->pvData)[i];
	}

	// attach args to 'disp_params'

	disp_params.cArgs = args.get_size();
	disp_params.rgvarg = args.get_data();
	disp_params.cNamedArgs = 0;
	disp_params.rgdispidNamedArgs = 0;

	// invoke

	hr = lpd->Invoke(
		memid,
		IID_NULL,
		LOCALE_USER_DEFAULT,
		DISPATCH_METHOD,
		&disp_params,
		&result,
		&exc,
		&n_error);

	if (FAILED(hr))
	{
		p_engine->disp_error(NO_ERR_LINE, hr, &exc, "Error invoking method: ");
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// persistent DBG watch -------------------------------------------------------
//-----------------------------------------------------------------------------

const char* c_engine::dbg_watch_get(int n)
{
	if (n < 0 || n >= m_dbg_watches.get_size()) return 0;
	return m_dbg_watches[n];
}

bool c_engine::dbg_watch_set(int n, const char* p)
{
	if (n < 0 || n > m_dbg_watches.get_size()) return false;

	if (n == m_dbg_watches.get_size()) m_dbg_watches.add(p);
	else m_dbg_watches.set_at(n, p);

	return true;
}

bool c_engine::dbg_watch_remove(int n)
{
	if (n < 0 || n > m_dbg_watches.get_size()) return false;

	m_dbg_watches.remove_at(n);

	return true;
}

int c_engine::dbg_watch_get_count()
{
	return m_dbg_watches.get_size();
}

void c_engine::dbg_watch_clear()
{
	m_dbg_watches.empty();
}

//-----------------------------------------------------------------------------
// terminal line count --------------------------------------------------------
//-----------------------------------------------------------------------------

void c_engine::set_terminal_lines(int n)
{
	if (n < 2 || n > 10000) return;
	m_nterminal_lines = n;
	if (m_pTerminal) m_pTerminal->on_line_count_changed();
}

int c_engine::get_terminal_lines()
{
	return m_nterminal_lines;
}

//-----------------------------------------------------------------------------
// assert_valid ---------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef _DEBUG
void c_engine::assert_valid()
{
	m_call_stack.assert_valid();
}
#endif

