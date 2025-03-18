#include "pch.h"

#include <crtdbg.h>

#include <atlbase.h>
#include "c_engine.h"

//-----------------------------------------------------------------------------
// is_valid_safearray ---------------------------------------------------------
//-----------------------------------------------------------------------------

bool is_valid_safearray(SAFEARRAY* psa)
{
	if (IsBadWritePtr(psa, sizeof(SAFEARRAY))) return false;

	if (psa->cbElements <= 0 || psa->cbElements > sizeof(VARIANT))
	{
		return false;
	}

	if (IsBadWritePtr(psa->rgsabound, sizeof(SAFEARRAYBOUND) * psa->cDims))
	{
		return false;
	}

	int n_items = 0;
	int n_dim;
	for (n_dim = 0; n_dim < psa->cDims; n_dim ++)
		n_items += psa->rgsabound[n_dim].cElements;

	if (n_items)
	{
		if (IsBadWritePtr(psa->pvData, n_items * psa->cbElements)) return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// compatible_arrays ----------------------------------------------------------
//-----------------------------------------------------------------------------

bool compatible_arrays(SAFEARRAY* psa1, SAFEARRAY* psa2)
{
	if (!psa1 || !psa2) return false;
	if (psa1->cDims != psa2->cDims) return false;

	int i;
	for(i = 0; i < psa1->cDims; i++)
	{
		if (psa1->rgsabound[i].lLbound != psa2->rgsabound[i].lLbound) return false;
		if (psa1->rgsabound[i].cElements != psa2->rgsabound[i].cElements) return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// dim_add --------------------------------------------------------------------
//-----------------------------------------------------------------------------

void dim_add(SAFEARRAY* psa1, SAFEARRAY* psa2, SAFEARRAY* psa3,
			 c_array<int> *p_dims, int n_dim, arr_operator action)
{
	unsigned int i;
	HRESULT hr;
	c_variable* p_val1 = 0;
	c_variable* p_val2 = 0;
	c_variable* p_val3 = 0;

	for (i = 0; i < psa1->rgsabound[n_dim].cElements; i++)
	{
		(*p_dims)[n_dim] = i;
		if (n_dim == psa1->cDims - 1)
		{
			hr = SafeArrayPtrOfIndex(psa1, (long*)p_dims->get_data(), (void**)&p_val1);
			_ASSERT(!FAILED(hr));
			hr = SafeArrayPtrOfIndex(psa2, (long*)p_dims->get_data(), (void**)&p_val2);
			_ASSERT(!FAILED(hr));
			hr = SafeArrayPtrOfIndex(psa3, (long*)p_dims->get_data(), (void**)&p_val3);
			_ASSERT(!FAILED(hr));

			switch (action)
			{
			case _arr_add:
				c_variable::var_add(p_val1, p_val2, p_val3);
				break;

			case _arr_sub:
				c_variable::var_sub(p_val1, p_val2, p_val3);
				break;

			case _arr_mul:
				c_variable::var_mul(p_val1, p_val2, p_val3);
				break;

			case _arr_div:
				c_variable::var_div(p_val1, p_val2, p_val3);
				break;

			case _arr_mod:
				c_variable::var_mod(p_val1, p_val2, p_val3);
				break;

			case _arr_and:
				c_variable::var_and(p_val1, p_val2, p_val3);
				break;

			case _arr_or:
				c_variable::var_or(p_val1, p_val2, p_val3);
				break;

			case _arr_xor:
				c_variable::var_xor(p_val1, p_val2, p_val3);
				break;

			case _arr_cat:
				c_variable::var_cat(p_val1, p_val2, p_val3);
				break;

			default: _ASSERT(0);

			}
		}
		else dim_add(psa1, psa2, psa3, p_dims, n_dim + 1, action);
	}
}

//-----------------------------------------------------------------------------
// arr_add --------------------------------------------------------------------
//-----------------------------------------------------------------------------

void arr_add(c_variable* p_left, c_variable* p_right, c_variable* p_result, arr_operator action)
{
	SAFEARRAY* psa1 = p_left->parray;
	SAFEARRAY* psa2 = p_right->parray;
	int i;

	_ASSERT(compatible_arrays(psa1, psa2));

// create destination array ---------------------------------------------------

// MICHAEL: this is very important. cannot call "reset()", because
// it will free c_var_data. it may not be freed, because we are between
// PUSH_RESERVED and POP_RESERVED in caller functions.
	//p_result->reset();
	VariantClear(p_result);
// END MICHAEL

	c_array<SAFEARRAYBOUND> bounds;
	SAFEARRAYBOUND bound;
	bound.lLbound = 0;

	for(i = 0; i < psa1->cDims; i++)
	{
		bound.cElements = psa1->rgsabound[i].cElements;
		bounds.add(bound);
	}

	p_result->parray = SafeArrayCreate(VT_VARIANT, bounds.get_size(), bounds.get_data());

	if (!p_result->parray)
	{
		_ASSERT(0);
		return;
	}

// do addition on all the dimensions ------------------------------------------

	c_array<int> dims;
	dims.alloc(psa1->cDims);
	for(i = 0; i < psa1->cDims; i++) dims[i] = 0;

	HRESULT hr = SafeArrayLock(psa1);
	_ASSERT(!FAILED(hr));
	hr = SafeArrayLock(psa2);
	_ASSERT(!FAILED(hr));
	hr = SafeArrayLock(p_result->parray);
	_ASSERT(!FAILED(hr));

	dim_add(psa1, psa2, p_result->parray, &dims, 0, action);

	SafeArrayUnlock(p_result->parray);
	SafeArrayUnlock(psa2);
	SafeArrayUnlock(psa1);

	p_result->vt = VT_VARARRAY;
}

//-----------------------------------------------------------------------------
// set_element ----------------------------------------------------------------
//-----------------------------------------------------------------------------

void set_element(SAFEARRAY* p_array, long n, c_variable* p_var)
{
	HRESULT hr;

	if (p_array && p_array->cbElements == sizeof(VARIANT))
	{
		if (n == 0)
		{
			c_variable *p_element = (c_variable*)p_array->pvData;
			*p_element = *p_var;
		}
		else if (p_array->cDims == 1)
		{
			VARIANT* p_data = 0;
			hr = SafeArrayLock(p_array);

			if (!FAILED(hr))
			{
				hr = SafeArrayPtrOfIndex(p_array, &n, (void**)&p_data);
				if (!FAILED(hr) && p_data) VariantCopy(p_data, p_var);

				SafeArrayUnlock(p_array);
			}

		}
		else _ASSERT(0);
	}
}

//-----------------------------------------------------------------------------
// get_element ----------------------------------------------------------------
//-----------------------------------------------------------------------------

void get_element(SAFEARRAY* p_array, long n, c_variable** p_var)
{
	HRESULT hr;
	*p_var = 0;

	if (p_array && p_array->cbElements == sizeof(VARIANT))
	{
		if (n == 0)
		{
			*p_var = (c_variable*)p_array->pvData;
		}
		else if (p_array->cDims == 1)
		{
			hr = SafeArrayLock(p_array);

			if (!FAILED(hr))
			{
				hr = SafeArrayPtrOfIndex(p_array, &n, (void**)p_var);
				_ASSERT(!FAILED(hr));
				SafeArrayUnlock(p_array);
			}
		}
		else _ASSERT(0);
	}
}

//-----------------------------------------------------------------------------
// arithmetics ----------------------------------------------------------------
//-----------------------------------------------------------------------------

HRESULT c_variable::var_add(c_variable* p_left, c_variable* p_right, c_variable* p_result)
{
	HRESULT hr	= 0;
	PUSH_RESERVED(p_result);

	if ( (p_left->vt == VT_VARARRAY && p_right->vt == VT_VARARRAY) &&
			compatible_arrays(p_left->parray, p_right->parray) )
	{
		arr_add(p_left, p_right, p_result, _arr_add);
	}
	else if (p_left->vt == VT_BSTR || p_right->vt == VT_BSTR)
		hr = VarCat(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());
	else
	{
		if (p_result->is_strict())
		{
			c_variable tmp;
			hr = VarAdd(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), &tmp);
			*p_result = tmp;
		}
		else
			hr = VarAdd(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());
	}

	POP_RESERVED(p_result);
#ifdef _DEBUG
	p_result->assert_valid();
#endif

	return hr;
}

HRESULT c_variable::var_cat(c_variable* p_left, c_variable* p_right, c_variable* p_result)
{
	HRESULT hr = 0;
	PUSH_RESERVED(p_result);

	if ( (p_left->vt == VT_VARARRAY && p_right->vt == VT_VARARRAY) &&
			compatible_arrays(p_left->parray, p_right->parray) )
	{
		arr_add(p_left, p_right, p_result, _arr_cat);
	}
	else
	{
		if (p_result->is_strict())
		{
			c_variable tmp;
			hr = VarCat(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), &tmp);
			*p_result = tmp;
		}
		else
			hr = VarCat(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());
	}

	POP_RESERVED(p_result);
#ifdef _DEBUG
	p_result->assert_valid();
#endif

	return hr;
}

HRESULT c_variable::var_sub(c_variable* p_left, c_variable* p_right, c_variable* p_result)
{
	HRESULT hr = 0;
	PUSH_RESERVED(p_result);

	if ( (p_left->vt == VT_VARARRAY && p_right->vt == VT_VARARRAY) &&
			compatible_arrays(p_left->parray, p_right->parray) )
	{
		arr_add(p_left, p_right, p_result, _arr_sub);
	}
	else
	{
		if (p_result->is_strict())
		{
			c_variable tmp;
			hr = VarSub(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), &tmp);
			*p_result = tmp;
		}
		else
			hr = VarSub(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());
	}

	POP_RESERVED(p_result);
#ifdef _DEBUG
	p_result->assert_valid();
#endif

	return hr;
}

HRESULT c_variable::var_mul(c_variable* p_left, c_variable* p_right, c_variable* p_result)
{
	HRESULT hr = 0;
#ifdef _DEBUG
	p_result->assert_valid();
#endif

	PUSH_RESERVED(p_result);

	if ( (p_left->vt == VT_VARARRAY && p_right->vt == VT_VARARRAY) &&
			compatible_arrays(p_left->parray, p_right->parray) )
	{
		arr_add(p_left, p_right, p_result, _arr_mul);
	}
	else
	{
		if (p_result->is_strict())
		{
			c_variable tmp;
			hr = VarMul(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), &tmp);
			*p_result = tmp;
		}
		else
			hr = VarMul(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());
	}

	POP_RESERVED(p_result);
#ifdef _DEBUG
	p_result->assert_valid();
#endif

	return hr;
}

HRESULT c_variable::var_div(c_variable* p_left, c_variable* p_right, c_variable* p_result)
{
	HRESULT hr = 0;
	PUSH_RESERVED(p_result);

	if ( (p_left->vt == VT_VARARRAY && p_right->vt == VT_VARARRAY) &&
			compatible_arrays(p_left->parray, p_right->parray) )
	{
		arr_add(p_left, p_right, p_result, _arr_div);
	}
	else
	{
		if (p_result->is_strict())
		{
			c_variable tmp;
			hr = VarDiv(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), &tmp);
			*p_result = tmp;
		}
		else
			hr = VarDiv(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());
	}

	POP_RESERVED(p_result);
#ifdef _DEBUG
	p_result->assert_valid();
#endif

	return hr;
}

HRESULT c_variable::var_mod(c_variable* p_left, c_variable* p_right, c_variable* p_result)
{
	HRESULT hr = 0;
	PUSH_RESERVED(p_result);

	if ( (p_left->vt == VT_VARARRAY && p_right->vt == VT_VARARRAY) &&
			compatible_arrays(p_left->parray, p_right->parray) )
	{
		arr_add(p_left, p_right, p_result, _arr_mod);
	}
	else
	{
		if (p_result->is_strict())
		{
			c_variable tmp;
			hr = VarMod(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), &tmp);
			*p_result = tmp;
		}
		else
			hr = VarMod(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());
	}

	POP_RESERVED(p_result);
#ifdef _DEBUG
	p_result->assert_valid();
#endif

	return hr;
}

HRESULT c_variable::var_and(c_variable* p_left, c_variable* p_right, c_variable* p_result)
{
	HRESULT hr = 0;
	PUSH_RESERVED(p_result);

	if ( (p_left->vt == VT_VARARRAY && p_right->vt == VT_VARARRAY) &&
			compatible_arrays(p_left->parray, p_right->parray) )
	{
		arr_add(p_left, p_right, p_result, _arr_and);
	}
	else
	{
		if (p_result->is_strict())
		{
			c_variable tmp;
			hr = VarAnd(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), &tmp);
			*p_result = tmp;
		}
		else
			hr = VarAnd(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());
	}

	POP_RESERVED(p_result);
#ifdef _DEBUG
	p_result->assert_valid();
#endif

	return hr;
}

HRESULT c_variable::var_or (c_variable* p_left, c_variable* p_right, c_variable* p_result)
{
	HRESULT hr = 0;
	PUSH_RESERVED(p_result);

	if ( (p_left->vt == VT_VARARRAY && p_right->vt == VT_VARARRAY) &&
			compatible_arrays(p_left->parray, p_right->parray) )
	{
		arr_add(p_left, p_right, p_result, _arr_or);
	}
	else
	{
		if (p_result->is_strict())
		{
			c_variable tmp;
			hr = VarOr(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), &tmp);
			*p_result = tmp;
		}
		else
			hr = VarOr(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());
	}

	POP_RESERVED(p_result);
#ifdef _DEBUG
	p_result->assert_valid();
#endif

	return hr;
}

HRESULT c_variable::var_xor(c_variable* p_left, c_variable* p_right, c_variable* p_result)
{
	HRESULT hr = 0;
	PUSH_RESERVED(p_result);

	if ( (p_left->vt == VT_VARARRAY && p_right->vt == VT_VARARRAY) &&
			compatible_arrays(p_left->parray, p_right->parray) )
	{
		arr_add(p_left, p_right, p_result, _arr_xor);
	}
	else
	{
		if (p_result->is_strict())
		{
			c_variable tmp;
			hr = VarXor(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), &tmp);
			*p_result = tmp;
		}
		else
			hr = VarXor(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());
	}

	POP_RESERVED(p_result);
#ifdef _DEBUG
	p_result->assert_valid();
#endif

	return hr;
}

HRESULT c_variable::var_pow(c_variable* p_left, c_variable* p_right, c_variable* p_result)
{
	PUSH_RESERVED(p_result);
	HRESULT hr = 0;

	if (p_result->is_strict())
	{
		c_variable tmp;
		hr = VarPow(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), &tmp);
		*p_result = tmp;
	}
	else
		hr = VarPow(p_left->get_scalar_ptr(), p_right->get_scalar_ptr(), p_result->get_scalar_ptr());

	POP_RESERVED(p_result);

#ifdef _DEBUG
	p_result->assert_valid();
#endif

	return hr;
}

//-----------------------------------------------------------------------------
// constructors ---------------------------------------------------------------
//-----------------------------------------------------------------------------

c_variable::c_variable()
{
	VariantInit(this);
	pdispVal = 0;
	wReserved1 = 0;
	wReserved2 = 0;
	wReserved3 = 0;
	dblVal = 0;
	pVarData = 0;
}

c_variable::c_variable(c_engine* p_engine)
{
	VariantInit(this);
	pdispVal = 0;
	wReserved1 = 0;
	wReserved2 = 0;
	wReserved3 = 0;
	dblVal = 0;
	pVarData = 0;

	set_engine(p_engine);
}

c_variable::c_variable(const int i)
{
	VariantInit(this);
	wReserved1 = 0;
	wReserved2 = 0;
	wReserved3 = 0;
	pVarData = 0;

	vt		= VT_I4;
	intVal	= i;
}

c_variable::c_variable(const double d)
{
	VariantInit(this);
	wReserved1 = 0;
	wReserved2 = 0;
	wReserved3 = 0;
	pVarData = 0;

	vt		= VT_R8;
	dblVal	= d;
}

c_variable::c_variable(LPCTSTR lpsz)
{
	VariantInit(this);
	wReserved1 = 0;
	wReserved2 = 0;
	wReserved3 = 0;
	pVarData = 0;

	vt		= VT_BSTR;
	USES_CONVERSION;
	bstrVal = CA2W(lpsz);

//	int n = strlen(lpsz);
//	if (!n) return;
//	bstrVal	= SysAllocStringLen(0, n + 1);
//	mbstowcs(bstrVal, lpsz, n + 1);
}

c_variable::c_variable(BSTR bstr)
{
	VariantInit(this);
	wReserved1 = 0;
	wReserved2 = 0;
	wReserved3 = 0;
	pVarData = 0;

	vt		= VT_BSTR;
	bstrVal	= SysAllocString(bstr);
}

c_variable::c_variable(c_variable& v)
{
	VariantInit(this);
	wReserved1 = 0;
	wReserved2 = 0;
	wReserved3 = 0;
	pVarData = 0;

	*this = v;
}

c_variable::~c_variable()
{
	reset();
}

void c_variable::store(c_var_storage* p_storage)
{
	c_variable* ptr = get_ptr();
	if (this != ptr)
	{
		ptr->store(p_storage);
		return;
	}

	p_storage->m_r1 = wReserved1;
	p_storage->m_r2 = wReserved2;
	p_storage->m_r3 = wReserved3;

	p_storage->m_pengine = get_engine();

	if (vt == VT_VARARRAY)
	{
		p_storage->m_psa_old = parray;
		_ASSERT(!p_storage->m_psa);
		if (parray)
		{
			HRESULT hr = SafeArrayCopy(parray, &p_storage->m_psa);
			_ASSERT(!FAILED(hr) && p_storage->m_psa);
		}
	}
}

void c_variable::restore(c_var_storage* p_storage)
{
	HRESULT hr = 0;
	c_variable* ptr = get_ptr();
	if (this != ptr)
	{
		ptr->restore(p_storage);
		return;
	}

	if (p_storage->m_psa)
	{
		if (vt != VT_VARARRAY)
		{
			wReserved1 = 0;
			wReserved2 = 0;
			wReserved3 = 0;
			set_element(p_storage->m_psa, 0, this);
			reset();

			vt = VT_VARARRAY;
			parray = p_storage->m_psa;

			if (p_storage->m_psa_old)
				if (is_valid_safearray(p_storage->m_psa_old))
				{
					hr = SafeArrayDestroy(p_storage->m_psa_old);
					_ASSERT(!FAILED(hr));
				}
		}
		else
		{
			if (p_storage->m_psa != parray)
			{
				hr = SafeArrayDestroy(p_storage->m_psa);
				_ASSERT(!FAILED(hr));
			}

// MICHAEL 2006.03.28
			if (p_storage->m_psa_old != parray)
				if (is_valid_safearray(p_storage->m_psa_old))
				{
					hr = SafeArrayDestroy(p_storage->m_psa_old);
					_ASSERT(!FAILED(hr));
				}
// END MICHAEL

			p_storage->m_psa = 0;
		}
	}

	wReserved1 = p_storage->m_r1;
	wReserved2 = p_storage->m_r2;
	wReserved3 = p_storage->m_r3;

#ifdef _DEBUG
	assert_valid();
#endif
}

void c_variable::ref(c_variable* p_var)
{
#ifdef _DEBUG
	assert_valid();
#endif

	if (p_var->vt & VT_BYREF)
	{
		c_var_data* p_var_data = get_var_data();
		HRESULT hr = VariantCopy(this, p_var);
		set_var_data(p_var_data);

		if (FAILED(hr))
		{
			c_engine* p_engine = get_engine();
			if (p_engine) p_engine->runtime_error(hr);
		}
	}
	else
	{
		_ASSERT(this != p_var);
		reset();
		vt = VT_BYREF | VT_VARIANT;
		byref = p_var;
	}
}

void c_variable::from_input(LPCTSTR str)
{
	_ASSERT(str);
	LPCTSTR pstr = str;
	c_variable temp_var;

	if (strlen(str) == 0)
	{
		temp_var.vt = VT_BSTR;
	}
	else
	{
		while(*pstr)
		{
			if (*pstr == ' ') goto _loop_end;

			if (isalpha(*pstr))
			{
				temp_var.vt = VT_BSTR;
				break;
			}

			if (*pstr == '.' && temp_var.vt == VT_I4)
				temp_var.vt = VT_R8;

_loop_end:
			pstr++;
		}
	}

	switch (temp_var.vt)
	{

	case VT_R8:
		temp_var = atof(str);
		break;

	case VT_BSTR:
		temp_var = str; // this will convert PCTSTR->BSTR
		break;

	case VT_I4:
	default:
		temp_var = atoi(str);
		break;
	}

	(*this) = temp_var;
}

void c_variable::create_object()
{
	_ASSERT(vt == VT_DISPATCH);
	_ASSERT(!pdispVal);

	c_var_data* p_data = get_var_data();
	_ASSERT(p_data);

	IDispatch* pd = 0;

	HRESULT hr = co_create_instance(p_data->m_clsid, &pd);
	if (FAILED(hr) || !pd)
	{
		p_data->m_pengine->runtime_error(hr);
		return;
	}

	pdispVal = pd;
	pd->AddRef();
}

// operator = -----------------------------------------------------------------

void c_variable::operator = (const int val)
{
#ifdef _DEBUG
	assert_valid();
#endif

	c_variable* p_val = get_scalar_ptr();
	if (p_val != this)
	{
		*p_val = val;
		return;
	}

	_ASSERT(this == get_ptr()); // must be plain here
	if (is_strict())
	{
		switch (vt)
		{
		case VT_I2: iVal = val;		break;
		case VT_I4:
		case VT_UINT:
		case VT_INT: lVal = val;	break;
		case VT_BOOL:
			boolVal = (val != 0 ? -1 : 0);break;
		case VT_R4: fltVal = (float)val;	break;
		case VT_R8: dblVal = (double) val;	break;
		case VT_BSTR:
			{
				if (bstrVal) SysFreeString(bstrVal);
				bstrVal = 0;

				wchar_t buffer[32];
				_itow(val, buffer, 10);
				bstrVal = SysAllocString(buffer);
			}
			break;

		default: _ASSERT(0);
		}
	}
	else
	{
		VariantClear(this);
		vt = VT_I4;
		intVal = val;
	}

#ifdef _DEBUG
	assert_valid();
#endif

}

void c_variable::operator = (bool val)
{
#ifdef _DEBUG
	assert_valid();
#endif

	c_variable* p_val = get_scalar_ptr();
	if (p_val != this)
	{
		*p_val = val;
		return;
	}

	_ASSERT(this == get_ptr()); // must be plain here
	if (is_strict())
	{
		switch (vt)
		{
		case VT_I2: iVal = (val ? -1 : 0);		break;
		case VT_I4:
		case VT_UINT:
		case VT_INT: lVal = (val ? -1 : 0);		break;
		case VT_BOOL: boolVal = (val ? -1 : 0);	break;
		case VT_R4: fltVal = (float)(val ? -1 : 0);	break;
		case VT_R8: dblVal = (double)(val ? -1 : 0);	break;
		case VT_BSTR:
			{
				if (bstrVal) SysFreeString(bstrVal);
				bstrVal = 0;

				if (val)
					bstrVal = SysAllocString(L"True");
				else
					bstrVal = SysAllocString(L"False");
			}
			break;

		default: _ASSERT(0);
		}
	}
	else
	{
		VariantClear(this);
		vt = VT_BOOL;
		boolVal = (val != 0) ? -1 : 0;
	}

#ifdef _DEBUG
	assert_valid();
#endif
}

void c_variable::operator = (const double val)
{
#ifdef _DEBUG
	assert_valid();
#endif

	c_variable* p_val = get_scalar_ptr();
	if (p_val != this)
	{
		*p_val = val;
		return;
	}

	_ASSERT(this == get_ptr()); // must be plain here
	if (is_strict())
	{
		switch (vt)
		{
		case VT_I2: iVal = (short)val;		break;
		case VT_I4:
		case VT_UINT:
		case VT_INT: lVal = (long)val;	break;
		case VT_BOOL:
			boolVal = (val != 0 ? -1 : 0);break;
		case VT_R4: fltVal = (float)val;	break;
		case VT_R8: dblVal = val;	break;
		case VT_BSTR:
			{
				if (bstrVal) SysFreeString(bstrVal);
				bstrVal = 0;

				HRESULT hr = VarBstrFromR8(val, 0, 0, &bstrVal);
				_ASSERT(!FAILED(hr));
			}
			break;

		default: _ASSERT(0);
		}
	}
	else
	{
		VariantClear(this);
		vt = VT_R8;
		dblVal = val;
	}

#ifdef _DEBUG
	assert_valid();
#endif
}

void c_variable::operator = (LPCTSTR val)
{
#ifdef _DEBUG
	assert_valid();
#endif

	c_variable* p_val = get_scalar_ptr();
	if (p_val != this)
	{
		*p_val = val;
		return;
	}

	_ASSERT(this == get_ptr()); // must be plain here
	if (is_strict())
	{
		switch (vt)
		{
		case VT_I2: iVal = atoi(val);		break;
		case VT_I4:
		case VT_UINT:
		case VT_INT: lVal = atoi(val);	break;
		case VT_BOOL:
			boolVal = (atoi(val) != 0 ? -1 : 0);break;
		case VT_R4: fltVal = (float)atof(val);	break;
		case VT_R8: dblVal = atof(val);	break;
		case VT_BSTR:
			{
				if (bstrVal) SysFreeString(bstrVal);
				bstrVal = 0;

				USES_CONVERSION;
				bstrVal = SysAllocString(CA2W(val));
			}
			break;

		default: _ASSERT(0);
		}
	}
	else
	{
		VariantClear(this);
		vt = VT_BSTR;
		if (!val) return;
		USES_CONVERSION;
		bstrVal = SysAllocString(CA2W(val));
	}

#ifdef _DEBUG
	assert_valid();
#endif
}

void c_variable::operator = (BSTR val)
{
#ifdef _DEBUG
	assert_valid();
#endif

	c_variable* p_val = get_scalar_ptr();
	if (p_val != this)
	{
		*p_val = val;
		return;
	}

	_ASSERT(this == get_ptr()); // must be plain here
	if (is_strict())
	{
		HRESULT hr;

		switch (vt)
		{
		case VT_I2:		hr = VarI2FromStr(val, 0, 0, &iVal); break;
		case VT_I4:
		case VT_UINT:
		case VT_INT:	hr = VarI4FromStr(val, 0, 0, &lVal);  break;
		case VT_BOOL:	hr = VarBoolFromStr(val, 0, 0, &boolVal); break;
		case VT_R4:		hr = VarR4FromStr(val, 0, 0, &fltVal); break;
		case VT_R8:		hr = VarR8FromStr(val, 0, 0, &dblVal); 	break;
		case VT_BSTR:
			{
				if (bstrVal) SysFreeString(bstrVal);
				bstrVal = SysAllocString(val);
			}
			break;

		default: _ASSERT(0);
		}
	}
	else
	{
		VariantClear(this);
		vt = VT_R8;
		bstrVal	= SysAllocString(val);
	}

#ifdef _DEBUG
	assert_valid();
#endif
}

void c_variable::operator = (SAFEARRAY* p_array)
{
#ifdef _DEBUG
	assert_valid();
#endif

	c_var_data* p_old = get_var_data();

	c_variable* p_val = get_ptr();
	if (p_val != this)
	{
		*p_val = p_array;
		return;
	}

	_ASSERT(this == get_ptr()); // must be plain here
	if (vt == VT_VARARRAY)
	{
		if (parray) SafeArrayDestroy(parray);
		parray = 0;
		SafeArrayCopy(p_array, &parray);
	}
	else
	{
		if (is_strict())
		{
			c_variable* p_element = 0;
			get_element(p_array, 0, &p_element);
			cast(p_element);
		}
		else
		{
			//reset();
			VariantClear(this);
			vt = VT_VARARRAY;
			SafeArrayCopy(p_array, &parray);
		}

//		c_variable* p_element = 0;
//		get_element(p_array, 0, &p_element);
//		if (p_element)
//		{
//			if (is_strict()) cast(p_element);
//			else assign(p_element);
//		}
	}

	set_var_data(p_old);

#ifdef _DEBUG
	assert_valid();
#endif
}

void c_variable::operator = (LPDISPATCH p_disp)
{
#ifdef _DEBUG
	assert_valid();
#endif

	c_variable* p_val = get_scalar_ptr();
	if (p_val != this)
	{
		*p_val = p_disp;
		return;
	}

	_ASSERT(this == get_ptr()); // must be plain here
	if (is_strict())
	{
		if (vt != VT_DISPATCH)
		{
			c_engine* p_engine = get_engine();
			if (p_engine) p_engine->runtime_error(CUR_ERR_LINE, "trying to assign value to object variable");
		}
		else
		{
			bool b_ok = true;
			c_var_data* p_var_data = get_var_data();

			if (p_var_data && !is_NULL_GUID(&p_var_data->m_clsid))
			{
				if (!is_matching_class(&p_var_data->m_iid, p_disp))
				{
					c_engine* p_engine = get_engine();
					_ASSERT(p_engine); // must be set here for sure

					p_engine->runtime_error(CUR_ERR_LINE, "type missmatch: assigning object of wrong class.");
					b_ok = false;
				}
			}

			if (b_ok)
			{
				if (pdispVal) pdispVal->Release();
				pdispVal = p_disp;
				if (p_disp) p_disp->AddRef();
			}
		}
	}
	else
	{
		VariantClear(this);
		vt = VT_DISPATCH;
		pdispVal = p_disp;
		if (p_disp) p_disp->AddRef();
	}

#ifdef _DEBUG
	assert_valid();
#endif
}

void c_variable::cast(c_variable* p_val)
{
	c_engine* p_engine = get_engine();
	_ASSERT(p_engine);
	_ASSERT(p_val->get_ptr() == p_val);// expect resolved
	c_variable* p_target = get_ptr();

// check interface type

	if (p_target->vt == VT_DISPATCH && p_val->vt == VT_DISPATCH && p_val->pdispVal)
	{
		c_var_data* p_var_data = get_var_data();

		if (p_var_data)
		{
			if (!is_matching_class(&p_var_data->m_iid, p_val->pdispVal))
			{
				c_engine* p_engine = get_engine();
				if (p_engine)
				{
					p_engine->runtime_error(CUR_ERR_LINE, "type missmatch: assigning object of wrong class.");
					return;
				}
				else _ASSERT(0); // must be
			}
		}
		else _ASSERT(0); // must be
	}

//

	if (p_val->vt == VT_VARARRAY && vt == VT_VARARRAY)
	{
		// both variables are arrays. copy one to another.
		if (parray) _ASSERT(is_valid_safearray(parray));
		_ASSERT(is_valid_safearray(p_val->parray));

		SafeArrayDestroy(parray);
		parray = 0;
		SafeArrayCopy(p_val->parray, &parray);
	}
	else if (vt == VT_VARARRAY)
	{
		// assigning scalar to array. set first element of the array.
		set_element(parray, 0, p_val);
	}
	else if (p_val->vt == VT_VARARRAY)
	{
		// assigning array to scalar. get first element of the array.

		c_variable* p_element = 0;
		get_element(p_val->parray, 0, &p_element);
		if (p_element)
		{
			HRESULT hr = VariantChangeType(	p_target,
											p_element,
											VARIANT_NOVALUEPROP | VARIANT_ALPHABOOL,
											p_target->vt);

			p_engine->runtime_error(hr);
		}
	}
	else
	{
		HRESULT hr = VariantChangeType(	p_target,
										p_val,
										VARIANT_NOVALUEPROP | VARIANT_ALPHABOOL,
										p_target->vt);
		p_engine->runtime_error(hr);
	}
}

void c_variable::assign(c_variable* p_val)
{
	VARIANT *pSrc, *pDst;

	if (vt == (VT_VARIANT | VT_BYREF))
	{
		pDst = get_ptr();
		pSrc = p_val->get_ptr();
	}
	else
	{
		pDst = this;
		pSrc = p_val;
	}

#ifdef _DEBUG
	if(pSrc->vt == VT_VARARRAY)
		if (pSrc->parray) _ASSERT(is_valid_safearray(pSrc->parray));

	if(pDst->vt == VT_VARARRAY)
		if (pDst->parray) _ASSERT(is_valid_safearray(pDst->parray));
#endif

	c_var_data* p_data = ((c_variable*)pDst)->get_var_data();
	HRESULT hr = VariantCopy(pDst, pSrc);
	((c_variable*)pDst)->set_var_data(p_data);

	_ASSERT(!FAILED(hr));
	c_engine* p_engine = get_engine();
	if (p_engine) p_engine->runtime_error(hr);
}

void c_variable::operator = (c_variable& val)
{
#ifdef _DEBUG
	assert_valid();
#endif

	c_var_data* p_old = get_var_data();

	if (get_ptr()->is_strict()) cast(val.get_ptr());
	else assign(&val);

	set_var_data(p_old);

#ifdef _DEBUG
	assert_valid();
#endif
}

// operator - -----------------------------------------------------------------

c_variable c_variable::operator - ()
{
	c_variable result;
	HRESULT hr = VarNeg(get_ptr(), &result);
	c_engine* p_engine = get_engine();
	if (p_engine) p_engine->runtime_error(hr);
	return result;
}

// operator += ----------------------------------------------------------------

void c_variable::operator += (c_variable& val)
{
	c_variable tmp;
	HRESULT hr;

	if (vt == VT_BSTR || val.vt == VT_BSTR)
		hr = VarCat(get_scalar_ptr(), val.get_scalar_ptr(), &tmp);
	else
		hr = VarAdd(get_scalar_ptr(), val.get_scalar_ptr(), &tmp);

	c_engine* p_engine = get_engine();
	if (p_engine) p_engine->runtime_error(hr);

	hr = VariantCopy(this, &tmp);
	if (p_engine) p_engine->runtime_error(hr);
}

// operator < -----------------------------------------------------------------

bool c_variable::operator < (c_variable& val)
{
	HRESULT hr = VarCmp(get_scalar_ptr(), val.get_scalar_ptr(), LOCALE_USER_DEFAULT, NORM_IGNORECASE);
	return hr == VARCMP_LT;
}

// operator > -----------------------------------------------------------------

bool c_variable::operator > (c_variable& val)
{
	HRESULT hr = VarCmp(get_scalar_ptr(), val.get_scalar_ptr(), LOCALE_USER_DEFAULT, NORM_IGNORECASE);
	return hr == VARCMP_GT;
}

// operator <= ----------------------------------------------------------------

bool c_variable::operator <= (c_variable& val)
{
	HRESULT hr = VarCmp(get_scalar_ptr(), val.get_scalar_ptr(), LOCALE_USER_DEFAULT, NORM_IGNORECASE);
	return (hr == VARCMP_LT) || (hr == VARCMP_EQ);
}

// operator >= ----------------------------------------------------------------

bool c_variable::operator >= (c_variable& val)
{
	HRESULT hr = VarCmp(get_scalar_ptr(), val.get_scalar_ptr(), LOCALE_USER_DEFAULT, NORM_IGNORECASE);
	return (hr == VARCMP_GT) || (hr == VARCMP_EQ);
}

// operator == ----------------------------------------------------------------

bool c_variable::operator == (c_variable& val)
{
	c_variable* p1 = (c_variable*)get_scalar_ptr();
	c_variable* p2 = (c_variable*)val.get_scalar_ptr();

	// well.... probably this can be uncommented...
/*
	if (p1->vt == VT_BOOL && p2->vt != VT_BOOL)
	{
		return p1->boolVal == (p2->as_integer() != 0);
	}

	if (p2->vt == VT_BOOL && p1->vt != VT_BOOL)
	{
		return p2->boolVal == (p1->as_integer() != 0);
	}
*/
	HRESULT hr = VarCmp(p1, p2, LOCALE_USER_DEFAULT, NORM_IGNORECASE);
	return hr == VARCMP_EQ;
}

// operator != ----------------------------------------------------------------

bool c_variable::operator != (c_variable& val)
{
	HRESULT hr = VarCmp(get_scalar_ptr(), val.get_scalar_ptr(), LOCALE_USER_DEFAULT, NORM_IGNORECASE);
	return hr != VARCMP_EQ;
}

// operator ! -----------------------------------------------------------------

bool c_variable::operator ! ()
{
	return !as_integer();
}

c_variable _power(c_variable a, c_variable b)
{
	c_variable result;
	HRESULT hr = VarPow(a.get_scalar_ptr(), b.get_scalar_ptr(), &result);
	c_engine* p_engine = a.get_engine();
	if (p_engine) p_engine->runtime_error(hr);

	return result;
}

// cast functions -------------------------------------------------------------

void c_variable::as_string(c_string& str)
{
	str.empty();

	switch (vt)
	{
	case VT_BSTR:
		//20220725 YDK 수정 - 한글 수정
		str = CW2A(bstrVal);
		break;

	default:
		{
			VARIANT* p = get_scalar_ptr();
			BSTR bstr = 0;
			HRESULT hr = 0;

			if (p->vt == VT_R8)
			{
				char buffer[32];
				sprintf(buffer, "%.14g", p->dblVal);
				str = buffer;
			}
//			else if (p->vt == VT_R4)
//			{
//				char buffer[32];
//				sprintf(buffer, "%g", p->fltVal);
//				str = buffer;
//			}
			else
			{
				//hr = VarFormatNumber(p, 14, 0, 0, 0, 0, &bstr);
				hr = VarFormat(p, L"", 0, 0, 0, &bstr);
				if (FAILED(hr))
				{
					c_engine* p_engine = get_engine();
					if (p_engine) p_engine->runtime_error(hr);
				}
				else
				{
					//_ASSERT(bstr); ???
					str = bstr;
					SysFreeString(bstr);
				}
			}
		}
	}
}

int c_variable::as_integer()
{
	switch (vt)
	{
	case VT_I2:
		return iVal;
		break;

	case VT_I4:
	case VT_INT:
	case VT_UINT:
		return intVal;
		break;

	case VT_BOOL:
		return boolVal;
		break;

	case VT_DATE:
	case VT_R8:
		return int(dblVal);
		break;

	case VT_BSTR:
		{
			wchar_t *p_end;
			return wcstol(bstrVal, &p_end, 10);
		}
		break;

	case VT_DISPATCH:
		return 0;
		break;

	case VT_EMPTY:
		return 0;
		break;

	case VT_VARARRAY:
		{
			c_variable* p_element = 0;
			get_element(parray, 0, &p_element);
			if (p_element) return p_element->as_integer();
		}
		break;

	default:
		return INT_MIN;
	}

	return INT_MIN;
}

double c_variable::as_double()
{
	switch (vt)
	{
	case VT_I2:
		return (double)iVal;

	case VT_I4:
	case VT_INT:
	case VT_UINT:
		return (double)intVal;

	case VT_BOOL:
		return (double)boolVal;

	case VT_R4:
		return fltVal;

	case VT_DATE:
	case VT_R8:
		return dblVal;

	case VT_BSTR:
		if (bstrVal) return wcstod(bstrVal, 0);
		else return 0;

	case VT_DISPATCH:
		return 0;

	case VT_EMPTY:
		return 0;

	case VT_VARARRAY:
		{
			c_variable* p_element = 0;
			get_element(parray, 0, &p_element);
			if (p_element) return p_element->as_double();
		}

	default:
		return INT_MIN;
	}

	return INT_MIN;
}

#ifdef _DEBUG
void c_variable::assert_valid()
{
	_ASSERT(!IsBadWritePtr(this, sizeof(c_variable)));
	c_var_data* p_var_data = get_var_data();

	if (p_var_data) p_var_data->assert_valid();

	if (vt == (VT_BYREF | VT_VARIANT))
	{
		_ASSERT(this != byref);
		((c_variable*)get_ptr())->assert_valid();
		return;
	}

	if (vt == VT_VARARRAY) if (parray) _ASSERT(parray->cLocks == 0);

	unsigned short base_type = vt & VT_TYPEMASK;
	unsigned short ext_type = vt & ~VT_TYPEMASK;

	_ASSERT(base_type == VT_EMPTY ||
			(base_type == VT_VARIANT && ext_type == VT_ARRAY) ||
			base_type == VT_I1 ||
			base_type == VT_I2 ||
			base_type == VT_I4 ||
			base_type == VT_BOOL ||
			base_type == VT_R4 ||
			base_type == VT_R8 ||
			base_type == VT_BSTR ||
			base_type == VT_DISPATCH ||
			base_type == VT_DATE ||
			base_type == VT_ERROR ||
			base_type == VT_NULL);

	_ASSERT(ext_type == 0 ||
			ext_type == VT_ARRAY);
}
#endif
