#include "pch.h"
#include "c_string.h"
#include "com.h"

//-----------------------------------------------------------------------------
// alloc_BSTR -----------------------------------------------------------------
//-----------------------------------------------------------------------------

BSTR alloc_BSTR(const char* p)
{
	if (!p) return 0;

	int n_len = MultiByteToWideChar(CP_ACP, 0, p, (int)strlen(p), NULL, NULL);
	BSTR bstr = ::SysAllocStringLen(NULL, n_len);
	_ASSERT(bstr);
	MultiByteToWideChar(CP_ACP, 0, p, (int)strlen(p), bstr, n_len);
	return bstr;
}

//-----------------------------------------------------------------------------
// string_from_uuid -----------------------------------------------------------
//-----------------------------------------------------------------------------

void string_from_uuid(UUID* p_uuid, char* p_out)
{
	sprintf(p_out,
			"{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}",
			p_uuid->Data1,
			p_uuid->Data2,
			p_uuid->Data3,
			p_uuid->Data4[0],
			p_uuid->Data4[1],
			p_uuid->Data4[2],
			p_uuid->Data4[3],
			p_uuid->Data4[4],
			p_uuid->Data4[5],
			p_uuid->Data4[6],
			p_uuid->Data4[7]);
}

//-----------------------------------------------------------------------------
// uuid_from_string -----------------------------------------------------------
//-----------------------------------------------------------------------------

void uuid_from_string(const char* p_str, UUID* p_uuid)
{
	char* p_end;
	char b[3] = {0};

	if (*p_str == '{') p_str ++;

	p_uuid->Data1 = strtoul(p_str, &p_end, 16);
	p_str = p_end + 1;
	p_uuid->Data2 = (unsigned short)strtoul(p_str, &p_end, 16);
	p_str = p_end + 1;
	p_uuid->Data3 = (unsigned short)strtoul(p_str, &p_end, 16);
	p_str = p_end + 1;

	int i;
	for (i = 0; i < 8; i++)
	{
		if (*p_str == '-') p_str ++;
		b[0] = *p_str ++;
		b[1] = *p_str ++;
		p_uuid->Data4[i] = (unsigned char)strtoul(b, &p_end, 16);
	}
}

//-----------------------------------------------------------------------------
// fix_file_name --------------------------------------------------------------
//-----------------------------------------------------------------------------

bool fix_file_name(const char* p, c_string* p_str)
{
	char buffer[MAX_PATH] = {0};

	if (GetFileAttributes(p) != -1)
	{
		*p_str = p;
		return true;
	}

	//
	//char* p_space = strrchr(p, ' ');
	//20210325 warning
	char* c = (char*)p;
	char* p_space = strrchr( c, ' ');

	if (p_space)
	{
		p_str->copy(p, int(p_space - p));
		if (GetFileAttributes(p) != -1) return true;

		if (sizeof(buffer) > GetLongPathName(p_str->get_buffer(), buffer, sizeof(buffer)))
		{
			if (GetFileAttributes(buffer) != -1)
			{
				*p_str = buffer;
				return true;
			}
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// get_clsid_libid3 -----------------------------------------------------------
//-----------------------------------------------------------------------------

bool get_clsid_libid3(const char* p_name, CLSID* p_clsid, GUID* p_libid)
{
	char buffer[MAX_PATH] = {0};
	long n_length;
	c_string s;

	BSTR olename = alloc_BSTR(p_name);
	HRESULT hr = CLSIDFromString(olename, p_clsid);
	SysFreeString(olename);

	if (FAILED(hr)) return false;

	string_from_uuid(p_clsid, buffer);

	s.format("CLSID\\%s\\LocalServer32", buffer);

	n_length = sizeof(buffer);
	if (ERROR_SUCCESS != RegQueryValue(HKEY_CLASSES_ROOT, s.get_buffer(), buffer, &n_length)) return false;

	if (!fix_file_name(buffer, &s)) return false;

// path found. load library.

	ITypeLib* ptl = 0;
	BSTR bstrPath = s.sys_alloc_string();
	hr = LoadTypeLibEx(bstrPath, REGKIND_NONE, &ptl);
	SysFreeString(bstrPath);
	if (FAILED(hr) || !ptl) return false;

// get lib attribute

	TLIBATTR* patr = 0;
	hr = ptl->GetLibAttr(&patr);
	if (FAILED(hr) || !patr)
	{
		ptl->Release();
		return false;
	}

	*p_libid = patr->guid;

	ptl->ReleaseTLibAttr(patr);
	ptl->Release();

	return true;
}

//-----------------------------------------------------------------------------
// get_clsid_libid2 -----------------------------------------------------------
//-----------------------------------------------------------------------------

bool get_clsid_libid2(const char* p_name, CLSID* p_clsid, GUID* p_libid)
{
	_ASSERT(p_name && *p_name);
	_ASSERT(p_clsid);
	_ASSERT(p_libid);

	char buffer[MAX_PATH] = {0};
	long n_length;
	c_string left, right;
	c_string s;

	const char* p = p_name;
	while (*p && *p != '.') left += *p ++;
	if (*p != '.') return false;
	left += *p ++;
	while (*p) right += *p ++;

	FILETIME ft;
	int i;
	for (i = 0; ; i++)
	{
		n_length = sizeof(buffer);
        if (ERROR_SUCCESS != RegEnumKeyEx(HKEY_CLASSES_ROOT,
										i,
										buffer,
										(unsigned long*)&n_length,
										NULL,
										NULL,
										NULL,
										&ft)) return false;

		if (!_strnicmp(buffer, left.get_buffer(), left.get_length())) break;
	}

// similar name was found. try it.

	s.format("%s\\Clsid", buffer);
	n_length = sizeof(buffer);
	if (ERROR_SUCCESS != RegQueryValue(HKEY_CLASSES_ROOT, s.get_buffer(), buffer, &n_length)) return false;

	s.format("CLSID\\%s\\InprocServer32", buffer);

	n_length = sizeof(buffer);
	if (ERROR_SUCCESS != RegQueryValue(HKEY_CLASSES_ROOT, s.get_buffer(), buffer, &n_length)) return false;
//	if (p_path) *p_path = buffer;

// path to the file found. try to load typelib.

	ITypeLib* ptl = 0;
	BSTR bstrPath = alloc_BSTR(buffer);
	HRESULT hr = LoadTypeLibEx(bstrPath, REGKIND_NONE, &ptl);
	SysFreeString(bstrPath);
	if (FAILED(hr) || !ptl) return false;

// get lib attribute

	TLIBATTR* patr = 0;
	hr = ptl->GetLibAttr(&patr);
	if (FAILED(hr) || !patr)
	{
		ptl->Release();
		return false;
	}

	*p_libid = patr->guid;

	ptl->ReleaseTLibAttr(patr);

// try to find name
	BSTR bstrName = right.sys_alloc_string();
	ITypeInfo* pti = 0;
	MEMBERID memid = -1;
	unsigned short n_count = 1;
	hr = ptl->FindName(bstrName, 0, &pti, &memid, &n_count);
	SysFreeString(bstrName);
	if (FAILED(hr) || !pti)
	{
		ptl->Release();
		return false;
	}

	if (p_clsid)
	{
		TYPEATTR* pta = 0;
		hr = pti->GetTypeAttr(&pta);
		_ASSERT(!FAILED(hr) && pta);

		*p_clsid = pta->guid;

		pti->ReleaseTypeAttr(pta);
	}

	pti->Release();
	ptl->Release();

	return true;
}

//-----------------------------------------------------------------------------
// get_clsid_libid1 -----------------------------------------------------------
//-----------------------------------------------------------------------------

bool get_clsid_libid1(const char* p_name, CLSID* p_clsid, GUID* p_libid)
{
	_ASSERT(p_name && *p_name);
	_ASSERT(p_clsid);
	_ASSERT(p_libid);

	BSTR olename = alloc_BSTR(p_name);
	HRESULT hr = CLSIDFromString(olename, p_clsid);
	SysFreeString(olename);

	if (!FAILED(hr))
	{
		char buffer[MAX_PATH] = {0};
		c_string s;
		long n_length = sizeof(buffer);

		string_from_uuid(p_clsid, buffer);
		s.format("CLSID\\%s\\TypeLib", buffer);

		if (ERROR_SUCCESS == RegQueryValue(HKEY_CLASSES_ROOT, s.get_buffer(), buffer, &n_length))
		{
			uuid_from_string(buffer, p_libid);
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// get_clsid_libid ------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef FREE_BEFORE_EXIT
#undef FREE_BEFORE_EXIT
#endif

#define FREE_BEFORE_EXIT() \
if (pta2) pti->ReleaseTypeAttr(pta2);\
if (pti2) pti2->Release();\
if (pta) pti->ReleaseTypeAttr(pta);\
if (pti) pti->Release();\
if (ptl) ptl->Release();\

bool get_clsid_libid(const char* p_name, CLSID* p_clsid, GUID* p_libid, GUID* p_iid)
{
	if (!get_clsid_libid1(p_name, p_clsid, p_libid))
		if (!get_clsid_libid2(p_name, p_clsid, p_libid))
			if (!get_clsid_libid3(p_name, p_clsid, p_libid)) return false;

	if (!p_iid) return true;

// get interface ID

	ITypeInfo* pti = 0;
	TYPEATTR* pta = 0;
	ITypeInfo* pti2 = 0;
	TYPEATTR* pta2 = 0;
	unsigned int i;

	memset(p_iid, 0, sizeof(GUID));

	ITypeLib* ptl = load_typelib(p_libid);
	if (!ptl) return false;

	HRESULT hr = ptl->GetTypeInfoOfGuid(*p_clsid, &pti);
	if (FAILED(hr) || !pti)
	{
		FREE_BEFORE_EXIT();
		return false;
	}

	hr = pti->GetTypeAttr(&pta);
	if (FAILED(hr) || !pta)
	{
		FREE_BEFORE_EXIT();
		return false;
	}

	_ASSERT(pta->typekind == TKIND_COCLASS);

	for (i = 0; i < pta->cImplTypes; i ++)
	{
		HREFTYPE RefType = 0;

		hr = pti->GetRefTypeOfImplType(i, &RefType);
		if (FAILED(hr) || !RefType)
		{
			FREE_BEFORE_EXIT();
			return false;
		}

		pti2 = 0;
		hr = pti->GetRefTypeInfo(RefType, &pti2);
		if (FAILED(hr) || !pti2)
		{
			FREE_BEFORE_EXIT();
			return false;
		}

		pta2 = 0;
		hr = pti2->GetTypeAttr(&pta2);
		if (FAILED(hr) || !pta2)
		{
			FREE_BEFORE_EXIT();
			return false;
		}

		if (pta2->guid != IID_IDispatch && pta2->guid != IID_IUnknown)
		{
			*p_iid = pta2->guid;
			FREE_BEFORE_EXIT();
			return true;
		}
		else _ASSERT(0); // for debug
	}

	return false;
}

//-----------------------------------------------------------------------------
// load_typelib ---------------------------------------------------------------
//-----------------------------------------------------------------------------

ITypeLib* load_typelib(GUID* p_libid)
{
	_ASSERT(p_libid);
	c_string s;

	char buffer[MAX_PATH] = {0};
	string_from_uuid(p_libid, buffer);

	s.format("TypeLib\\%s", buffer);

	HKEY key;
	if (ERROR_SUCCESS != RegOpenKey(HKEY_CLASSES_ROOT, s.get_buffer(), &key)) return 0;

	char buffer2[MAX_PATH] = {0};
	unsigned long n_length;
	FILETIME write_time;

	unsigned int i;
	for (i = 0; ; i++)
	{
		n_length = sizeof(buffer2);
		if (ERROR_SUCCESS != RegEnumKeyEx(key,
										  i,
										  buffer2,
										  &n_length,
										  0,
										  0,
										  0,
										  &write_time)) break;
	}

	if (!strlen(buffer2))
	{
#ifdef _DEBUG
		MessageBox(0, "error 1", "LOAD TYPELIB", 0);
#endif
		RegCloseKey(key);
		return 0;
	}

	s.format("TypeLib\\%s\\%s\\0\\Win32", buffer, buffer2);

	n_length = sizeof(buffer);
	int nRet = RegQueryValue(HKEY_CLASSES_ROOT, s.get_buffer(), buffer, (long*)&n_length);
	if (ERROR_SUCCESS != nRet)
	{
#ifdef _DEBUG
		c_string err;
		err.format_error(false);

		c_string msg;
		msg.format("error reading %s\n\nerror code: %d\n\n%s", s.get_buffer(), nRet, err.get_buffer());
		MessageBox(0, msg.get_buffer(), "LOAD TYPELIB", 0);
#endif

		RegCloseKey(key);
		return 0;
	}

// we have path to the typelib now. load it.

	USES_CONVERSION;

	ITypeLib* ptl = 0;
	if (CT2OLE(buffer) !=NULL)
	{
		HRESULT hr = LoadTypeLib(CT2OLE(buffer), &ptl);
		_ASSERT(!FAILED(hr) && ptl);
	}
	return ptl;
}

//-----------------------------------------------------------------------------
// is_matching_class ----------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef FREE_BEFORE_EXIT
#undef FREE_BEFORE_EXIT
#endif

#define FREE_BEFORE_EXIT() \
if (pta) pti->ReleaseTypeAttr(pta);\
if (pti) pti->Release();

bool is_matching_class(GUID* p_iid, IDispatch* p_disp)
{
	_ASSERT(p_disp);
	_ASSERT(p_iid);
	_ASSERT(!is_NULL_GUID(p_iid));

	HRESULT hr = 0;
	ITypeInfo* pti = 0;
	TYPEATTR* pta = 0;
	bool b_ret = false;

	hr = p_disp->GetTypeInfo(0, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &pti);
	if (FAILED(hr) || !pti)
	{
		FREE_BEFORE_EXIT();
		return false;
	}

	hr = pti->GetTypeAttr(&pta);
	if (FAILED(hr) || !pta)
	{
		FREE_BEFORE_EXIT();
		return false;
	}

	b_ret = (pta->guid == *p_iid);

	FREE_BEFORE_EXIT();
	return b_ret;
}

//-----------------------------------------------------------------------------
// co_create_instance ---------------------------------------------------------
//-----------------------------------------------------------------------------

HRESULT co_create_instance(CLSID clsid, IDispatch** ppd)
{
	*ppd = 0;

	HRESULT hr = CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER, IID_IDispatch, (void**)ppd);
	if (!FAILED(hr) && *ppd) return hr;

	hr = CoCreateInstance(clsid, 0, CLSCTX_INPROC_HANDLER , IID_IDispatch, (void**)ppd);
	if (!FAILED(hr) && *ppd) return hr;

	hr = CoCreateInstance(clsid, 0, CLSCTX_LOCAL_SERVER , IID_IDispatch, (void**)ppd);
	if (!FAILED(hr) && *ppd) return hr;

	return hr;
}