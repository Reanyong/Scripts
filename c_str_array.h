#ifndef	C_STR_ARRAY_H
#define C_STR_ARRAY_H

#include <stdlib.h>	// fealloc, free
#include <crtdbg.h>	// _ASSERT

class c_str_array  
{
	int				m_nsize;
	int				m_nalloc;
	int				m_ngrow;
	char**			m_pdata;
	bool			grow();
public:
	c_str_array();
	~c_str_array();

	inline const char* operator[] (int i);
	inline const char* get(int i);
	inline const char* get_last();

	char** get_data()	{return m_pdata;}
	int get_size()		{return m_nsize;}

	bool add(const char* p);

	void empty();
	void remove_at(int n);
	void remove_empty();

	bool contains(const char* p_str, bool b_case = false);
	void copy(c_str_array* p_origin);

	void split(const char* p_str, char c_delim, bool b_trim = true);
	void split(const char* p_str, const char* p_delim);
	void split_lines(const char* p_str);

	void set_at(int n, const char* p);
	void insert(int n, const char* p);
	void swap (int n1, int n2);

	void fill_variant(VARIANT* p_var);
	void fill_safearray(SAFEARRAY** psa);
	void from_safearray(SAFEARRAY* psa);
};

// inlines --------------------------------------------------------------------

inline const char* c_str_array::operator[] (int i)
{
	return get(i);
}

inline const char* c_str_array::get(int i)
{
	_ASSERT(i >=0 && i < m_nsize);
	return m_pdata[i];
}

inline const char* c_str_array::get_last()
{
	_ASSERT(m_nsize);
	return m_pdata[m_nsize - 1];
}

#endif
