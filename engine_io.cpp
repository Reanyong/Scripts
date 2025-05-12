#include "pch.h"

#include <crtdbg.h>
#include "c_engine.h"

// c_engine initialization ----------------------------------------------------

#pragma warning(disable:4355)
c_engine::c_engine()
:
m_atom_table (&m_call_stack, this),
m_call_stack(this),
m_com_hint(this)
{
	m_pTerminal			= 0;
	m_pInputDialog		= 0;
	m_nentry_pos		= 0;
	m_hfont				= (HFONT)GetStockObject(ANSI_FIXED_FONT);
	m_pnotify_ptr		= 0;
	m_pfn_step			= 0;
	m_pfn_end			= 0;
	m_pfn_close			= 0;
	m_pfn_about			= 0;
	m_nterminal_lines	= MAX_TERMINAL_STRINGS;
	m_ballow_debugging	= true;

	m_bContinueOnError = true;  // 기본값: 오류 발생해도 계속 진행

	internal_reset();

// extensions -----------------------------------------------------------------

	add_standard_extension_constants();
	add_standard_extension_functions();
	add_standard_extension_subs();
	add_standard_hints();

// handles --------------------------------------------------------------------

//	create_events();
	//create_terminal(h_inst);
	//create_dialog(h_inst);
}

c_engine::~c_engine(void)
{
	internal_reset();

//	delete m_pTerminal;
//	delete m_pInputDialog;
//	if (m_pwnd)
//		delete m_pwnd;


	//SAFE_DELETE(m_pTerminal);
	//SAFE_DELETE(m_pInputDialog);
	//SAFE_DELETE(m_pwnd);

	//20220715 YDK 수정

}

c_terminal* c_engine::get_terminal()
{
	_ASSERT(m_pTerminal);
	return m_pTerminal;
}

HWND c_engine::get_terminal_HWND()
{
	if (m_pTerminal == 0) return 0;
	return m_pTerminal->m_hwnd;
}

c_input_dialog* c_engine::get_input_dialog()
{
	return m_pInputDialog;
}

void c_engine::dbg_print(const char* p_str)
{
	dbg_wnd_visible(true);
	//m_pwnd->DbgPrint(p_str);
}

void c_engine::create_terminal(HINSTANCE h_inst)
{
	m_pTerminal = new c_terminal(this);
	m_pTerminal->create(h_inst);
}

void c_engine::create_dialog(HINSTANCE h_inst)
{
	m_pInputDialog = new c_input_dialog(this);
	m_pInputDialog->create(h_inst);
	m_pInputDialog->create_controls(h_inst);
}


void c_engine::create_dbg_wnd()
{
	//_ASSERT(!m_pwnd);
	//m_pwnd = new CBasicEditWnd;
	//
 //
	////20210405
	//BOOL bRet = m_pwnd->LoadFrame(IDR_MENU1);
	//if (0 != bRet)
	//{
	//	m_pwnd->InitialUpdateFrame(NULL, FALSE);
	//	m_pwnd->set_engine(this);
	//	CString s = m_char_stream.get_buffer();
	//	m_pwnd->SetBuffer(s);
	//}
}

void c_engine::on_end_execution()
{
	internal_reset();
	if (m_pfn_end) m_pfn_end(m_pnotify_ptr);
	//if (m_pwnd) m_pwnd->PostStepUpdate();
}

void c_engine::internal_reset()
{
	prevtok.reset();
	curtok.reset();

	m_char_stream.reset();
	m_atom_table.reset();
	m_call_stack.reset();
	m_sub_table.reset();
	m_pcur_routine_entry= 0;
	m_bruntime			= true;
	m_bstopped			= false;
	m_brun				= false;
}

void c_engine::reset()
{
#ifndef _DEBUG
	try
	{
#endif
		m_ext_objects.reset();
		m_ext_obj_flags.reset();

		m_atom_table.free_all();
		internal_reset();

		//if (m_pwnd) delete m_pwnd;
		//m_pwnd = 0;

		/*if (m_pTerminal)
		{
			m_pTerminal->clear();
			m_pTerminal->hide();
		}
		if (m_pInputDialog)
		{
			m_pInputDialog->reset_contents();
			m_pInputDialog->hide();
		}*/

		m_bstopped = false;
#ifndef _DEBUG
	}
	catch(...)
	{
		AfxMessageBox("exception in c_engine::reset()");
	}
#endif

	m_str_error.empty();
}

const char* c_engine::get_VB_error(int n_code)
{
	switch (n_code)
	{
	case  5: return "Invalid procedure call or argument";
	case  6: return "Overflow";
	case  7: return "Out of memory";
	case  9: return "Subscript out of range";
	case 10: return "Array fixed or temporarily locked";
	case 11: return "Division by zero";
	case 13: return "Type mismatch";
	case 14: return "Out of string space";
	case 17: return "Can't perform requested operation";
	case 28: return "Out of stack space";
	case 35: return "Sub or Function not defined";
	case 48: return "Error in loading DLL";
	case 51: return "Internal error";
	case 52: return "Bad file name or number";
	case 53: return "File not found";
	case 54: return "Bad file mode";
	case 55: return "File already open";
	case 57: return "Device I/O error";
	case 58: return "File already exists";
	case 61: return "Disk full";
	case 62: return "Input past end of file";
	case 67: return "Too many files";
	case 68: return "Device unavailable";
	case 70: return "Permission denied";
	case 71: return "Disk not ready";
	case 74: return "Can't rename with different drive";
	case 75: return "Path/File access error";
	case 76: return "Path not found";
	case 91: return "Object variable not set";
	case 92: return "For loop not initialized";
	case 94: return "Invalid use of Null";
	case 322: return "Can't create necessary temporary file";
	case 424: return "Object required";
	case 429: return "ActiveX component can't create object";
	case 430: return "Class doesn't support Automation";
	case 432: return "File name or class name not found during Automation operation";
	case 438: return "Object doesn't support this property or method";
	case 440: return "Automation error";
	case 445: return "Object doesn't support this action";
	case 446: return "Object doesn't support named arguments";
	case 447: return "Object doesn't support current locale setting";
	case 448: return "Named argument not found";
	case 449: return "Argument not optional";
	case 450: return "Wrong number of arguments or invalid property assignment";
	case 451: return "Object not a collection";
	case 453: return "Specified DLL function not found";
	case 455: return "Code resource lock error";
	case 457: return "This key already associated with an element of this collection";
	case 458: return "Variable uses an Automation type not supported in VBScript";
	case 500: return "Variable is undefined";
	case 501: return "Illegal assignment";
	case 502: return "Object not safe for scripting";
	case 503: return "Object not safe for initializing";
	case 32811: return "Element not found";
	}

	return "";
}

void c_engine::disp_error(int n_line,
						  HRESULT hr,
						  EXCEPINFO* p_exc,
						  const char *p_descr)
{
	c_string str;
	if (p_descr) str = p_descr;

    if(hr == DISP_E_EXCEPTION)
    {
        c_string errDesc;
        c_string errMsg;

		if (p_exc && p_exc->bstrDescription) errDesc = p_exc->bstrDescription;
		else
		{
//			IErrorInfo* pei = 0;
//			GetErrorInfo(0, &pei);
//
//			if (pei)
//			{
//			}
//			else
			{
				errDesc = get_VB_error(p_exc->scode & 0x0000FFFF);

				if (!errDesc.get_length())
					errDesc = "<no error description>";
			}
		}

		errMsg.format("Run-time error %d. %s",
					p_exc->scode & 0x0000FFFF,	//Lower 16-bits of SCODE
					errDesc.get_buffer());

		str += errMsg;
    }
    else
    {
		runtime_error(hr);
		return;
/*
        LPVOID lpMsgBuf;
        ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr,
                        MAKELANGID(LANG_NEUTRAL,
                        SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,
                        0, NULL);
		str += (LPCTSTR)lpMsgBuf;
        ::LocalFree( lpMsgBuf );
*/
    }

	runtime_error(n_line, str.get_buffer());
}

void c_engine::dbg_break()
{
	m_atom_table.m_run_mode = run_mode::run_step_into;
	if (m_ballow_debugging) dbg_wnd_visible(true);
}

void c_engine::runtime_error(int n_line, const char *format, ...)
{
	if (!m_bruntime) return;

// just normal error

	char err[MAX_ERR_LENGTH + 1];

	va_list arglist;
	va_start(arglist, format);
	vsprintf(err, format, arglist);
	va_end(arglist);

	//AddError(n_line, err);
	error(n_line, err);

// and then break execution

	if (m_ballow_debugging)
	{
		dbg_break();
		//m_pwnd->ShowErrorPane();
	}
}

void c_engine::runtime_error(HRESULT hr)
{
	if (!FAILED(hr)) return;

	IErrorInfo* pei = 0;
	HRESULT hr2 = GetErrorInfo(0, &pei);

	if (!FAILED(hr2) && pei)
	{
		BSTR pDescr = 0;
		hr2 = pei->GetDescription(&pDescr);

		if (!FAILED(hr2) && pDescr)
		{
			c_string s = pDescr;
			SysFreeString(pDescr);
			runtime_error(CUR_ERR_LINE, s.get_buffer());
		}

		pei->Release();
	}
	else
	{
		LPVOID lpMsgBuf;
		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
						FORMAT_MESSAGE_FROM_SYSTEM |
						FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr,
						MAKELANGID(LANG_NEUTRAL,
						SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,
						0, NULL);

		runtime_error(CUR_ERR_LINE, (const char*)lpMsgBuf);
		::LocalFree(lpMsgBuf);
	}
}

/*
void c_engine::error(int n_line, const char *format, ...)
{
	char err_str[MAX_ERR_LENGTH + 1];
	char err[MAX_ERR_LENGTH + 1];

	va_list arglist;
	va_start(arglist, format);
	vsprintf(err, format, arglist);
	va_end(arglist);

	int n_actual_error_line;
	switch(n_line)
	{
	case CUR_ERR_LINE:
		n_actual_error_line = m_atom_table.get_cur_line();
		if (n_actual_error_line == -1)
			n_actual_error_line = m_char_stream.cur_line();
		break;

	case NO_ERR_LINE: n_actual_error_line = -1;
		break;

	default: n_actual_error_line = n_line;
	}

	if (n_actual_error_line != -1)
		_snprintf(err_str, MAX_ERR_LENGTH, "line: %d error: %s", n_actual_error_line, err);
	else
		_snprintf(err_str, MAX_ERR_LENGTH, "error: %s", err);

	if (m_str_error.get_length()) m_str_error += "\r\n";
	m_str_error += err_str;
}
*/

void c_engine::error(int nLine, const char* format, ...)
{
	char err[MAX_ERR_LENGTH + 1];
	va_list arglist;
	va_start(arglist, format);
	vsprintf_s(err, MAX_ERR_LENGTH, format, arglist);
	va_end(arglist);

	// 실제 에러 라인 계산
	int n_actual_error_line;
	switch (nLine)
	{
	case CUR_ERR_LINE:
		n_actual_error_line = m_atom_table.get_cur_line();
		if (n_actual_error_line == -1)
			n_actual_error_line = m_char_stream.cur_line();
		break;

	case NO_ERR_LINE:
		n_actual_error_line = -1;
		break;

	default:
		n_actual_error_line = nLine;
	}

	// 현재 루틴 이름 가져오기
	const char* routineName = NULL;
	if (m_pcur_routine_entry) {
		routineName = m_pcur_routine_entry->get_var_desc()->get_name();
	}

	// 오류 정보 생성 전 로그
	DebugLog("오류 생성 전: 라인=%d, 메시지=%s, 루틴=%s",
		n_actual_error_line, err, routineName ? routineName : "NULL");

	// 오류 정보 추가
	ErrorInfo info(n_actual_error_line, err, routineName, false);

	// ErrorInfo 생성 후 로그
	DebugLog("ErrorInfo 생성 후: 라인=%d, 메시지=%s, 루틴=%s",
		info.nLine,
		info.message.get_buffer() ? info.message.get_buffer() : "NULL",
		info.routineName.get_buffer() ? info.routineName.get_buffer() : "NULL");

	// m_errors 배열에 추가하기 전
	DebugLog("오류 추가 전 m_errors 크기: %d", m_errors.get_size());

	m_errors.add(info);

	// m_errors 배열에 추가한 후
	DebugLog("오류 추가 후 m_errors 크기: %d", m_errors.get_size());

	if (m_errors.get_size() > 0) {
		ErrorInfo& lastError = m_errors[m_errors.get_size() - 1];
		DebugLog("마지막 오류 확인: 라인=%d, 메시지=%s",
			lastError.nLine,
			lastError.message.get_buffer() ? lastError.message.get_buffer() : "NULL");
	}

	// 기존 호환성을 위해 첫 번째 오류 메시지만 m_str_error에 저장
	if (m_str_error.get_length() == 0) {
		char err_str[MAX_ERR_LENGTH + 1];
		if (n_actual_error_line != -1)
			sprintf_s(err_str, "line: %d error: %s", n_actual_error_line, err);
		else
			sprintf_s(err_str, "error: %s", err);

		m_str_error = err_str;
	}

	// 중요: m_bContinueOnError가 true면 예외를 던지지 않음
	if (!m_bContinueOnError) {
		//throw std::runtime_error(err);
	}
}

/*
void c_engine::AddError(int nLine, const char* format, ...)
{
	char buffer[MAX_ERR_LENGTH + 1];
	va_list args;
	va_start(args, format);
	vsprintf_s(buffer, MAX_ERR_LENGTH, format, args);
	va_end(args);

	// 실제 에러 라인 계산
	int n_actual_error_line;
	switch (nLine)
	{
	case CUR_ERR_LINE:
		n_actual_error_line = m_atom_table.get_cur_line();
		if (n_actual_error_line == -1)
			n_actual_error_line = m_char_stream.cur_line();
		break;
	case NO_ERR_LINE:
		n_actual_error_line = -1;
		break;
	default:
		n_actual_error_line = nLine;
	}

	// 현재 루틴 이름 가져오기
	const char* routineName = NULL;
	if (m_pcur_routine_entry) {
		routineName = m_pcur_routine_entry->get_var_desc()->get_name();
	}

	// 오류 정보 추가
	ErrorInfo info(n_actual_error_line, buffer, routineName, false);
	m_errors.add(info);

	// 기존 호환성을 위해 첫 번째 오류는 m_str_error에도 저장
	if (m_str_error.get_length() == 0) {
		char err_str[MAX_ERR_LENGTH + 1];
		if (n_actual_error_line != -1)
			_snprintf(err_str, MAX_ERR_LENGTH, "line: %d error: %s", n_actual_error_line, buffer);
		else
			_snprintf(err_str, MAX_ERR_LENGTH, "error: %s", buffer);

		m_str_error = err_str;
	}
}
*/

void c_engine::stop()
{
	m_bstopped					= true;
	m_brun						= false;
	/*if (m_pInputDialog!=NULL)
	{
		m_pInputDialog->m_bcontinue = false;
	}
	if (m_pTerminal!=NULL)
	{
		m_pTerminal->m_bcontinue = false;
	}	*/
	m_atom_table.stop();
	//on_end_execution();
}

bool c_engine::is_waiting()
{
	return m_atom_table.is_waiting();
}

bool c_engine::set_buffer(const char* pData)
{
	//if (m_pwnd) m_pwnd->ClearError();
	internal_reset();
	m_atom_table.free_all();
	m_dll_funcs.reset();

	m_char_stream.set_buffer(pData);
	m_str_error.empty();
	if (!parse())
	{
		internal_reset();
		m_atom_table.free_all();
		return false;
	}

	return true;
}

bool c_engine::check_syntax(const char* p_code)
{
	ClearAllErrors();

	internal_reset();
	m_char_stream.set_buffer(p_code);
	m_str_error.empty();

	bool b = parse();

	internal_reset();

	return b;
}

const char* c_engine::get_internal_buffer()
{
	return m_char_stream.get_buffer();
}

const char* c_engine::get_buffer()
{
	//if (!m_pwnd)
	//	return m_char_stream.get_buffer();
	//else
	{
		//const char* p_edit_buffer = m_pwnd->GetBuffer();
		//if (p_edit_buffer && strlen(p_edit_buffer)) return p_edit_buffer;
		return m_char_stream.get_buffer();
	}
}

void c_engine::set_edit_buffer(const char* pData)
{
	//if (!m_pwnd) return;
	//m_pwnd->SetBuffer(pData);
	//m_pwnd->RedrawWindow();
}

bool c_engine::dbg_wnd_visible()
{
	//if (!m_pwnd) return false;
	return false; //m_pwnd->IsWindowVisible() != 0;
}

void c_engine::dbg_wnd_visible(bool b)
{
	//if (b)
	//{
	//	//if (!m_pwnd) create_dbg_wnd();
	//	_ASSERT(m_pwnd);

	//	m_pwnd->ShowWindow(SW_SHOW);
	//	//m_pwnd->PostStepUpdate();
	//	//m_pwnd->UpdateBreakpoints();
	//	m_pwnd->RunModalLoop();
	//}
	//else
	//	if (m_pwnd) m_pwnd->ShowWindow(SW_HIDE);
}

//-----------------------------------------------------------------------------

int c_engine::get_local_var_count()
{
	return m_call_stack.get_local_size();
}

bool c_engine::get_local_var(int n,
							 c_variable* p_var,
							 const char** pp_name)
{
	return m_call_stack.get_local_var(	n,
										p_var,
										pp_name);
}

int c_engine::get_global_var_count()
{
	return m_call_stack.get_global_size();
}

bool c_engine::get_global_var(int n,
							  c_variable* p_var,
							  const char** pp_name)
{
	return m_call_stack.get_global_var(	n,
										p_var,
										pp_name);
}

void c_engine::get_var_watch(const char* p_name, c_string& str)
{
	_ASSERT(p_name);
	if (!m_call_stack.declared(p_name, 0)) return;

	c_variable* p_var;
	c_var_desc vd; vd = p_name;

	m_call_stack.get(&vd, &p_var);
	p_var->as_string(str);
}

bool c_engine::run_routine(const char* p_name,
						   c_variable* p_args,
						   int n_args,
						   bool b_stop,
						   VARIANT* pRes)
{
	VariantClear(pRes);
	m_str_error.empty();

	if (is_waiting())
	{
		error(NO_ERR_LINE, "cannot run_routine while in waiting state.", p_name);
		return false;
	}

	bool b_found = false;
	c_routine_entry_atom* p_routine = 0;
	int i;
	int n_atoms = m_atom_table.get_count();

	for (i = 0; i < n_atoms; i++)
	{
		c_atom* p_atom = m_atom_table.get_atom(i);

		if (p_atom->m_type == atom_type::routine_entry_atom)
		{
			p_routine = (c_routine_entry_atom*)p_atom;
			if (_stricmp(p_routine->get_var_desc()->get_name(), p_name) == 0)
			{
				b_found = true;
				break;
			}
		}
	}

	if (!b_found)
	{
		error(NO_ERR_LINE, "there is no routine '%s' in the source.", p_name);
		return false;
	}

	if (p_routine->get_arg_count() != n_args)
	{
		error(NO_ERR_LINE, "routine '%s' takes %d arguments. you passed %d.", p_name, p_routine->get_arg_count(), n_args);
		return false;
	}

	// atom table will start from this routine
	p_routine->pass_args(p_args);
	m_atom_table.m_pentry_atom = p_routine;

#ifdef _DEBUG
	assert_valid();
#endif

	if (b_stop)
	{
		dbg_wnd_visible(true);
		step_into();
	}
	else
	{
		if (run_go())
		{
			VariantCopy(pRes, m_call_stack.get_ret_val());
		}
	}

	return true;
}

bool c_engine::run_routine(const char* p_name,
						   const VARIANT* p_safearray,
						   bool b_stop,
						   VARIANT* pRes)
{
//	char buffer[256];
	m_str_error.empty();

	if (is_waiting())
	{
		error(NO_ERR_LINE, "attempt to call routine while engine is running.");
		return false;
	}

	if ( !(p_safearray->vt & VT_ARRAY) )
	{
		error(NO_ERR_LINE, "attempt to call routine with non-array arguments.");
		return false;
	}

	if (!p_safearray->parray || !p_safearray->parray->pvData)
	{
		error(NO_ERR_LINE, "attempt to call routine with null array of arguments.");
		return false;
	}

	if (p_safearray->parray->cDims != 1)
	{
		error(NO_ERR_LINE, "array of arguments not 1-dimensioned.");
		return false;
	}

//	if (!(args.parray->fFeatures &	0x0800))
//	{
//		error(NO_ERR_LINE, "only array of variants supported.");
//		return false;
//	}

	c_array<c_variable> a;
	a.alloc(p_safearray->parray->rgsabound[0].cElements);

//	sprintf(buffer, "cElements: %d, lBound: %d",
//		p_safearray->parray->rgsabound[0].cElements,
//		p_safearray->parray->rgsabound[0].lLbound);
//	MessageBox(0, buffer, 0, 0);

	unsigned int i;
	for (i = 0; i < p_safearray->parray->rgsabound[0].cElements; i++)
	{
		VARIANT* p_v = (VARIANT*)p_safearray->parray->pvData + sizeof(VARIANT) * i;
		switch (p_v->vt)
		{
		case VT_I2:
			a[i] = (int)p_v->iVal;
			break;

		case VT_INT:
		case VT_I4:
		case VT_UINT:
			a[i] = p_v->intVal;
			break;

		case VT_BOOL:
			a[i] = p_v->boolVal;
			break;

		case VT_R8:
			a[i] = p_v->dblVal;
			break;

		case VT_BSTR:
			a[i] = p_v->bstrVal;
			break;

		default:
			error(NO_ERR_LINE, "element %d of array is of type 0x%8.8X. please only 'string', 'integer' or 'double' values in array.", i, p_v->vt);
			return false;
		}
	}

	m_str_error.empty();
	return run_routine(
		p_name,
		a.get_data(),
		a.get_size(),
		b_stop,
		pRes);
}

bool c_engine::run_routine(const char* p_string, bool b_dbg, VARIANT* pRes)
{
	_ASSERT(p_string);

	int i = 0;
	c_string name;
	c_array<c_variable> args;

	const char* p = p_string;
	while (*p && *p != '(' && !isspace(*p))
	{
		name += *p;
		p++;
	}

	while (*p && isspace(*p)) p++;

	if (*p) if (*p != '(')
	{
		error(NO_ERR_LINE, "'(' expected after routine name '%s'", name.get_buffer());
		return false;
	}

	if (!*p) goto _done; // end of line. no arguments.

	p++;	// ommit '('

	while (*p && isspace(*p)) p++;
	if (*p == ')') goto _done; // no arguments.

	while (*p)
	{
		while (*p && isspace(*p)) p++;

		if (*p == '"')	// string
		{
			p++; // ommit '"'
			c_string str;
			while (*p && *p != '"')
			{
				str += *p;
				p++;
			}

			if (*p != '"')
			{
				error(NO_ERR_LINE, "missing closing '\"' for  argument %d of routine '%s'", i, name.get_buffer());
				return false;
			}

			p++; // ommit '"'

			c_variable* v = args.add_new();
			(*v) = str.get_buffer();
		}
		else			// number
		{
			char* p_end;
			double d = strtod(p, &p_end);
			if (p_end && (p_end == p || isalnum(*p_end)) )
			{
				error(NO_ERR_LINE, "bad argument %d of routine '%s'", i, name.get_buffer());
				return false;
			}

			c_variable* v = args.add_new();
			(*v) = d;
			p = p_end;
		}

		while (*p && isspace(*p)) p++;

		if (*p == ')') break;
		else if (*p == ',')
		{
			p++;
			continue;
		}
		else
		{
			error(NO_ERR_LINE, "illegal characters after argument %d of routine '%s'", i, name.get_buffer());
			return false;
		}

		i++;
	}

_done:
	return run_routine(name.get_buffer(),
						args.get_data(),
						args.get_size(),
						b_dbg,
						pRes);
}

//-----------------------------------------------------------------------------
// get_error_descr ------------------------------------------------------------
//-----------------------------------------------------------------------------

const char* c_engine::get_error_descr()
{
	const char* p = get_error();
	if (!p) return 0;
	if (!strlen(p)) return 0;

	if (!_strnicmp(p, "line:", 5))
	{
		p += 5;
		while (p && isspace(*p)) p++;
		while (p && isdigit(*p)) p++;
		while (p && isspace(*p)) p++;
	}

	if (_strnicmp(p, "error: ", 6) == 0) p += 6;
	while (p && isspace(*p)) p++;

	return p;
}

void c_engine::AddWarning(int nLine, const char* format, ...)
{
	char buffer[1024] = { 0 };
	va_list args;
	va_start(args, format);
	_vsnprintf_s(buffer, sizeof(buffer) - 1, format, args);
	va_end(args);

	// 현재 루틴 이름 가져오기
	c_string routineName;
	int temp_line = 0;
	get_line_context(nLine, &temp_line, &routineName);

	// 경고 추가
	ErrorInfo warning(nLine, buffer, routineName.get_buffer(), true);
	m_errors.add(warning);

	DebugLog("경고 추가: 라인=%d, 메시지=%s", nLine, buffer);
}

int c_engine::GetErrorCount() const
{
	int count = 0;
	c_array<ErrorInfo>& errors = const_cast<c_array<ErrorInfo>&>(m_errors);
	for (int i = 0; i < errors.get_size(); i++) {
		if (!errors[i].bWarning) count++;
	}
	return count;
}

int c_engine::GetWarningCount() const
{
	int count = 0;
	c_array<ErrorInfo>& errors = const_cast<c_array<ErrorInfo>&>(m_errors);
	for (int i = 0; i < errors.get_size(); i++) {
		if (!errors[i].bWarning) count++;
	}
	return count;
}

void c_engine::PrintAllErrors(bool bToDebugger)
{

	DebugLog("PrintAllErrors 호출: 오류 개수=%d", m_errors.get_size());

	if (m_errors.get_size() == 0) {
		if (bToDebugger) dbg_print("컴파일 성공: 오류 없음");
		return;
	}

	// 먼저 라인 번호로 정렬
	for (int i = 0; i < m_errors.get_size() - 1; i++) {
		for (int j = 0; j < m_errors.get_size() - i - 1; j++) {
			if (m_errors[j].nLine > m_errors[j + 1].nLine) {
				ErrorInfo temp = m_errors[j];
				m_errors[j] = m_errors[j + 1];
				m_errors[j + 1] = temp;
			}
		}
	}

	c_string output;
	char buffer[MAX_ERR_LENGTH + 1];

	sprintf_s(buffer, "컴파일 오류\r\n오류: %d개, 경고: %d개\r\n", GetErrorCount(), GetWarningCount());
	output = buffer;
	output += "--------------------------------------------\r\n";

	for (int i = 0; i < m_errors.get_size(); i++) {
		const ErrorInfo& info = m_errors[i];

		if (info.bWarning) {
			if (info.nLine != -1) {
				if (info.routineName.get_length() > 0) {
					sprintf_s(buffer, "[경고] 라인 %d (루틴: %s): %s\r\n",
						info.nLine,
						const_cast<c_string&>(info.routineName).get_buffer(),
						const_cast<c_string&>(info.message).get_buffer());
				}
				else {
					sprintf_s(buffer, "[경고] 라인 %d: %s\r\n",
						info.nLine,
						const_cast<c_string&>(info.message).get_buffer());
				}
			}
			else {
				sprintf_s(buffer, "[경고]: %s\r\n",
					const_cast<c_string&>(info.message).get_buffer());
			}
		}
		else {
			if (info.nLine != -1) {
				if (info.routineName.get_length() > 0) {
					sprintf_s(buffer, "[오류] 라인 %d (루틴: %s): %s\r\n",
						info.nLine,
						const_cast<c_string&>(info.routineName).get_buffer(),
						const_cast<c_string&>(info.message).get_buffer());
				}
				else {
					sprintf_s(buffer, "[오류] 라인 %d: %s\r\n",
						info.nLine,
						const_cast<c_string&>(info.message).get_buffer());
				}
			}
			else {
				sprintf_s(buffer, "[오류]: %s\r\n",
					const_cast<c_string&>(info.message).get_buffer());
			}
		}
		output += buffer;
	}

	DebugLog("최종 출력: %s", output.get_buffer());

	// MessageBox로 표시
	::MessageBox(NULL, output.get_buffer(), "컴파일 오류", MB_OK | MB_ICONERROR);

	// 디버거에 출력
	if (bToDebugger) {
		dbg_print(output.get_buffer());
	}

	// 파일에도 저장
	FILE* f = fopen("CompileResult.log", "w");
	if (f) {
		fprintf(f, "%s", output.get_buffer());
		fclose(f);
	}
}