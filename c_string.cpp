#include "pch.h"

#include <stdio.h>
#include <stdlib.h>
#include <Winsock2.h>
#include "c_string.h"

#pragma comment(lib, "Ws2_32.lib")

//MUST BE NOT LESS THAN NEEDED TO CONTAIN CONVERTED DOUBLE
#define MIN_C_STRING_GROW	32

c_string::c_string()
{
	m_pdata		= 0;
	m_nalloc	= 0;
	m_nlength	= 0;
	m_ngrow		= MIN_C_STRING_GROW;
}


c_string::c_string(c_string& origin)
{
	m_ngrow		= MIN_C_STRING_GROW;

	if (origin.m_nlength == 0)
	{
		m_nlength = 0;
		m_pdata		= 0;
		m_nalloc	= 0;
		m_nlength	= 0;
		return;
	}

	m_pdata   = (char*)malloc(origin.m_nlength + 1);
	m_nalloc  = origin.m_nlength;
	m_nlength = origin.m_nlength;

	memcpy(m_pdata, origin.m_pdata, origin.m_nlength + 1);
}

c_string::c_string(const char* p_str)
{
	m_pdata		= 0;
	m_nalloc	= 0;
	m_nlength	= 0;
	m_ngrow		= MIN_C_STRING_GROW;

	*this = p_str; // use operator =
}

c_string::c_string(BSTR bstr)
{
	m_pdata		= NULL;
	m_nalloc	= 0;
	m_nlength	= 0;

	*this = bstr;
}

c_string::~c_string()
{
	if (m_pdata != 0) free(m_pdata);
	m_pdata = 0;
}

void c_string::operator = (const c_string& origin)
{
	if (origin.get_length() == 0) empty();
	else
	{
		if(m_nalloc < origin.m_nlength)
		{
			m_nalloc  = origin.m_nlength;
			m_pdata   = (char*)realloc(m_pdata, m_nalloc + 1);
		}

		memcpy(m_pdata, origin.m_pdata, origin.m_nlength + 1);
		m_nlength = origin.m_nlength;
	}
}

void c_string::operator = (const char* p_origin)
{
	if (p_origin == 0 || *p_origin == 0)
	{
		empty();
		return;
	}

	int n_str_length = (int)strlen(p_origin);

	if(m_nalloc < n_str_length)
	{
		m_pdata   = (char*)realloc(m_pdata, n_str_length + 1);
		m_nalloc  = n_str_length;
	}
	m_nlength = n_str_length;

	strcpy(m_pdata, p_origin);
}

void c_string::operator = (const int origin)
{
	if (m_nalloc < m_ngrow)
	{
		m_nalloc = m_ngrow;
		m_pdata = (char*)realloc(m_pdata, m_nalloc + 1);
	}

	_itoa(origin, m_pdata, 10);
	m_nlength = (int)strlen(m_pdata);
}

void c_string::operator = (const double origin)
{
	if (m_nalloc < m_ngrow)
	{
		m_nalloc = m_ngrow;
		m_pdata = (char*)realloc(m_pdata, m_nalloc + 1);
	}

	sprintf(m_pdata, "%.20g", origin);
	m_nlength = (int)strlen(m_pdata);
}

void c_string::operator = (BSTR bstr)
{
	if (!bstr)
	{
		empty();
		return;
	}

	int n_len = (int)wcslen(bstr);

	if (!n_len)
	{
		empty();
		return;
	}

	if(m_nalloc < n_len)
	{
		m_pdata   = (char*)realloc(m_pdata, n_len + 1);
		m_nalloc  = n_len;
	}

	wcstombs(m_pdata, bstr, (n_len*2)+1);
	m_nlength = (int)strlen(m_pdata);
}

void c_string::operator += (const c_string& origin)
{
	if (origin.m_nlength == 0) return;

	if (m_nlength + origin.m_nlength > m_nalloc)
	{
		m_nalloc  =  m_nlength + origin.m_nlength;
		m_pdata   =  (char*)realloc(m_pdata, m_nalloc + 1);
	}

	strcpy(m_pdata + m_nlength, origin.m_pdata);
	m_nlength += origin.m_nlength;
}

void c_string::operator += (const char* p_origin)
{
	if (!p_origin) return;
	int n_str_length = (int)strlen(p_origin);
	if (!n_str_length) return;

	if(m_nlength + n_str_length > m_nalloc)
	{
		m_nalloc  =  m_nlength + n_str_length;
		m_pdata   =  (char*)realloc(m_pdata, m_nalloc + 1);
	}

	strcpy(m_pdata + m_nlength, p_origin);
	m_nlength += n_str_length;
}

void c_string::operator += (const char _char)
{
	if (m_nlength == m_nalloc)
	{
		m_pdata   =  (char*)realloc(m_pdata, m_nalloc + m_ngrow + 1);
		m_nalloc += m_ngrow;
	}

	m_pdata [m_nlength++] = _char;
	m_pdata [m_nlength] = 0;
}

void c_string::operator += (const int val)
{
	if (m_nalloc - m_nlength < m_ngrow)
	{
		m_nalloc += m_ngrow;
		m_pdata   =  (char*)realloc(m_pdata, m_nalloc + 1);
	}

	_itoa(val, m_pdata + m_nlength, 10);
	m_nlength += (int)strlen(m_pdata + m_nlength);
}

void c_string::operator += (const double val)
{
	if (m_nalloc - m_nlength < m_ngrow)
	{
		m_nalloc += m_ngrow;
		m_pdata   =  (char*)realloc(m_pdata, m_nalloc + 1);
	}

	sprintf(m_pdata + m_nlength, "%.20g", val);
	m_nlength += (int)strlen(m_pdata + m_nlength);
}

void c_string::empty()
{
	m_nlength = 0;
	if (m_pdata != 0) m_pdata [0] = 0;
}

c_string::operator LPCTSTR()
{
	if (m_nlength == 0) return "";
	return m_pdata;
}

void c_string::kill(const int n_pos)
{
	if (n_pos < 0) return;
	if (n_pos >= m_nlength) return;

	for (int i = n_pos; i < m_nlength; i++)
		m_pdata[i] = m_pdata[i + 1];

	m_nlength --;
}

void c_string::insert(const char c, const int n_pos)
{
	if (m_nlength == m_nalloc)
	{
		m_pdata   =  (char*)realloc(m_pdata, m_nalloc + m_ngrow + 1);
		m_nalloc += m_ngrow;
	}

	for (int i = m_nlength; i > n_pos; i--)
		m_pdata [i] = m_pdata [i - 1];

	m_pdata [m_nlength + 1] = 0;

	m_pdata [n_pos] = c;
	m_nlength ++;
}

void c_string::copy(const char* p_origin, int n_length)
{
	if (p_origin == 0 || *p_origin == 0 || n_length == 0)
	{
		empty();
		return;
	}

	int n_str_length = (int)strlen(p_origin);
	if (n_str_length > n_length) n_str_length = n_length;

	if(m_nalloc < n_str_length)
	{
		m_pdata   = (char*)realloc(m_pdata, n_str_length + 1);
		m_nalloc  = n_str_length;
	}
	m_nlength = n_str_length;

	strncpy(m_pdata, p_origin, n_str_length);
	m_pdata[n_str_length] = 0;
}

void c_string::append(const char* p_origin, int n_length)
{
	int n_str_length = (int)strlen(p_origin);
	if (n_str_length > n_length) n_str_length = n_length;

	if(m_nalloc < n_str_length + m_nlength)
	{
		m_pdata   = (char*)realloc(m_pdata, n_str_length + m_nlength + 1);
		if (!m_nlength) m_pdata[0] = 0;
		m_nalloc  = n_str_length + m_nlength;
	}
	m_nlength += n_str_length;

	strncat(m_pdata, p_origin, n_str_length);
	//m_pdata[m_nlength] = 0;
}

void c_string::l_trim()
{
	if (m_nlength == 0) return;

	char* p_first = m_pdata;

	while (*p_first == ' ') p_first ++;

	if (p_first == m_pdata) return;

	char* p = m_pdata;

	while (*p_first != 0) *p++ = *p_first++;

	*p = 0;
	m_nlength = int(p - m_pdata);
}

void c_string::r_trim()
{
	if (m_nlength == 0) return;

	for (char* p = m_pdata + m_nlength - 1; p >= m_pdata; p --)
	{
		if (*p != ' ') break;
		*p = 0;
		m_nlength --;
	}
}

void c_string::u_case()
{
	if (m_nlength == 0) return;
	_strupr(m_pdata);
}

void c_string::l_case()
{
	if (m_nlength == 0) return;
	_strlwr(m_pdata);
}

char c_string::get_last()
{
	if (m_nlength == 0) return 0;
	return m_pdata[m_nlength - 1];
}

BSTR c_string::sys_alloc_string()
{
	int n_len = MultiByteToWideChar(CP_ACP, 0, m_pdata, m_nlength, NULL, NULL);
	BSTR bstr = ::SysAllocStringLen(NULL, n_len);
	_ASSERT(bstr);
	MultiByteToWideChar(CP_ACP, 0, m_pdata, m_nlength, bstr, n_len);
	return bstr;
}

void c_string::formatV(const char* p_format, va_list argList)
{
	empty();

	const char* p;
	const char* p_start = p_format;

	for (p = p_format; *p; p++)
	{
		if (*p != '%') continue;
		p++;

		switch (*p)
		{
		case '%':
			append(p_start, (int)(p - p_start - 1));
			p_start = p + 1;
			break;

		case 's':
			{
				append(p_start, (int)(p - p_start - 1));
				const char* p_arg = va_arg(argList, const char*);
				(*this) += p_arg;
				p_start = p + 1;
			}
			break;

		case 'd':
			{
				append(p_start, (int)(p - p_start - 1));
				int n = va_arg(argList, int);
				(*this) += n;
				p_start = p + 1;
			}
			break;

		default: _ASSERT(0);
		}
	}

	if (p != p_start) (*this) += p_start;
}

void c_string::format_error(bool b_wsa)
{
	int n = b_wsa ? WSAGetLastError() : GetLastError();

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		n,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL);

	*this = (const char*)lpMsgBuf;

	LocalFree(lpMsgBuf);
}

void c_string::format(const char* p_format, ...)
{
	_ASSERT(p_format);

	va_list argList;
	va_start(argList, p_format);
	formatV(p_format, argList);
	va_end(argList);
}
