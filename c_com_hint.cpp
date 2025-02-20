#include "pch.h"
#include "c_com_hint.h"
#include "c_engine.h"
#include "com.h"

//-----------------------------------------------------------------------------
// construction/destruction ---------------------------------------------------
//-----------------------------------------------------------------------------

c_com_hint::c_com_hint(c_engine* p_engine)
{
	_ASSERT(p_engine);
	m_pengine = p_engine;
}

//-----------------------------------------------------------------------------
// get_member_type ------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_com_hint::get_member_type(ITypeInfo* pti,
					 const char* p_member,
					 VARTYPE* p_type,
					 HREFTYPE* p_href_type)
{
	*p_type = -1;
	*p_href_type = -1;

// get memberid of name -------------------------------------------------------

	MEMBERID memid;

	int n = (int)strlen(p_member);
	if (!n) return;

	BSTR bstr_member = SysAllocStringLen(0, n + 1);
	mbstowcs(bstr_member, p_member, (n + 1));

	HRESULT hr = pti->GetIDsOfNames(&bstr_member, 1, &memid);

	SysFreeString(bstr_member);

	if (FAILED(hr)) return;
	if (memid < 0) return;

// get TypeAttr, the information about all functions --------------------------

	TYPEATTR* pta;
	hr = pti->GetTypeAttr(&pta);
	if (FAILED(hr)) return;

	get_dispatch_type_info(&pti, &pta);

// search for function by memberid --------------------------------------------

	int i;
	for (i = 0; i < pta->cFuncs; i++)
	{
		FUNCDESC* pfd;
		hr = pti->GetFuncDesc(i, &pfd);
		if (FAILED(hr)) continue;

		if (pfd->memid == memid)
		{
			*p_type = pfd->elemdescFunc.tdesc.vt;
			if (*p_type == VT_PTR)
			{
				*p_type = VT_DISPATCH;
				*p_href_type = pfd->elemdescFunc.tdesc.lptdesc->hreftype;
			}

			pti->ReleaseFuncDesc(pfd);
			break;
		}

		pti->ReleaseFuncDesc(pfd);
	}

	pti->ReleaseTypeAttr(pta);
}

//-----------------------------------------------------------------------------
// get_typeinfo ---------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_com_hint::get_typeinfo(const char* p_class,
							  c_str_array* p_levels,
							  bool b_for_list,
							  ITypeLib** pp_tl,
							  ITypeInfo** pp_ti)
{
	_ASSERT(p_levels);
	_ASSERT(p_levels->get_size());

	HRESULT hr;
	ITypeInfo *pti = 0;
	ITypeLib* ptl = 0;

	*pp_tl = 0;
	*pp_ti = 0;
	int n = 1;

	if (p_class && strlen(p_class))
	{
		// this is object variable

		CLSID clsid = {0};
		GUID  libid = {0};

		if (get_clsid_libid(p_class, &clsid, &libid, 0))
		{
			USES_CONVERSION;
			ptl = load_typelib(&libid);
			if (ptl)
			{
				hr = ptl->GetTypeInfoOfGuid(clsid, &pti);
				_ASSERT(!FAILED(hr) && pti);
			}
		}
	}
	else
	{
		// this is object added with AddObject

		IDispatch* lpd = m_pengine->get_object(p_levels->get(0));

		if (!lpd)
		{
			lpd = m_pengine->get_default_parent(p_levels->get(0));
			n = 0;
		}

		if (lpd)
		{
			hr = lpd->GetTypeInfo(0, 0, &pti);
			_ASSERT(!FAILED(hr));
			_ASSERT(pti);
		}
	}

// go through the rest of levels

	if (pti)
	{
		VARTYPE type;
		HREFTYPE href_type;
		int i;
		int n_top = p_levels->get_size();
		if (!b_for_list) n_top --;

		for (i = n; i < n_top; i++)
		{
			get_member_type(pti,
							p_levels->get(i),
							&type,
							&href_type);

			// non-object member
			if (href_type == -1)
			{
				if (b_for_list || i < n_top - 1)
				{
					pti->Release();
					pti = 0;
				}
				break;
			}

			// get reference type
			ITypeInfo* pti_ref;
			hr = pti->GetRefTypeInfo(href_type, &pti_ref);
			if (!FAILED(hr) && pti_ref)
			{
				pti->Release();
				pti = pti_ref;
			}
			else
			{
				pti->Release();
				pti = 0;
				break;
			}
		}
	}

	*pp_ti = pti;
	*pp_tl = ptl;
}

//-----------------------------------------------------------------------------
// get_func_desc --------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_com_hint::get_func_desc(ITypeInfo* pti,
							   const char* p_member,
							   FUNCDESC **pp_fd)
{
	_ASSERT(p_member && strlen(p_member));

	*pp_fd = 0;

	TYPEATTR* pta;
	HRESULT hr = pti->GetTypeAttr(&pta);
	if (FAILED(hr)) return;

	get_dispatch_type_info(&pti, &pta);

// iterate through all members to find member by name

	c_string str_name;
	int i;

	for (i = 0; i < pta->cFuncs; i++)
	{
		FUNCDESC* pfd;
		hr = pti->GetFuncDesc(i, &pfd);
		if (FAILED(hr)) continue;

		BSTR pb_name;
		pti->GetDocumentation(pfd->memid, &pb_name, 0, 0, 0);
		str_name = pb_name;
		SysFreeString(pb_name);

		if (_stricmp(str_name, p_member) == 0)
		{
			*pp_fd = pfd;
			break;
		}

		pti->ReleaseFuncDesc(pfd);
	}
}

//-----------------------------------------------------------------------------
// is_byref -------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_com_hint::is_byref(TYPEDESC td)
{
	if (td.vt == VT_PTR) return true;
	return (td.vt & VT_BYREF) != 0;
}

//-----------------------------------------------------------------------------
// get_type_name --------------------------------------------------------------
//-----------------------------------------------------------------------------

const char* c_com_hint::get_type_name(ITypeInfo* pti, TYPEDESC td)
{
	HRESULT hr = 0;
	HREFTYPE hrftp = 0;

	if (td.vt == VT_PTR)
	{
		if (td.lptdesc && td.lptdesc->vt != VT_PTR && td.lptdesc->vt != VT_USERDEFINED)
			return ::get_type_name((VARENUM)td.lptdesc->vt);
		else
			hrftp = td.lptdesc->hreftype;
	}
	else if (td.vt == VT_USERDEFINED)
		hrftp = td.hreftype;

	if (hrftp)
	{
		const char* p_ret = "UNKNOWN";

		ITypeInfo* pti_ref = 0;
		hr = pti->GetRefTypeInfo(hrftp, &pti_ref);
		if (!FAILED(hr) && pti_ref)
		{
			TYPEATTR* pta = 0;
			hr = pti_ref->GetTypeAttr(&pta);
			if (!FAILED(hr) && pta)
			{
				if (pta->typekind == TKIND_ENUM)
					p_ret = "Enum";
				else if (pta->typekind == TKIND_DISPATCH)
					p_ret = "Object";

				pti->ReleaseTypeAttr(pta);
			}

			pti_ref->Release();
		}

		return p_ret;
	}
	else
		return ::get_type_name((VARENUM)td.vt);
}

//-----------------------------------------------------------------------------
// format_funcdesc ------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_com_hint::format_funcdesc(ITypeInfo* pti,
								 FUNCDESC* pfd,
								 const char** pp_hint)
{
	BSTR pb_name = 0;
	pti->GetDocumentation(pfd->memid, &pb_name, 0, 0, 0);
	c_string str_name = pb_name;
	SysFreeString(pb_name);
	const char* p_type = 0;

	if (pfd->invkind & DISPATCH_PROPERTYGET)
	{
		p_type = get_type_name(pti, pfd->elemdescFunc.tdesc);
		if (p_type && *p_type)
		{
			m_format = str_name;
			m_format += " as ";
			m_format += p_type;
		}
	}
	else if (pfd->invkind & DISPATCH_PROPERTYPUT)
	{
		if (pfd->cParams == 1)
		{
			p_type = get_type_name(pti, pfd->lprgelemdescParam[0].tdesc);
			if (p_type && *p_type)
			{
				m_format = str_name;
				m_format += " as ";
				m_format += p_type;
			}
		}
	}
	else if (pfd->invkind & DISPATCH_METHOD)
	{
		m_format = str_name;
		m_format += '(';

		int i;
		for (i = 0; i < pfd->cParams; i++)
		{
			if (i) m_format += ", ";
			if (is_byref(pfd->lprgelemdescParam[i].tdesc)) m_format += "ByRef ";
			m_format += "v";
			m_format += i;
			p_type = get_type_name(pti, pfd->lprgelemdescParam[i].tdesc);
			if (p_type)
			{
				m_format += " as ";
				m_format += p_type;
			}
		}

		m_format += ')';

		p_type = get_type_name(pti, pfd->elemdescFunc.tdesc);
		if (p_type && *p_type)
		{
			m_format += " as ";
			m_format += p_type;
		}
	}

	*pp_hint = m_format.get_buffer();
}

//-----------------------------------------------------------------------------
// list_members ---------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_com_hint::list_members(const char* p_class,
							  c_str_array* p_levels,
							  c_str_array* p_members,
							  c_array<bool>* p_methods)
{
	p_members->empty();
	p_methods->reset();

	if (p_levels->get_size() < 1) return;

//

	ITypeLib* ptl = 0;
	ITypeInfo* pti = 0;
	get_typeinfo(p_class, p_levels, true, &ptl, &pti);
	if (!pti)
	{
		if (ptl) ptl->Release();
		return;
	}

//

	c_string str_name;
	TYPEATTR* pta = 0;
	HRESULT hr = pti->GetTypeAttr(&pta);
	if (FAILED(hr)) return;
	if (!pta) return;

	get_dispatch_type_info(&pti, &pta);

// get all members ------------------------------------------------------------

	_ASSERT(pta->cFuncs);
	int i;
	for (i = 0; i < pta->cFuncs; i++)
	{
		FUNCDESC* pfd;
		pti->GetFuncDesc(i, &pfd);

		if (pfd->wFuncFlags & (FUNCFLAG_FRESTRICTED | FUNCFLAG_FHIDDEN))
		{
			pti->ReleaseFuncDesc(pfd);
			continue;
		}

		BSTR pb_name = 0;
		pti->GetDocumentation(pfd->memid, &pb_name, 0, 0, 0);
		str_name = pb_name;
		SysFreeString(pb_name);

//		if (pfd->memid) // this would hide some meaningful names , like "Item".
		if (_stricmp(str_name.get_buffer(), "_Default"))
		if (!p_members->contains(str_name.get_buffer()))
		{
			_ASSERT(pfd->invkind & DISPATCH_PROPERTYGET ||
					pfd->invkind & DISPATCH_PROPERTYPUT ||
					pfd->invkind & DISPATCH_METHOD);

			p_members->add(str_name.get_buffer());

			bool b = pfd->invkind & (DISPATCH_METHOD != 0);
			p_methods->add(b);
		}

		pti->ReleaseFuncDesc(pfd);
	}

	pti->ReleaseTypeAttr(pta);
	pti->Release();

	_ASSERT(p_members->get_size() == p_methods->get_size());

// sort -----------------------------------------------------------------------

	bool b_swap;
	do
	{
		b_swap = false;

		for (i = 0; i < p_members->get_size() - 1; i++)
		{
			if (_stricmp(p_members->get(i), p_members->get(i + 1)) > 0)
			{
				p_members->swap(i, i + 1);
				p_methods->swap(i, i + 1);

				b_swap = true;
			}
		}
	}
	while (b_swap);}

//-----------------------------------------------------------------------------
// get_hint -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_com_hint::get_hint(const char* p_class,
						  c_str_array* p_levels,
						  const char** pp_hint)
{
	*pp_hint = 0;
	if (!p_levels->get_size()) return;

	ITypeLib* ptl = 0;
	ITypeInfo* pti = 0;
	get_typeinfo(p_class, p_levels, false, &ptl, &pti);
	if (!pti)
	{
		if (ptl) ptl->Release();
		return;
	}

//

	const char* p_member = p_levels->get_last();
	FUNCDESC* pfd = 0;
	get_func_desc(pti, p_member, &pfd);

	if (pfd) format_funcdesc(pti, pfd, pp_hint);

	pti->ReleaseFuncDesc(pfd);
	pti->Release();
	if (ptl) ptl->Release();
}
