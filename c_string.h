#ifndef C_STRING_H
#define C_STRING_H

#include "windows.h"

class c_string
{
	char* m_pdata;						// pointer to the string
	int m_nalloc;						// length allocated (but zero)
	int m_nlength;						// string's length  (but zero)
	int m_ngrow;						// number of bytes to grow by

public:
	c_string();
	c_string(const c_string& origin);
	c_string(const char* p_str);
	c_string(BSTR bstr);
	~c_string();

	void format(const char* p_format, ...);
	void formatV(const char* p_format, va_list argList);

	void operator =  (const c_string&	origin);
	void operator =  (const char*		p_origin);
	void operator =  (const int			origin);
	void operator =  (const double		origin);
	void operator =  (BSTR bstr);

	void operator += (const c_string&	origin);
	void operator += (const char*		p_origin);
	void operator += (const char		_char);

	void operator += (const int			val);
	void operator += (const double		val);

	const char* get_buffer() const
	{
		return m_pdata ? m_pdata : "";  // NULL이면 빈 문자열 반환
	}

	void empty();
	void kill(const int n_pos);
	void insert(const char c, const int n_pos);

	int get_length() const {return m_nlength;}

	void copy(const char* p_origin, int n_length);
	void append(const char* p_origin, int n_length);

	void l_trim();
	void r_trim();

	void u_case();
	void l_case();

	char get_last();

	operator LPCTSTR();
	BSTR sys_alloc_string();

	void format_error(bool b_wsa);
	inline bool eq(const char* p);
};

inline bool c_string::eq(const char* p)
{
	if (!m_pdata) return (p == 0 || *p == 0);
	if (!p) return false;

	//20210326 warning
	//return !stricmp(m_pdata, p);
	return !_stricmp(m_pdata, p);
}
#endif
