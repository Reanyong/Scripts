// Basic.cpp : Implementation of CBeeBasicApp and DLL registration.

#include "pch.h"
#include "Basic.h"
#include "ScriptSystem.h"
//#include "c_wait_cursor.h"

BSTR alloc_BSTR(const char* p);

HRESULT BeeBasicCreateInstanceW(WCHAR* pKey, VARIANT* pVar)
{
	if (!pVar) return E_INVALIDARG;
	if (!pKey || !*pKey) return CLASS_E_NOTLICENSED;

	VariantClear(pVar);
	pVar->vt = VT_DISPATCH;
	pVar->pdispVal = 0;

	IDispatch* pDisp = 0;


/*
	//20210329 jhson
	//20210325  license 관련 제거
	IClassFactory* pCF = 0;
	HRESULT hr = CoGetClassObject(__uuidof(Basic), CLSCTX_ALL, 0, IID_IClassFactory, (void**)&pCF);
	_ASSERT(!FAILED(hr) && pCF);
	// this means you don't have valid serial number. OK, create in demo mode.
	hr = pCF->CreateInstance(0, IID_IDispatch, (void**)&pDisp);
	_ASSERT(!FAILED(hr) && pDisp);


	pCF->Release();

	_ASSERT(pDisp);
//	pDisp->AddRef();
	pVar->pdispVal = pDisp;
*/
	return S_OK;
}

HRESULT BeeBasicCreateInstanceA(const char* pKey, VARIANT* pVar)
{
	if (!pVar) return E_INVALIDARG;
	if (!pKey || !*pKey) return CLASS_E_NOTLICENSED;

	WCHAR wKey[64];


	size_t ConvertedChars = 0;
	mbstowcs_s(&ConvertedChars, wKey, pKey, sizeof(wKey));


	//
	//mbstowcs(wKey, pKey, sizeof(wKey));
	return BeeBasicCreateInstanceW(wKey, pVar);
}

//-----------------------------------------------------------------------------
// asynchronous routines ------------------------------------------------------
//-----------------------------------------------------------------------------
/*
void __stdcall asynchronous_step(unsigned long l)
{
	Basic* pb = (Basic*)l;
	pb->Fire_Step();
}

void __stdcall asynchronous_end(unsigned long l)
{
	Basic* pb = (Basic*)l;
	pb->Fire_End();
}

void __stdcall asynchronous_close(unsigned long l)
{
	Basic* pb = (Basic*)l;
	pb->Fire_DbgClose();
}

void __stdcall asynchronous_about(unsigned long l)
{
	Basic* pb = (Basic*)l;
	pb->Fire_DbgAbout();
}
//*/

/*
//20210329 jhson
//-----------------------------------------------------------------------------
// event callback routines ----------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall on_step(void* p)
{
	unsigned int n_ok = QueueUserAPC(asynchronous_step,
									 GetCurrentThread(),
									 (unsigned long)p);
	_ASSERT(n_ok);
	SleepEx(INFINITE, TRUE);
}

void __stdcall on_end(void* p)
{
	unsigned int n_ok = QueueUserAPC(asynchronous_end,
									 GetCurrentThread(),
									 (unsigned long)p);
	_ASSERT(n_ok);
	SleepEx(INFINITE, TRUE);
}

void __stdcall on_close(void* p)
{
	unsigned int n_ok = QueueUserAPC(asynchronous_close,
									 GetCurrentThread(),
									 (unsigned long)p);
	_ASSERT(n_ok);
	SleepEx(INFINITE, TRUE);
}

void __stdcall on_about(void* p)
{
	unsigned int n_ok = QueueUserAPC(asynchronous_about,
									 GetCurrentThread(),
									 (unsigned long)p);
	_ASSERT(n_ok);
	SleepEx(INFINITE, TRUE);
}
*/

//-----------------------------------------------------------------------------
// ThrowError -----------------------------------------------------------------
//-----------------------------------------------------------------------------

void ThrowError(const char* p)
{
	c_string s = p;
	BSTR bstr = s.sys_alloc_string();

	ICreateErrorInfo *pcerrinfo;
	IErrorInfo *perrinfo;
	HRESULT hr;

	hr = CreateErrorInfo(&pcerrinfo);

	// set fields here by calling ICreateErrorInfo methods on pcerrinfo
	pcerrinfo->SetHelpContext(0);
	pcerrinfo->SetDescription(bstr);
	//20220715 YDK 수정
	pcerrinfo->SetHelpFile(CT2OLE(_T("")));
	pcerrinfo->SetSource(CT2OLE(_T("")));

	hr = pcerrinfo->QueryInterface(IID_IErrorInfo, (LPVOID FAR*) &perrinfo);
	if (SUCCEEDED(hr))
	{
		hr = SetErrorInfo(0, perrinfo);
		_ASSERT(!FAILED(hr));
		perrinfo->Release();
	}
	pcerrinfo->Release();
}

//-----------------------------------------------------------------------------
// get_extension_constant -----------------------------------------------------
//-----------------------------------------------------------------------------

void Basic::get_extension_constant(const char* p_name, long* res)
{
	c_variable* p_val = m_engine.get_extension_constant(p_name);
	if (!p_val)
	{
		ASSERT(0); // should be added
		res = 0;
	}

	*res = p_val->as_integer();
}

//-----------------------------------------------------------------------------
// get_extension_constant -----------------------------------------------------
//-----------------------------------------------------------------------------

void Basic::get_extension_constant(const char* p_name, double* res)
{
	c_variable* p_val = m_engine.get_extension_constant(p_name);
	if (!p_val)
	{
		ASSERT(0); // should be added
		res = 0;
	}

	*res = p_val->as_double();
}

//-----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
 /*
STDMETHODIMP Basic::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IBasic,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
} */

//Basic::Basic() : m_engine(_Module.m_hInst)

Basic::Basic() : m_engine()
{
	/*m_hTerminalFont = CreateFont(
		-12,
		0,
		0,
		0,
		FW_MEDIUM,
		0,
		0,
		0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE,
		"Courier New"
		);

	if (m_hTerminalFont) m_engine.set_terminal_font(m_hTerminalFont);*/

	m_engine.set_notify_ptr(this);
	/*
//20210329 jhson
	m_engine.set_end_notify(on_end);
	m_engine.set_step_notify(on_step);
	m_engine.set_close_notify(on_close);
	m_engine.set_about_notify(on_about);
*/
//	if (!hHook)
//	{
//	}
//
//	ASSERT (hHook);

	m_bSetSource = false;
}

Basic::~Basic()
{
//	if (hHook) ::UnhookWindowsHookEx (hHook);
}

bool Basic::IsRunDebug()
{
	/*if (m_engine.m_pwnd != NULL)
	{
		if (m_engine.m_pwnd->m_pengine->m_pwnd->IsWindowVisible())
		{
			return true;
		}
	}*/

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_DebuggerVisible(/*[out, retval]*/ VARIANT_BOOL* res)
{
	*res = (m_engine.dbg_wnd_visible() ? -1 : 0);
	return S_OK;
}

STDMETHODIMP Basic::put_DebuggerVisible(/*[in]*/ VARIANT_BOOL val)
{
	m_engine.dbg_wnd_visible(val != 0);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_Error(/*[out, retval]*/ BSTR* res)
{
	c_string str = m_engine.get_error();
	*res = str.sys_alloc_string();
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_Line(/*[out, retval]*/ long* res)
{
	*res = m_engine.get_cur_line();
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_Ready(bool& res)
{
	res = m_engine.ready();
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_IsWaiting(/*[out, retval]*/ VARIANT_BOOL* res)
{
	*res = (m_engine.is_waiting() ? -1 : 0);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_Stopped(bool& res)
{
	res = m_engine.stopped();
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_RunMode(/*[out, retval]*/ long* res)
{
	*res = (long)m_engine.get_run_mode();
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_KwdList(/*[out, retval]*/ VARIANT* res)
{
	VariantClear(res);

	int i;
	c_str_array a;

	m_engine.list_all_obj_members(	true,
									true,
									true,
									&a);

	for (i = 0; i < NUM_TOKENS; i++)
		a.add(reserved_w[i]);

	for (i = 0; i < sizeof(types) / sizeof(type_map); i++)
		a.add(types[i]._name);

	m_engine.list_ext_consts(a);
	m_engine.list_ext_funcs(a);
	m_engine.list_ext_subs(a);

	SAFEARRAY* psa = SafeArrayCreateVector(VT_BSTR, 0, a.get_size());
	_ASSERT(psa);
	_ASSERT(psa->pvData);

	BSTR* p = (BSTR*)psa->pvData;

	for (i = 0; i < a.get_size(); i++)
	{
		//20220715 YDK 수정
		int n_len = MultiByteToWideChar(CP_ACP, 0, a[i], (int)strlen(a[i]), NULL, NULL);
		p[i] = ::SysAllocStringLen(NULL, n_len);
		_ASSERT(p[i]);
		//20220715 YDK 수정
		MultiByteToWideChar(CP_ACP, 0, a[i], (int)strlen(a[i]), p[i], n_len);
	}

	res->vt = (VT_ARRAY | VT_BSTR);
	res->parray = psa;

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_OpenPositionProfit(/*[out, retval]*/ double* res)
{
	get_extension_constant("OpenPositionProfit", res);
	return S_OK;
}

STDMETHODIMP Basic::put_OpenPositionProfit(/*[in]*/ double val)
{
	m_engine.add_extension_constant("OpenPositionProfit", val);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


STDMETHODIMP Basic::get_TerminalLines(/*[out, retval]*/ long* res)
{
	*res = m_engine.get_terminal_lines();
	return S_OK;
}

STDMETHODIMP Basic::put_TerminalLines(/*[in]*/ long val)
{
	m_engine.set_terminal_lines(val);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


STDMETHODIMP Basic::get_TerminalText(/*[out, retval]*/ BSTR* res)
{
	c_terminal* p_term = m_engine.get_terminal();
	_ASSERT(p_term);

	c_string s;
	p_term->get_text(&s);

	*res = s.sys_alloc_string();
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_AllowDebugging(/*[out, retval]*/ VARIANT_BOOL* res)
{
	*res = (m_engine.get_allow_debugging() ? -1 : 0);
	return S_OK;
}

STDMETHODIMP Basic::put_AllowDebugging(/*[in]*/ VARIANT_BOOL val)
{
	m_engine.set_allow_debugging(val != 0);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


STDMETHODIMP Basic::AddObject(/*[in]*/ BSTR name,
							  /*[in]*/ IDispatch* obj,
							  /*[in]*/ VARIANT_BOOL def,
							  /*[out, retval]*/ VARIANT_BOOL* OK)
{
	if (m_engine.is_waiting())
	{
#ifdef _DEBUG
		::MessageBox(0, "Trying to add object when engine is running", "DEBUG", 0);
#endif
		return S_FALSE;
	}

	if (!obj)
	{
#ifdef _DEBUG
		::MessageBox(0, "Trying to add NULL object", "DEBUG", 0);
#endif
		return S_FALSE;
	}

	if (m_engine.is_added_object(obj)) return TRUE;

	c_string s_name = name;
	*OK = (m_engine.add_object(s_name.get_buffer(), obj, def != 0) ? -1 : 0);

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::ExecRoutine(/*[in]*/ BSTR script,
								/*[in]*/ VARIANT_BOOL dbg,
								/*[out]*/ VARIANT* pRes,
								/*[out, retval]*/ VARIANT_BOOL* OK)
{

	c_string s_code = script;
	*OK = (m_engine.run_routine(s_code.get_buffer(), dbg != 0, pRes) ? -1 : 0);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::Reset()
{
	m_engine.reset();
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
STDMETHODIMP Basic::EditScript()
{
	m_engine.dbg_wnd_visible(true);
	return S_OK;
}
STDMETHODIMP Basic::Run()
{
	if (!m_engine.run_go())
	{
		return E_FAIL;
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::SetSource(c_string script)
{
	if (m_engine.is_waiting())
	{
		m_engine.error(NO_ERR_LINE, "Cannot set source when the engine is in waiting state.");
		return E_FAIL;
	}

	c_string s_code = script;
	m_bSetSource = true;
	if (!m_engine.set_buffer(s_code.get_buffer()) ? -1 : 0)
	{
		return E_FAIL;
	}

	//c_string	error = GetError();
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::GetSource(/*[out, retval]*/ BSTR* script)
{
	c_string str_source;
	str_source = m_engine.get_buffer();
	*script = str_source.sys_alloc_string();

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::AddHint(/*[in]*/ BSTR keyword,
							/*[in]*/ BSTR hint,
							/*[out, retval]*/ VARIANT_BOOL* OK)
{
	c_string s_hint = hint;
	c_string s_keyword = keyword;

	*OK = (m_engine.add_hint(s_keyword.get_buffer(), s_hint.get_buffer()) ? -1 : 0);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//STDMETHODIMP Basic::CheckSyntax(/*[in]*/ BSTR code, /*[out, retval]*/ VARIANT_BOOL* OK)
//{
//	c_string s_code = code;
//
//	*OK = (m_engine.check_syntax(s_code.get_buffer()) ? -1 : 0);
//	return S_OK;
//}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::Eval(/*[in]*/ BSTR expression,
						 /*[out]*/ VARIANT* res,
						 /*[out, retval]*/ VARIANT_BOOL* OK)
{
	VariantClear(res);
	res->wReserved1 = 0;
	res->wReserved2 = 0;
	res->wReserved3 = 0;

	c_string s_expression = expression;
	bool b = m_engine.eval_watch(s_expression.get_buffer(), (c_variable*)res);
	*OK = (b ? -1 : 0);

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//STDMETHODIMP Basic::DeleteHint(/*[in]*/ BSTR keyword)
//{
//	c_string s_keyword = keyword;
//	m_engine.delete_hint(s_keyword.get_buffer());
//	return S_OK;
//}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//STDMETHODIMP Basic::DeleteAllHints()
//{
//	m_engine.delete_all_hints();
//	return S_OK;
//}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//STDMETHODIMP Basic::ClearWatch()
//{
//	m_engine.dbg_watch_clear();
//	return S_OK;
//}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::SetConstant(/*[in]*/ BSTR name, /*[in]*/ VARIANT* value)
{
	c_string s_name = name;
	m_engine.add_extension_constant(s_name.get_buffer(), (c_variable*)&value);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::GetConstant(/*[in]*/ BSTR name, /*[out, retval]*/ VARIANT* res)
{
	VariantClear(res);

	c_string s_name = name;
	c_variable* p_v = m_engine.get_extension_constant(s_name.get_buffer());
	//20220715 YDK 수정
	if (FAILED(VariantCopy(res, p_v)))
	{
		return S_FALSE;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::GetErrorContext(/*[out]*/ long* line, /*[out]*/ BSTR* routine)
{
	c_string str_routine;
	int n_line;

	m_engine.get_error_context(&n_line, &str_routine);

	if (str_routine.get_length())
	{
		*line = n_line;
		*routine = str_routine.sys_alloc_string();
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::get_DbgHelp(/*[out, retval]*/ BSTR* pRet)
{
	c_string s;
	s = m_engine.get_dbg_help();
	*pRet = s.sys_alloc_string();

	return S_OK;
}

STDMETHODIMP Basic::put_DbgHelp(/*[in]*/ BSTR FileName)
{
	c_string s_name = FileName;

	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(s_name.get_buffer(), &fd);

	if (h == INVALID_HANDLE_VALUE)
	{
		c_string msg;
		msg.format("DbgHelp : FileName '%s' seems to be incorrect.", s_name.get_buffer());
		MessageBox(0, msg, "Error", MB_ICONERROR);
	}
	else FindClose(h);

	m_engine.set_dbg_help(s_name.get_buffer());

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::Stop()
{
	m_engine.stop();
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::ClearError()
{
	m_engine.clear_error();
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::GetBreakpoints(/*[out]*/ VARIANT* list)
{
	VariantClear(list);

	c_str_array a;
	m_engine.format_bp_list(&a);

	a.fill_variant(list);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::SetBreakpoints(/*[in]*/ VARIANT* list)
{
	c_str_array a;
	a.from_safearray(list->parray);
	m_engine.set_bp_list(&a);

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::IsKeyword(/*[in]*/ BSTR str,
							  /*[in]*/ long length,
							  /*[out, retval]*/ VARIANT_BOOL* bRet)
{
	c_string s_str = str;
	*bRet = (m_engine.is_keyword(s_str.get_buffer(), length) ? -1 : 0);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::IsRoutine(/*[in]*/ BSTR str,
							  /*[in]*/ long length,
							  /*[out, retval]*/ VARIANT_BOOL* bRet)
{
	c_string s_str = str;
	*bRet = (m_engine.is_routine(s_str.get_buffer(), static_cast<int>(length)) ? -1 : 0);
	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::GetHint(/*[in]*/ SAFEARRAY** levels,
							/*[in]*/ BSTR TypeName,
							/*[out]*/ BSTR* hint)
{
	c_str_array arr_levels;
	const char* p_hint = 0;
	c_string sTypeName = TypeName;

	if (*hint) SysFreeString(*hint);

	arr_levels.from_safearray(*levels);
	m_engine.get_hint(&arr_levels, &p_hint, sTypeName.get_buffer());

	*hint = alloc_BSTR(p_hint);

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::ListMembers(/*[in]*/ BSTR type,
								/*[in]*/ SAFEARRAY** levels,
								/*[out]*/ SAFEARRAY** members,
								/*[out]*/	 SAFEARRAY** methods)
{
	c_str_array arr_levels;
	c_str_array arr_members;
	c_array<bool> arr_methods;
	c_string s_type = type;

	arr_levels.from_safearray(*levels);

	m_engine.list_members(s_type.get_buffer(), &arr_levels, &arr_members, &arr_methods);
	arr_members.fill_safearray(members);

	if (*methods) SafeArrayDestroy(*methods);
	*methods = SafeArrayCreateVector(VT_BOOL, 0, arr_methods.get_size());
	_ASSERT(*methods);

	long i;
	VARIANT_BOOL* p_bool;
	for (i = 0; i < arr_methods.get_size(); i++)
	{
		p_bool = 0;
		HRESULT hr = SafeArrayPtrOfIndex(*methods, &i, (void**)&p_bool);
		_ASSERT(!FAILED(hr) && p_bool);
		*p_bool = (arr_methods[i] ? -1 : 0);
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

STDMETHODIMP Basic::CapitalizeKeyword(/*[in]*/ BSTR Kwd,
									  /*[out]*/ BSTR* Capitalized)
{
	c_string s = Kwd;
	m_engine.capitalize_keyword((char*)s.get_buffer());
	*Capitalized = s.sys_alloc_string();

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// todo: this does not work!!!
// custom class name is not applied to Edit frame!

	int __n_windows;
	BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
	{
		char ClassName[256];
		if (GetClassName(hwnd, ClassName, sizeof(ClassName)))
		{
			if (!strcmp(ClassName, DEBUG_WND_CLASS_NAME))
				__n_windows ++;
		}

		return TRUE;
	}

STDMETHODIMP Basic::get_NumDebuggersOpen(/*[out, retval]*/ long* res)
{
	__n_windows = 0;
	EnumWindows(EnumWindowsProc, 0);
	*res = __n_windows;
	return S_OK;
}