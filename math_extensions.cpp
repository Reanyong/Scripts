#include "pch.h"

#include <crtdbg.h>
#include "math_extensions.h"
#include "c_engine.h"

//-----------------------------------------------------------------------------
// array_stat -----------------------------------------------------------------
//-----------------------------------------------------------------------------

void array_stat(SAFEARRAY* p_array, c_engine* p_engine, int n_offset, int n_count,
				double* p_dmax, double* p_dmin, double* p_avg,
				int* p_nmax, int* p_nmin, double* p_dstddev, double* p_dstddevs)
{
	c_variable* p_data;
	HRESULT hr;
	double d_cur;
	double d_sum = 0;
	int i;

// fit into bounds ------------------------------------------------------------

	if ( (unsigned)(n_offset + n_count) > /*n_bar*/ p_array->rgsabound[0].cElements)
		n_count = p_array->rgsabound[0].cElements;//n_bar - n_offset + 1;
	if (n_count <= 0)
	{
		*p_dmax = 0;
		*p_dmin = 0;
		*p_nmax = 0;
		*p_nmin = 0;
		*p_avg = 0;
		return;
	}

// calc -----------------------------------------------------------------------

	hr = SafeArrayLock(p_array);
	_ASSERT(!FAILED(hr));

	hr = SafeArrayAccessData(p_array, (void**)&p_data);
	_ASSERT(!FAILED(hr) && p_data);

	d_cur = p_data[n_offset].as_double();
	d_sum = d_cur;
	*p_dmin = d_cur;
	*p_dmax = d_cur;
	*p_nmin = n_offset;
	*p_nmax = n_offset;

	for (i = n_offset + 1; i < n_offset + n_count; i++)
	{
		if ((unsigned)i >= p_array->rgsabound[0].cElements) break;

		d_cur = p_data[i].as_double();
		d_sum += d_cur;
		if (*p_dmin > d_cur)
		{
			*p_dmin = d_cur;
			*p_nmin = i;
		}
		if (*p_dmax < d_cur)
		{
			*p_dmax = d_cur;
			*p_nmax = i;
		}
	}

	*p_avg = d_sum / n_count;

	if (p_dstddev || p_dstddevs)
	{
		double d = 0;

		for (i = n_offset; i < n_offset + n_count; i++)
		{
			if ((unsigned)i >= p_array->rgsabound[0].cElements) break;
			d += ((p_data[i].as_double() - *p_avg) * (p_data[i].as_double() - *p_avg));
		}

		if (p_dstddev) *p_dstddev = sqrt(d / n_count);
		if (p_dstddevs)
			if (n_count != 1) *p_dstddevs = sqrt(d / (n_count - 1));
	}

	SafeArrayUnaccessData(p_array);
	SafeArrayUnlock(p_array);

}


bool std_math_check(int n,
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

//	if (p_types[0])
//		if (strlen(p_types[0]))
//			if (stricmp(p_types[0], "integer") != 0 &&
//				stricmp(p_types[0], "string") != 0 &&
//				stricmp(p_types[0], "floating") != 0)
//			{
//				*p_msg = "'";
//				*p_msg += p_funcname;
//				*p_msg += "' function should not get object arguments.";
//				return false;
//			}

	return true;
}

// sqr ------------------------------------------------------------------------

void __stdcall sqr(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	result = sqrt(pargs[0]->as_double());
}

bool _check_sqr(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Sqr");
}

// hypot ----------------------------------------------------------------------

void __stdcall hypot(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 2);
	result = hypot(pargs[0]->as_double(), pargs[1]->as_double());
}

bool _check_hypot(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg += "'Hypot' function gets 2 arguments.";
		return false;
	}
	return true;
}

// abs -----------------------------------------------------------------------

void __stdcall abs(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	HRESULT hr = VarAbs(pargs[0]->get_scalar_ptr(), result.get_scalar_ptr());
	p_engine->runtime_error(hr);
}

bool _check_abs(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Abs");
}

// sin ------------------------------------------------------------------------

void __stdcall sin(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	result = (double)sin(pargs[0]->as_double());
}

bool _check_sin(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Sin");
}

// cos ------------------------------------------------------------------------

void __stdcall cos(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	result = (double)cos(pargs[0]->as_double());
}

bool _check_cos(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Cos");
}

// tan ------------------------------------------------------------------------

void __stdcall tan(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	result = (double)tan(pargs[0]->as_double());
}

bool _check_tan(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Tan");
}

// atan -----------------------------------------------------------------------

void __stdcall atan(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = (double)atan(pargs[0]->as_double());
}

bool _check_atan(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Atan");
}

// asin -----------------------------------------------------------------------

void __stdcall asin(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = (double)asin(pargs[0]->as_double());
}

bool _check_asin(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Asin");
}

// acos -----------------------------------------------------------------------

void __stdcall acos(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = (double)acos(pargs[0]->as_double());
}

bool _check_acos(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Acos");
}

// atan2 ----------------------------------------------------------------------

void __stdcall atan2(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 2);
	result = (double)atan2(pargs[0]->as_double(), pargs[1]->as_double());
}

bool _check_atan2(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg += "'Atan2' function gets 1 argument.";
		return false;
	}
	return true;
}

// RadToDeg -------------------------------------------------------------------

void __stdcall RadToDeg(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = pargs[0]->as_double() * 180 / 3.14159265358979;
}

bool _check_RadToDeg(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "RadToDeg");
}

// DegToRad -------------------------------------------------------------------

void __stdcall DegToRad(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	result = pargs[0]->as_double() * 3.14159265358979/ 180;
}

bool _check_DegToRad(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "DegToRad");
}

// exp ------------------------------------------------------------------------

void __stdcall exp(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	result = (double)exp(pargs[0]->as_double());
}

bool _check_exp(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Exp");
}

// log ------------------------------------------------------------------------

void __stdcall log(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	result = (double)log(pargs[0]->as_double());
}

bool _check_log(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Log");
}

// log10 ----------------------------------------------------------------------

void __stdcall log10(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	result = (double)log10(pargs[0]->as_double());
}

bool _check_log10(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Log10");
}

// ceil -----------------------------------------------------------------------

void __stdcall ceil(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	result = (double)ceil(pargs[0]->as_double());
}

bool _check_ceil(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Ceil");
}

// floor ----------------------------------------------------------------------

void __stdcall floor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	result = (double)floor(pargs[0]->as_double());
}

bool _check_floor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Floor");
}

// rnd ------------------------------------------------------------------------
//20231102 YDK ¼öÁ¤
//static bool __bee_randomized = false;
void __stdcall rnd(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 2);
	if (nargs != 2)
	{
		result = INT_MIN;
		return;
	}
	for (int i = 0; i < 2; i++)
	{
		switch (pargs[i]->vt)
		{
		case VT_EMPTY:
		case VT_NULL:
		case VT_CY:
		case VT_DATE:
		case VT_BSTR:
		case VT_DISPATCH:
		case VT_ERROR:
		case VT_BOOL:
		case VT_VARIANT:
		case VT_UNKNOWN:
		case VT_DECIMAL:
		case VT_VOID:
		case VT_HRESULT:
		case VT_PTR:
		case VT_SAFEARRAY:
		case VT_CARRAY:
		case VT_USERDEFINED:
		case VT_LPSTR:
		case VT_LPWSTR:
		case VT_RECORD:
		case VT_INT_PTR:
		case VT_UINT_PTR:
		case VT_FILETIME:
		case VT_BLOB:
		case VT_STREAM:
		case VT_STORAGE:
		case VT_STREAMED_OBJECT:
		case VT_STORED_OBJECT:
		case VT_BLOB_OBJECT:
		case VT_CF:
		case VT_CLSID:
		case VT_VERSIONED_STREAM:
		case VT_BSTR_BLOB:
		case VT_VECTOR:
		case VT_ARRAY:
		case VT_BYREF:
		case VT_RESERVED:
		case VT_ILLEGAL:
			result = INT_MIN;
			return;
		default:
			break;
		}
	}

	if (nargs == 2)
	{
		int MIN = pargs[0]->as_integer();
		int MAX = pargs[1]->as_integer();
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<double> distr(MIN, MAX);

		result = distr(eng);
	}
	else
	{
		result = 0;
	}

}

bool _check_rnd(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg += "'Rnd' function gets 2 argument.";
		return false;
	}
	return true;
}

// sign -----------------------------------------------------------------------

void __stdcall sign(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	switch (pargs[0]->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
	case VT_DISPATCH:
	case VT_ERROR:
	case VT_BOOL:
	case VT_VARIANT:
	case VT_UNKNOWN:
	case VT_DECIMAL:
	case VT_VOID:
	case VT_HRESULT:
	case VT_PTR:
	case VT_SAFEARRAY:
	case VT_CARRAY:
	case VT_USERDEFINED:
	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_RECORD:
	case VT_INT_PTR:
	case VT_UINT_PTR:
	case VT_FILETIME:
	case VT_BLOB:
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
	case VT_CF:
	case VT_CLSID:
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
	case VT_RESERVED:
	case VT_ILLEGAL:
		result = INT_MIN;
		return;
	default:
		break;
	}
	double d = pargs[0]->as_double();

	if (d < 0) result = -1;
	else if (d > 0) result = 1;
	else result = 0;
}

bool _check_sign(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	return std_math_check(n, p_types, p_msg, "Sign");
}

//-----------------------------------------------------------------------------
// max ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall bee_max(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs >= 1);
	if (nargs < 1)
	{
		result = INT_MIN;
		return;
	}
	if (nargs > 1)
	{
		// list of values
		double d = pargs[0]->as_double();
		double d1;

		int i;
		for (i= 1; i < nargs; i++)
		{
			switch (pargs[i]->vt)
			{
			case VT_EMPTY:
			case VT_NULL:
			case VT_CY:
			case VT_DATE:
			case VT_BSTR:
			case VT_DISPATCH:
			case VT_ERROR:
			case VT_BOOL:
			case VT_VARIANT:
			case VT_UNKNOWN:
			case VT_DECIMAL:
			case VT_VOID:
			case VT_HRESULT:
			case VT_PTR:
			case VT_SAFEARRAY:
			case VT_CARRAY:
			case VT_USERDEFINED:
			case VT_LPSTR:
			case VT_LPWSTR:
			case VT_RECORD:
			case VT_INT_PTR:
			case VT_UINT_PTR:
			case VT_FILETIME:
			case VT_BLOB:
			case VT_STREAM:
			case VT_STORAGE:
			case VT_STREAMED_OBJECT:
			case VT_STORED_OBJECT:
			case VT_BLOB_OBJECT:
			case VT_CF:
			case VT_CLSID:
			case VT_VERSIONED_STREAM:
			case VT_BSTR_BLOB:
			case VT_VECTOR:
			case VT_ARRAY:
			case VT_BYREF:
			case VT_RESERVED:
			case VT_ILLEGAL:
				result = INT_MIN;
				return;
			default:
				break;
			}
			d1 = pargs[i]->as_double();
			if (d < d1) d = d1;
		}

		result = d;
	}
	else
	{
		// one argument. array.
		if (pargs[0]->vt & VT_ARRAY)
		{
			SAFEARRAY* p_array = pargs[0]->parray;
			if (p_array)
			{
				double dmax, dmin, davg;
				int nmax, nmin;
				double dstddev, dstdevs;

				array_stat(p_array, p_engine, 0, p_array->rgsabound[0].cElements, &dmax, &dmin, &davg, &nmax, &nmin, &dstddev, &dstdevs);
				result = dmax;
			}
		}
		else
		{
			result = *pargs[0];
		}
	}
}

bool _check_max(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n < 1)
	{
		*p_msg = "'Max' function gets at least 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// min ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall bee_min(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs >= 1);
	if (nargs < 1)
	{
		result = INT_MIN;
		return;
	}
	if (nargs > 1)
	{
		// list of values
		double d = pargs[0]->as_double();
		double d1;

		int i;
		for (i= 1; i < nargs; i++)
		{
			switch (pargs[i]->vt)
			{
			case VT_EMPTY:
			case VT_NULL:
			case VT_CY:
			case VT_DATE:
			case VT_BSTR:
			case VT_DISPATCH:
			case VT_ERROR:
			case VT_BOOL:
			case VT_VARIANT:
			case VT_UNKNOWN:
			case VT_DECIMAL:
			case VT_VOID:
			case VT_HRESULT:
			case VT_PTR:
			case VT_SAFEARRAY:
			case VT_CARRAY:
			case VT_USERDEFINED:
			case VT_LPSTR:
			case VT_LPWSTR:
			case VT_RECORD:
			case VT_INT_PTR:
			case VT_UINT_PTR:
			case VT_FILETIME:
			case VT_BLOB:
			case VT_STREAM:
			case VT_STORAGE:
			case VT_STREAMED_OBJECT:
			case VT_STORED_OBJECT:
			case VT_BLOB_OBJECT:
			case VT_CF:
			case VT_CLSID:
			case VT_VERSIONED_STREAM:
			case VT_BSTR_BLOB:
			case VT_VECTOR:
			case VT_ARRAY:
			case VT_BYREF:
			case VT_RESERVED:
			case VT_ILLEGAL:
				result = INT_MIN;
				return;
			default:
				break;
			}
			d1 = pargs[i]->as_double();
			if (d > d1) d = d1;
		}

		result = d;
	}
	else
	{
		// one argument. array.
		if (pargs[0]->vt & VT_ARRAY)
		{
			SAFEARRAY* p_array = pargs[0]->parray;
			if (p_array)
			{
				double dmax, dmin, davg;
				int nmax, nmin;
				double dstddev, dstdevs;

				array_stat(p_array, p_engine, 0, p_array->rgsabound[0].cElements, &dmax, &dmin, &davg, &nmax, &nmin, &dstddev, &dstdevs);
				result = dmin;
			}
		}
		else
		{
			result = INT_MIN;
		}
	}
}

bool _check_min(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n < 1)
	{
		*p_msg = "'Min' function gets at least 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// avg ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall bee_avg(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs > 1);
	if (nargs <= 1)
	{
		result = INT_MIN;
		return;
	}
	double d = pargs[0]->as_double();
	double sum = 0;

	int i;
	for (i = 0; i < nargs; i++)
	{
		switch (pargs[i]->vt)
		{
		case VT_EMPTY:
		case VT_NULL:
		case VT_CY:
		case VT_DATE:
		case VT_BSTR:
		case VT_DISPATCH:
		case VT_ERROR:
		case VT_BOOL:
		case VT_VARIANT:
		case VT_UNKNOWN:
		case VT_DECIMAL:
		case VT_VOID:
		case VT_HRESULT:
		case VT_PTR:
		case VT_SAFEARRAY:
		case VT_CARRAY:
		case VT_USERDEFINED:
		case VT_LPSTR:
		case VT_LPWSTR:
		case VT_RECORD:
		case VT_INT_PTR:
		case VT_UINT_PTR:
		case VT_FILETIME:
		case VT_BLOB:
		case VT_STREAM:
		case VT_STORAGE:
		case VT_STREAMED_OBJECT:
		case VT_STORED_OBJECT:
		case VT_BLOB_OBJECT:
		case VT_CF:
		case VT_CLSID:
		case VT_VERSIONED_STREAM:
		case VT_BSTR_BLOB:
		case VT_VECTOR:
		case VT_ARRAY:
		case VT_BYREF:
		case VT_RESERVED:
		case VT_ILLEGAL:
			result = INT_MIN;
			return;
		default:
			break;
		}
		d = pargs[i]->as_double();
		sum += d;
	}

	result = sum / nargs;
}

bool _check_avg(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n <= 1)
	{
		*p_msg = "'Avg' function gets more than 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// sum ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall bee_sum(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs > 1);
	if (nargs <= 1)
	{
		result = INT_MIN;
		return;
	}
	double d = pargs[0]->as_double();
	double sum = 0;

	int i;
	for (i = 0; i < nargs; i++)
	{
		switch (pargs[i]->vt)
		{
		case VT_EMPTY:
		case VT_NULL:
		case VT_CY:
		case VT_DATE:
		case VT_BSTR:
		case VT_DISPATCH:
		case VT_ERROR:
		case VT_BOOL:
		case VT_VARIANT:
		case VT_UNKNOWN:
		case VT_DECIMAL:
		case VT_VOID:
		case VT_HRESULT:
		case VT_PTR:
		case VT_SAFEARRAY:
		case VT_CARRAY:
		case VT_USERDEFINED:
		case VT_LPSTR:
		case VT_LPWSTR:
		case VT_RECORD:
		case VT_INT_PTR:
		case VT_UINT_PTR:
		case VT_FILETIME:
		case VT_BLOB:
		case VT_STREAM:
		case VT_STORAGE:
		case VT_STREAMED_OBJECT:
		case VT_STORED_OBJECT:
		case VT_BLOB_OBJECT:
		case VT_CF:
		case VT_CLSID:
		case VT_VERSIONED_STREAM:
		case VT_BSTR_BLOB:
		case VT_VECTOR:
		case VT_ARRAY:
		case VT_BYREF:
		case VT_RESERVED:
		case VT_ILLEGAL:
			result = INT_MIN;
			return;
		default:
			break;
		}
		d = pargs[i]->as_double();
		sum += d;
	}

	result = sum;
}

bool _check_sum(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n <= 1)
	{
		*p_msg = "'Sum' function gets more than 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// round ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall round(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 2);
	if (nargs != 2)
	{
		result = INT_MIN;
		return;
	}
	for (int i = 0; i < 2; i++)
	{
		switch (pargs[i]->vt)
		{
		case VT_EMPTY:
		case VT_NULL:
		case VT_CY:
		case VT_DATE:
		case VT_BSTR:
		case VT_DISPATCH:
		case VT_ERROR:
		case VT_BOOL:
		case VT_VARIANT:
		case VT_UNKNOWN:
		case VT_DECIMAL:
		case VT_VOID:
		case VT_HRESULT:
		case VT_PTR:
		case VT_SAFEARRAY:
		case VT_CARRAY:
		case VT_USERDEFINED:
		case VT_LPSTR:
		case VT_LPWSTR:
		case VT_RECORD:
		case VT_INT_PTR:
		case VT_UINT_PTR:
		case VT_FILETIME:
		case VT_BLOB:
		case VT_STREAM:
		case VT_STORAGE:
		case VT_STREAMED_OBJECT:
		case VT_STORED_OBJECT:
		case VT_BLOB_OBJECT:
		case VT_CF:
		case VT_CLSID:
		case VT_VERSIONED_STREAM:
		case VT_BSTR_BLOB:
		case VT_VECTOR:
		case VT_ARRAY:
		case VT_BYREF:
		case VT_RESERVED:
		case VT_ILLEGAL:
			result = INT_MIN;
			return;
		default:
			break;
		}
	}
	double d = pargs[0]->as_double();
	int n = pargs[1]->as_integer();
	int scale = 1;

	if (n >= 10) return;

	int i;

	for (i = 0; i < n; i++) scale *= 10;

	d = int(round(d * scale));
	d /= scale;

	result = d;
}

bool _check_round(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'Round' function gets 2 arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// InvLog ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall InvLog(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	double d = pargs[0]->as_double();
	result = exp(d);
}

bool _check_InvLog(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'InvLog' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// InvLog10 -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall InvLog10(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	double d = pargs[0]->as_double();
	result = pow((double)10, d);
}

bool _check_InvLog10(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'InvLog10' function gets 1 argument.";
		return false;
	}

	return true;
}