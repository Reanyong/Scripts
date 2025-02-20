#ifndef BEEBASIC_COM_H
#define BEEBASIC_COM_H

class c_string;

bool get_clsid_libid(const char* p_name, CLSID* p_clsid, GUID* p_libid, GUID* p_iid);

ITypeLib* load_typelib(GUID* p_libid);
bool is_matching_class(GUID* p_iid, IDispatch* p_disp);
HRESULT co_create_instance(CLSID clsid, IDispatch** ppd);

inline bool is_NULL_GUID(GUID* p)
{
	unsigned char* pp = (unsigned char*) p;
	register int i;

	for (i = 0; i < sizeof(GUID); i++)
		if (pp[i] != 0) return false;

	return true;
}

#endif