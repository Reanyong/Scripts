#pragma once

#include "c_engine.h"
class  Basic
{

	c_engine	m_engine;
	HFONT		m_hTerminalFont;
	bool m_bSetSource;
	void get_extension_constant(const char* p_name, long* res);
	void get_extension_constant(const char* p_name, double* res);
public:
	Basic();
	~Basic();
	bool IsRunDebug();
	bool isSetSource()
	{
		return m_bSetSource;
	}
public:

	STDMETHOD(get_DebuggerVisible) (VARIANT_BOOL* res);
	STDMETHOD(put_DebuggerVisible) (VARIANT_BOOL val);

	STDMETHOD(get_Error) (BSTR* res);

	STDMETHOD(get_Line) (long* res);

	STDMETHOD(get_Ready) (bool& res);
	STDMETHOD(get_IsWaiting) (VARIANT_BOOL* res);
	STDMETHOD(get_Stopped) (bool& res);
	STDMETHOD(get_RunMode)(long* res);

	STDMETHOD(get_KwdList)(VARIANT* res);

	STDMETHOD(get_OpenPositionProfit)(double* res);
	STDMETHOD(put_OpenPositionProfit)(double res);

	STDMETHOD(get_TerminalLines)(long* res);
	STDMETHOD(put_TerminalLines)(long val);
	STDMETHOD(get_TerminalText)(BSTR* res);

	STDMETHOD(get_AllowDebugging)(VARIANT_BOOL* res);
	STDMETHOD(put_AllowDebugging)(VARIANT_BOOL val);

// methods

	STDMETHOD(AddObject)(BSTR name, IDispatch* obj, VARIANT_BOOL def, VARIANT_BOOL* OK);
	STDMETHOD(ExecRoutine)(BSTR script, VARIANT_BOOL dbg, VARIANT* pRes, VARIANT_BOOL* OK);

	STDMETHOD(Reset)();
	STDMETHOD(Run)();
	//STDMETHOD(SetSource)(BSTR script, VARIANT_BOOL* OK);
	STDMETHOD(EditScript)();
	STDMETHOD(SetSource)(c_string script);
	STDMETHOD(GetSource)(BSTR* script);
	STDMETHOD(Eval)(BSTR expression, VARIANT* res, VARIANT_BOOL* OK);
	STDMETHOD(SetConstant)(BSTR name, VARIANT* value);
	STDMETHOD(GetConstant)(BSTR name, VARIANT* res);
	STDMETHOD(GetErrorContext)(long* line, BSTR* routine);

	STDMETHOD(get_DbgHelp)(BSTR* pRet);
	STDMETHOD(put_DbgHelp)(BSTR FileName);

	STDMETHOD(Stop)();

	STDMETHOD(ClearError)();
	STDMETHOD(GetBreakpoints)(VARIANT* list);
	STDMETHOD(SetBreakpoints)(VARIANT* list);

	STDMETHOD(IsKeyword)(BSTR str, long length, VARIANT_BOOL* bRet);
	STDMETHOD(IsRoutine)(BSTR str, long length, VARIANT_BOOL* bRet);
	STDMETHOD(ListMembers)(BSTR type, SAFEARRAY** levels, SAFEARRAY** members, SAFEARRAY** methods);
	STDMETHOD(CapitalizeKeyword)(BSTR Kwd, BSTR* Capitalized);

	STDMETHOD(get_NumDebuggersOpen)(long* res);

	STDMETHOD(GetHint)(SAFEARRAY** levels, BSTR TypeName, BSTR* hint);
	STDMETHOD(AddHint) (BSTR keyword, BSTR hint, VARIANT_BOOL* OK);
};