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
	//register int i; C++17 부터 register 키워드가 의미가 없음, C++20 부터는 완전히 제거 되었음.
	int i;

	for (i = 0; i < sizeof(GUID); i++)
		if (pp[i] != 0) return false;

	return true;
}

#endif