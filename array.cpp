#include "pch.h"
#include "array.h"
#include "c_engine.h"

void num_sort_dimension(SAFEARRAY* psa,
					c_array<int>* p_indexes,
					int n_cur,		// current dimension
					int n_dim,		// dimension sorted
					bool b_desc)	// descending
{
	unsigned int i, j, n;
	c_variable *p_base, *p_cur, *p_prev;
	unsigned int n_base;
	HRESULT		hr;
	c_variable* p_tmp = (c_variable*)alloca(sizeof(c_variable));

	if (n_cur == n_dim)
	{
		num_sort_dimension(psa, p_indexes, n_cur + 1, n_dim, b_desc);
	}
	else if (n_cur == psa->cDims)
	{
		n = psa->rgsabound[n_dim].cElements;

		for (i = 0; i < n - 1; i++)
		{
			(*p_indexes)[n_dim] = i;

			hr = SafeArrayPtrOfIndex(psa, (long*)p_indexes->get_data(), (void**)&p_base);
			_ASSERT(!FAILED(hr) && p_base);
			n_base = i;
			p_prev = p_base;

			for (j = i + 1; j < n; j++)
			{
				(*p_indexes)[n_dim] = j;
				hr = SafeArrayPtrOfIndex(psa, (long*)p_indexes->get_data(), (void**)&p_cur);
				_ASSERT(!FAILED(hr) && p_cur);

				if (b_desc)
				{
					if (p_base->as_double() < p_cur->as_double())
					{
						n_base = j;
						p_base = p_cur;
					}
				}
				else
				{
					if (p_base->as_double() > p_cur->as_double())
					{
						n_base = j;
						p_base = p_cur;
					}
				}
			}

			if (n_base != i)
			{
				memcpy(p_tmp, p_prev, sizeof(c_variable));
				memcpy(p_prev, p_base, sizeof(c_variable));
				memcpy(p_base, p_tmp, sizeof(c_variable));
			}
		}
	}
	else
	{
		n = psa->rgsabound[n_cur].cElements;

		for (i = 0; i < n; i++)
		{
			(*p_indexes)[n_cur] = i;
			num_sort_dimension(psa, p_indexes, n_cur + 1, n_dim, b_desc);
		}
	}
}

//-----------------------------------------------------------------------------
// UBound ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall UBound(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1 || nargs == 2);

	c_variable* parray = pargs[0];
	int n_dim = 0;
	if (nargs == 2) n_dim = pargs[1]->as_integer();

	// check type
	if (parray->vt != (VT_VARIANT | VT_ARRAY))
	{
		result = -1;
		return;
	}

	// check pointer
	SAFEARRAY* p_array = parray->parray;
	if (!p_array)
	{
		result = -1;
		return;
	}

	// check dimensions
	if (n_dim < 0 || p_array->cDims <= n_dim)
	{
		p_engine->runtime_error(CUR_ERR_LINE, "when executing 'Ubound'. valid dimensions are 0 ... %d. you are trying to get %d.", p_array->cDims - 1, n_dim);
		result = -1;
		return;
	}

	result = (int)(p_array->rgsabound[n_dim].cElements - 1);
}

bool _check_UBound(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1 && n != 2)
	{
		*p_msg = "'UBound' function gets 1 or 2 arguments.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// NumDims --------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall NumDims(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);

	c_variable* parray = pargs[0];

	// check type
	if (parray->vt != (VT_VARIANT | VT_ARRAY))
	{
		result = 0;
		return;
	}

	// check pointer
	SAFEARRAY* p_array = parray->parray;
	if (!p_array)
	{
		result = 0;
		return;
	}	
	result = p_array->cDims;
}

bool _check_NumDims(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'NumDims' function gets 1 argument.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// ReDim ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall ReDim(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs >= 2);

	c_variable* parray = pargs[0];
	_ASSERT(parray->vt == (VT_VARIANT | VT_ARRAY));

// get old and new dimensions -------------------------------------------------

	int n_new_dims = nargs - 1;
	int n_old_dims = 0;
	if (parray->parray)
		n_old_dims = SafeArrayGetDim(parray->parray);

// prepare new dimensions -----------------------------------------------------

	int i, d;

	c_array<SAFEARRAYBOUND> dims;
	SAFEARRAYBOUND sab;
	sab.lLbound = 0;

	//for (i = nargs - 1; i >= 1; i--)
	for (i = 1; i < nargs; i++)
	{
		sab.cElements = pargs[i]->as_integer();
		dims.add(sab);
	}

// create new safearray -------------------------------------------------------

	SAFEARRAY* p_array = SafeArrayCreateEx(	VT_VARIANT,
											dims.get_size(),
											dims.get_data(),
											0);

	if (!p_array)
	{
		SafeArrayDestroy(parray->parray);
		parray->parray =0;
		result = 0;
		return;
	}

// if old was empty, just attach new ------------------------------------------

	if (!parray->parray)
	{
		parray->parray = p_array;
		result = 1;
		return;
	}

// if dimension count is different, don't preserve old data -------------------

	if (n_new_dims != n_old_dims)
	{
		SafeArrayDestroy(parray->parray);
		parray->parray = p_array;
		result = 1;
		return;
	}

// copy data from old array to new array --------------------------------------

	HRESULT hr;
	c_variable* p_old_data = 0;
	c_variable* p_new_data = 0;

	hr = SafeArrayLock(parray->parray);
	_ASSERT(!FAILED(hr));
	hr = SafeArrayLock(p_array);
	_ASSERT(!FAILED(hr));

	hr = SafeArrayAccessData(parray->parray, (void**)&p_old_data);
	_ASSERT(!FAILED(hr) && p_old_data);
	hr = SafeArrayAccessData(p_array, (void**)&p_new_data);
	_ASSERT(!FAILED(hr) && p_new_data);

	for (d = 0; d < __min(n_old_dims, n_new_dims); d++)
	{
		int n1 = parray->parray->rgsabound[d].cElements;
		int n2 = p_array->rgsabound[d].cElements;

		for (i = 0; i < __min(n1, n2); i++)
		{
			p_new_data[d * i] = p_old_data[d * i];
		}
	}

	SafeArrayUnaccessData(p_array);
	SafeArrayUnaccessData(parray->parray);

	SafeArrayUnlock(p_array);
	SafeArrayUnlock(parray->parray);

// free old and attach new ----------------------------------------------------

	SafeArrayDestroy(parray->parray);
	parray->parray = p_array;

	result = 1;
}

bool _check_ReDim(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n < 2)
	{
		*p_msg = "'ReDim' function gets 2 or more arguments.";
		return false;
	}

	if (p_types[0] != (VT_VARIANT | VT_ARRAY))
	{
		*p_msg = "first argument for 'ReDim' function must be a variable declared as Array.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// NumSort --------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall NumSort(int nargs, c_variable** pargs, c_engine* p_engine)
{
	_ASSERT(nargs >= 1 && nargs <= 3);
	_ASSERT(pargs[0]->vt == (VT_VARIANT | VT_ARRAY));

// prepare and check ----------------------------------------------------------

	if (!pargs[0]->parray) return;  // no array attached
	SAFEARRAY* p_array = pargs[0]->parray;

	bool b_desc = (nargs == 1 || (nargs >= 2 && pargs[1]->as_integer()));
	int  n_dim  = (nargs == 3 ? pargs[2]->as_integer() : 0);

	if (n_dim >= p_array->cDims) return; // out of dimensions

// do sort --------------------------------------------------------------------

	int n_step = 1;
	c_array<int> indexes;

	c_variable* p_data = 0;
	HRESULT hr = SafeArrayLock(p_array);
	_ASSERT(!FAILED(hr));

	indexes.alloc(p_array->cDims);

	num_sort_dimension(p_array, &indexes, 0, n_dim, b_desc);

	SafeArrayUnlock(p_array);
}

bool _check_NumSort(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n < 1 || n > 3)
	{
		*p_msg = "'NumSort' function gets 1 to 3 arguments";
		return false;
	}

	if (p_types[0] != (VT_VARIANT | VT_ARRAY))
	{
		*p_msg = "first argument for 'NumSort' function must be a variable declared as Array.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// AlphaSort ------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall AlphaSort(int nargs, c_variable** pargs, c_engine* p_engine)
{
}

bool _check_AlphaSort(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n < 1 || n > 3)
	{
		*p_msg = "'NumSort' function gets 1 to 3 arguments";
		return false;
	}

	if (p_types[0] != (VT_VARIANT | VT_ARRAY))
	{
		*p_msg = "first argument for 'NumSort' function must be a variable declared as Array.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Erase ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall Erase(int nargs, c_variable** pargs, c_engine* p_engine)
{
	_ASSERT(nargs >= 1);

	int i;
	for (i = 0; i < nargs; i++)
	{
		c_variable* p_var = pargs[i];
		if (p_var->vt == (VT_VARIANT | VT_BYREF))
			p_var = (c_variable*)p_var->pvarVal;

		if (!p_var) continue;

		switch (p_var->vt)
		{
		case VT_VARARRAY:
			if (p_var->parray)
				SafeArrayDestroy(p_var->parray);
			p_var->parray = 0;
			break;

		case VT_BSTR:
			if (p_var->bstrVal) SysFreeString(p_var->bstrVal);
			p_var->bstrVal = 0;
			break;

		case VT_VARIANT:
			p_var->reset();
			break;

		case VT_DISPATCH:
			if (p_var->pdispVal) p_var->pdispVal->Release();
			p_var->pdispVal = 0;
			break;

		case VT_CY:
			p_var->cyVal.Hi = 0;
			p_var->cyVal.Lo = 0;
			break;

		case VT_UINT:
		case VT_UI4:
		case VT_UI2:
		case VT_I2:			p_var->iVal = 0;			break;

		case VT_INT:
		case VT_I4:			p_var->lVal = 0;			break;

		case VT_BOOL:		p_var->boolVal = 0;			break;

		case VT_R4:			p_var->fltVal = 0;			break;
		case VT_R8:			p_var->dblVal = 0;			break;
		case VT_DATE:		p_var->date = 0;			break;

		case VT_I1:
		case VT_UI1:		p_var->cVal = 0;			break;
		}
	}
}

bool _check_Erase(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n < 1)
	{
		*p_msg = "'Erase' function gets at least one argument";
		return false;
	}

	return true;
}


void __stdcall ArrCount(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	if (pargs[0]->vt != (VT_VARIANT | VT_ARRAY))
	{
		result = INT_MIN;
		return;
	}
	//_ASSERT(pargs[0]->vt == (VT_VARIANT | VT_ARRAY));
	c_variable* parray = pargs[0];
	// check type
	if (parray->vt != (VT_VARIANT | VT_ARRAY))
	{
		result = INT_MIN;
		return;
	}	
	// check pointer
	SAFEARRAY* p_array = parray->parray;
	if (!p_array)
	{
		result = 0;
		return;
	}	 
	LONG LB = 0;
	LONG UB = 0;
	SafeArrayGetLBound(p_array, 1, &LB);
	SafeArrayGetUBound(p_array, 1, &UB);
	result = int(UB - LB + 1);
}

bool _check_ArrCount(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'ArrCount' function gets at one argument";
		return false;
	}
	if (p_types[0] != (VT_VARIANT | VT_ARRAY))
	{
		*p_msg = "first argument for 'ArrCount' function must be a variable declared as Array.";
		return false;
	}
	return true;
}
