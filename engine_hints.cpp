#include "pch.h"

#include <crtdbg.h>
#include "c_engine.h"

//-----------------------------------------------------------------------------
// str_start ------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool str_start(const char* p_str, const char* p_start)
{
	if (! (p_str && p_start)) return false;

	while (*p_start)
	{
		if (*p_str)
		{
			if (*p_str != *p_start)
				if (tolower(*p_str) != tolower(*p_start)) return false;
		}
		else return false;

		p_str ++;
		p_start ++;
	}

	return true;
}

//-----------------------------------------------------------------------------
// get_argument_pointers ------------------------------------------------------
//-----------------------------------------------------------------------------

void get_argument_pointers(c_ptr_array<c_variable>& ptr_args, c_array<c_variable>& args)
{
	int i;
	for (i = 0; i < args.get_size(); i++)
	{
		if ((args[i].vt == (VT_BYREF | VT_VARIANT)) && args[i].byref)
			ptr_args.add((c_variable*)args[i].byref);
		else
			ptr_args.add(args.get_ptr(i));
	}
}

//-----------------------------------------------------------------------------
// get_type_name --------------------------------------------------------------
//-----------------------------------------------------------------------------

const char* get_type_name(VARENUM vt)
{
	const char* p_result = 0;

	switch (vt)
	{
		case VT_NULL:		p_result = "null";			break;
		case VT_ERROR:		p_result = "error";			break;
		case VT_EMPTY:		p_result = "empty";			break;
		case VT_I2:			p_result = "integer";		break;
		case VT_INT:
		case VT_I4:			p_result = "long";			break;
		case VT_R4:			p_result = "float";			break;
		case VT_R8:			p_result = "double";		break;
		case VT_CY:			p_result = "currency";		break;
		case VT_DATE:		p_result = "date";			break;
		case VT_BSTR:		p_result = "string";		break;
		case VT_BOOL:		p_result = "boolean";		break;
		case VT_VARIANT:	p_result = "variant";		break;
		case VT_DECIMAL:	p_result = "decimal";		break;
		case VT_I1:			p_result = "char";			break;
		case VT_UI1:		p_result = "byte";			break;
		case VT_UI2:		p_result = "unsigned int";	break;
		case VT_UINT:
		case VT_UI4:		p_result = "unsigned long";	break;
		case VT_DISPATCH:	p_result = "object";		break;
		case VT_VARARRAY:	p_result = "array";			break;
		case VT_VOID:		p_result = "";				break;
		default:
			_ASSERT(0);
			p_result = "UNKNOWN";
	}

	return p_result;
}

//-----------------------------------------------------------------------------
// get_type_name --------------------------------------------------------------
//-----------------------------------------------------------------------------

void get_type_name(c_variable* pv, c_string* p_str)
{
	p_str->empty();

	if (pv->vt & VT_BYREF)
	{
		c_variable* p = pv->get_scalar_ptr();
		if (p)
		{
			*p_str = "byref ";
			*p_str += get_type_name((VARENUM)p->vt);
		}
	}
	else
		*p_str = get_type_name((VARENUM)pv->vt);
}

//-----------------------------------------------------------------------------
// safearray_from_variant -----------------------------------------------------
//-----------------------------------------------------------------------------

SAFEARRAY* safearray_from_variant(const VARIANT* p)
{
	SAFEARRAY* parray = 0;

	switch( p->vt)
	{
	case (VT_VARIANT | VT_ARRAY):
		{
			parray = p->parray;
		}
		break;

	case (VT_VARIANT | VT_BYREF):
		{
			p = (VARIANT*)p->byref;

			if (p && p->vt == VT_VARARRAY)
			{
				parray = p->parray;
			}
		}
		break;

	case (VT_VARIANT | VT_ARRAY | VT_BYREF):
		{
			parray = (SAFEARRAY*)*p->pparray;
		}
		break;
	}

	return parray;
}

//-----------------------------------------------------------------------------
// add_hint -------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_engine::add_hint(const char* p_keyword, const char* p_hint)
{
	if (!p_keyword)	return false;
	if (!p_hint)	return false;
	if (!*p_keyword)return false;
	if (!*p_hint)	return false;

	int i;
	for (i = 0; i < m_hint_kwds.get_size(); i++)
		if (_stricmp(p_keyword, m_hint_kwds[i]) == 0)
		{
			m_hint_descs.set_at(i, p_hint);
			return true;
		}

	m_hint_kwds.add(p_keyword);
	m_hint_descs.add(p_hint);

	return true;
}

//-----------------------------------------------------------------------------
// delete_hint ----------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_engine::delete_hint(const char* p_keyword)
{
	if (!p_keyword)  return;
	if (!*p_keyword) return;

	int i;
	for (i = 0; i < m_hint_kwds.get_size(); i++)
		if (_stricmp(p_keyword, m_hint_kwds[i]) == 0)
		{
			m_hint_kwds.remove_at(i);
			m_hint_descs.remove_at(i);
		}
}

//-----------------------------------------------------------------------------
// delete_all_hints -----------------------------------------------------------
//-----------------------------------------------------------------------------

void c_engine::delete_all_hints()
{
	_ASSERT(m_hint_kwds.get_size() == m_hint_descs.get_size());

	while (m_hint_descs.get_size() >= n_default_hints)
	{
		m_hint_kwds.remove_at(n_default_hints);
		m_hint_descs.remove_at(n_default_hints);
	}
}

//-----------------------------------------------------------------------------
// get_hint -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_engine::get_hint(const char* p_keyword, const char** pp_hint)
{
	*pp_hint = 0;

// try extension functions

	int i;
	for (i = 0; i < m_hint_kwds.get_size(); i++)
	{
		if (_stricmp(m_hint_kwds[i], p_keyword) == 0)
		{
			*pp_hint = m_hint_descs[i];
			return;
		}
	}
}

//-----------------------------------------------------------------------------
// get_hint -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_engine::get_hint(c_str_array* p_levels,
						const char** pp_hint,
						const char* p_type)
{
	*pp_hint = 0;

// try single-word hint

	if (p_levels->get_size() == 1)
	{
		get_hint(p_levels->get(0), pp_hint);
		if (*pp_hint) return;
	}

// try dispatch hint

	m_com_hint.get_hint(p_type, p_levels, pp_hint);
}

//-----------------------------------------------------------------------------
// list_members ---------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_engine::list_members(const char* p_class_name,
							c_str_array* p_levels,
							c_str_array* p_members,
							c_array<bool>* p_methods)
{
	m_com_hint.list_members(p_class_name, p_levels, p_members, p_methods);
}

void c_engine::list_all_obj_members(bool b_functions,
									bool b_subs,
									bool b_props,
									c_str_array* p_array)
{
	p_array->empty();

	unsigned int n_flags = 0;
	if (b_functions || b_subs) n_flags |= DISPATCH_METHOD;
	if (b_props) n_flags |= (DISPATCH_PROPERTYGET | DISPATCH_PROPERTYPUT);

	if (!n_flags) return;

	HRESULT hr;
	ITypeInfo* pti = 0;
	TYPEATTR* pta = 0;
	c_string str_name;

	int i, j;
	for (i = 0; i < m_ext_objects.get_size(); i++)
	{
		hr = m_ext_objects.vars[i].pdispVal->GetTypeInfo(0, 0, &pti);
		if (FAILED(hr)) return;
		if (!pti) return;

		hr = pti->GetTypeAttr(&pta);
		if (FAILED(hr))
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
				pti->Release();
				pti = pti_temp;
				pti->GetTypeAttr(&pta);
			}
		}

		for (j = 0; j < pta->cFuncs; j++)
		{
			FUNCDESC* pfd;
			hr = pti->GetFuncDesc(j, &pfd);
			if (FAILED(hr)) continue;

			if ( (pfd->wFuncFlags & FUNCFLAG_FHIDDEN) == 0)
				if (pfd->invkind & n_flags)
				{
					if ( (b_subs && (pfd->elemdescFunc.tdesc.vt == VT_VOID)) ||
						 (b_functions && (pfd->elemdescFunc.tdesc.vt != VT_VOID)) )
					{
						BSTR pb_name;
						pti->GetDocumentation(pfd->memid, &pb_name, 0, 0, 0);
						str_name = pb_name;
						SysFreeString(pb_name);
						p_array->add(str_name.get_buffer());
					}
				}

			pti->ReleaseFuncDesc(pfd);
		}

		pti->ReleaseTypeAttr(pta);
		pti->Release();
	}
}
