#include "pch.h"

#include <crtdbg.h>
#include "c_engine.h"
#include "cast_extensions.h"

bool std_cast_check(int n,
					VARENUM* p_types,
					c_string* p_msg,
					const char* p_funcname)
{
	if (n != 1)
	{
		*p_msg = "'";
		*p_msg += p_funcname;
		*p_msg += "' function gets 1 argument.";
		return false;
	}
	
//	if (p_types[0] & VT_DISPATCH)
//	{
//		*p_msg = "'";
//		*p_msg += p_funcname;
//		*p_msg += "' function should not get object arguments.";
//		return false;
//	}

	return true;
}

//-----------------------------------------------------------------------------
// IsInteger ------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall IsInteger(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = (int)((pargs[0]->vt == VT_I4) ||
					(pargs[0]->vt == VT_INT) ||
					(pargs[0]->vt == VT_I2));
}

bool _check_IsInteger(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "IsInteger");
}

//-----------------------------------------------------------------------------
// IsDouble -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall IsDouble(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = (int)(pargs[0]->vt == VT_R8);
}

bool _check_IsDouble(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "IsDouble");
}

//-----------------------------------------------------------------------------
// IsString -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall IsString(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = (int)(pargs[0]->vt == VT_BSTR);
}

bool _check_IsString(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "IsString");
}

//-----------------------------------------------------------------------------
// IsArray --------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall IsArray(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = (int)(pargs[0]->vt == (VT_VARARRAY));
}

bool _check_IsArray(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "IsArray");
}


//-----------------------------------------------------------------------------
// IsObject -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall IsObject(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = (int)(pargs[0]->vt == VT_DISPATCH);
}

bool _check_IsObject(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "IsObject");
}

//-----------------------------------------------------------------------------
// IsEmpty --------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall IsEmpty(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = (int)(pargs[0]->vt == VT_EMPTY);
}

bool _check_IsEmpty(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "IsEmpty");
}

//-----------------------------------------------------------------------------
// IsNull ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall IsNull(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = (int)( (pargs[0]->vt == VT_DISPATCH) && (pargs[0]->pdispVal == 0) );
}

bool _check_IsNull(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "IsNull");
}

//-----------------------------------------------------------------------------
// CInt -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall CInt(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = pargs[0]->as_integer();
}

bool _check_CInt(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "CInt");
}

//-----------------------------------------------------------------------------
// CDbl -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall CDbl(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = pargs[0]->as_double();
}

bool _check_CDbl(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "CDbl");
}

//-----------------------------------------------------------------------------
// CStr -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall CStr(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	c_string str;
	pargs[0]->as_string(str);
	result = str;
}

bool _check_CStr(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "CStr");
}

//-----------------------------------------------------------------------------
// CDate ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall CDate(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);

	c_variable* p = pargs[0]->get_scalar_ptr();
	c_variable* p_res = &result;

	PUSH_RESERVED(p_res)

	HRESULT hr = VariantChangeType(	p_res,
									p,
									VARIANT_NOVALUEPROP | VARIANT_ALPHABOOL,
									VT_DATE);

	POP_RESERVED(p_res)

	p_engine->runtime_error(hr);

}

bool _check_CDate(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_cast_check(n, p_types, p_msg, "CDate");
}
