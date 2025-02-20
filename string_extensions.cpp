#include "pch.h"

#include <crtdbg.h>
#include "string_extensions.h"

#define NO_OBJ_ARGS(func_name) \
if (p_types[0] == VT_DISPATCH)\
{\
	*p_msg = "'";\
	*p_msg += func_name;\
	*p_msg += "' function should not get object arguments.";\
	return false;\
}\

BSTR alloc_BSTR(const char* p);

// Len ------------------------------------------------------------------------

void __stdcall Len (int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	c_string str;
	pargs[0]->as_string(str);
	result = str.get_length();
}

bool _check_Len(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Len' function gets 1 argument.";
		return false;
	}
//	NO_OBJ_ARGS("Len")
	return true;
}

// InStr ----------------------------------------------------------------------

void __stdcall InStr (int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 2);
	c_string str0, str1;

	pargs[0]->as_string(str0);
	pargs[1]->as_string(str1);

	const char *p0 = str0.get_buffer();
	const char *p1 = str1.get_buffer();

	if (!p0 || !p1)
	{
		result = -1;
		return;
	}
	
	result = int(UINT64(strstr(p0, p1)) - UINT64(p0));
	if (result.as_integer() < -1) result = (int)-1;
}

bool _check_InStr(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'InStr' function gets 2 arguments.";
		return false;
	}
//	NO_OBJ_ARGS("InStr")
	return true;
}

// Mid ------------------------------------------------------------------------

void __stdcall Mid (int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 3);

	int n_start  = pargs[1]->as_integer();
	int n_length = pargs[2]->as_integer();

	c_string str;
	pargs[0]->as_string(str);

	int n_str_len = str.get_length();

	if (n_start  < 0) n_start  = 0;
	if (n_length < 0) n_length = 0;
	if (n_start  > n_str_len) n_start  = n_str_len;
	if (n_length > n_str_len) n_length = n_str_len;

	c_string temp;
	temp.copy(str.get_buffer() + n_start, n_length);
	result = temp;
	_ASSERT(result.vt == VT_BSTR);
}

bool _check_Mid(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'Mid' function gets 3 arguments.";
		return false;
	}
//	NO_OBJ_ARGS("Mid")
	return true;
}

// Left -----------------------------------------------------------------------

void __stdcall Left (int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 2);

	c_string str;
	pargs[0]->as_string(str);

	int n_length = pargs[1]->as_integer();
	int n_str_len = str.get_length();

	if (n_length < 0) n_length = 0;
	if (n_length > n_str_len) n_length = n_str_len;

	c_string temp;
	temp.copy(str, n_length);
	result = temp;
	_ASSERT(result.vt == VT_BSTR);
}

bool _check_Left(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'Left' function gets 2 arguments.";
		return false;
	}
//	NO_OBJ_ARGS("Left")
	return true;
}

// Right ----------------------------------------------------------------------

void __stdcall Right (int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 2);

	c_string str;
	pargs[0]->as_string(str);

	int n_length = pargs[1]->as_integer();
	int n_str_len = str.get_length();

	if (n_length < 0) n_length = 0;
	if (n_length > n_str_len) n_length = n_str_len;

	c_string temp;
	temp.copy(str + (n_str_len - n_length), n_length);
	result = temp;
	_ASSERT(result.vt == VT_BSTR);
}

bool _check_Right(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'Right' function gets 2 arguments.";
		return false;
	}
//	NO_OBJ_ARGS("Right")
	return true;
}

// RTrim ----------------------------------------------------------------------

void __stdcall RTrim (int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	c_string temp;
	pargs[0]->as_string(temp);
	temp.r_trim();
	result = temp;
	_ASSERT(result.vt == VT_BSTR);
}

bool _check_RTrim(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'RTrim' function gets 1 argument.";
		return false;
	}
//	NO_OBJ_ARGS("RTrim")
	return true;
}

// LTrim ----------------------------------------------------------------------

void __stdcall LTrim (int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{

	_ASSERT(nargs == 1);
	c_string temp;
	pargs[0]->as_string(temp);
	temp.l_trim();
	result = temp;
	_ASSERT(result.vt == VT_BSTR);
}

bool _check_LTrim(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'' function gets 1 argument.";
		return false;
	}
//	NO_OBJ_ARGS("LTrim")
	return true;
}

// Trim -----------------------------------------------------------------------

void __stdcall Trim (int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	c_string temp;
	pargs[0]->as_string(temp);
	temp.r_trim();
	temp.l_trim();
	result = temp;
	_ASSERT(result.vt == VT_BSTR);
}

bool _check_Trim(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Trim' function gets 1 argument.";
		return false;
	}
//	NO_OBJ_ARGS("Trim")
	return true;
}

// UCase ----------------------------------------------------------------------

void __stdcall UCase(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	c_string temp;
	pargs[0]->as_string(temp);
	temp.u_case();
	result = temp;
	_ASSERT(result.vt == VT_BSTR);
}

bool _check_UCase(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'UCase' function gets 1 argument.";
		return false;
	}
//	NO_OBJ_ARGS("UCase")
	return true;
}

// LCase ----------------------------------------------------------------------

void __stdcall LCase(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	c_string temp;
	pargs[0]->as_string(temp);
	temp.l_case();
	result = temp;
	_ASSERT(result.vt == VT_BSTR);
}

bool _check_LCase(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'LCase' function gets 1 argument.";
		return false;
	}
//	NO_OBJ_ARGS("LCase")
	return true;
}

// Chr ------------------------------------------------------------------------

void __stdcall Chr (int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	c_string temp;

	for (int i = 0; i < nargs; i++)
		temp += (char)pargs[i]->as_integer();

	result = temp;
	_ASSERT(result.vt == VT_BSTR);
}

bool _check_Chr(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n < 1)
	{
		*p_msg = "'Chr' function gets 1 or more arguments.";
		return false;
	}
//	NO_OBJ_ARGS("Chr")
	return true;
}

// Asc ------------------------------------------------------------------------

void __stdcall Asc (int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	c_string str;
	pargs[0]->as_string(str);
	LPCTSTR pstr = str.get_buffer();
	if (pstr)
	{
		result = (int)(char)pstr[0];
		_ASSERT(result.vt == VT_I4);
	}
	else
		result = 0;
}

bool _check_Asc(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Asc' function gets 1 argument.";
		return false;
	}
//	NO_OBJ_ARGS("Asc")
	return true;
}

// Split ----------------------------------------------------------------------

void __stdcall Split(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs!=2)
	{
		result = INT_MIN;
		return;
	}
	for (int i = 0; i < 2; i++)
	{
		if (pargs[i]->vt != VT_BSTR)
		{
			result = INT_MIN;
			return;
		}
	}
	c_string s;
	c_string delim;
	c_str_array list;

	pargs[0]->as_string(s);
	pargs[1]->as_string(delim);

	list.split(s.get_buffer(), delim.get_buffer());

	result.reset();
	result.vt = VT_VARARRAY;
	result.parray = SafeArrayCreateVector(VT_VARIANT, 0, list.get_size());

	long i;
	for (i = 0; i < list.get_size(); i++)
	{
		VARIANT* p_var = 0;
		HRESULT hr = SafeArrayPtrOfIndex(result.parray, &i, (void**)&p_var);
		_ASSERT(!FAILED(hr) && p_var);

		p_var->vt = VT_BSTR;
		p_var->bstrVal = alloc_BSTR(list[i]);
	}
}

bool _check_Split(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'Split' function gets 2 arguments.";
		return false;
	}

	NO_OBJ_ARGS("Split")
	return true;
}
