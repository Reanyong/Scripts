#include "pch.h"
#include "c_str_array.h"
#include "c_string.h"
#include <ctype.h>
#include <string.h>

//-----------------------------------------------------------------------------
// trim -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void trim(char* p_str)
{
	char* p1 = p_str;
	char* p2 = p_str;

// ommit leading spaces -------------------------------------------------------

	while (*p2)
	{
		if (!isspace(*p2)) break;
		p2 ++;
	}

// shift string ---------------------------------------------------------------

	while(*p2) *p1 ++ = *p2 ++;

// scan back for trailing spaces ----------------------------------------------

	while (p1 >= p_str)
	{
		p1 --;
		if (!isspace(*p1))
		{
			*(p1 + 1) = 0;
			break;
		}
	}

//	*p1 = 0;
}

//-----------------------------------------------------------------------------
// construction/destruction ---------------------------------------------------
//-----------------------------------------------------------------------------

c_str_array::c_str_array()
{
	m_nsize		= 0;
	m_nalloc	= 0;
	m_ngrow		= 100;
	m_pdata		= 0;
}

c_str_array::~c_str_array()
{
	empty();

	if (m_pdata) free(m_pdata);
}

//-----------------------------------------------------------------------------
// grow -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_str_array::grow()
{
	m_pdata = (char**)realloc(m_pdata, sizeof(char*) * (m_nalloc + m_ngrow));
	if (!m_pdata) return false;

	int i;
	for (i = m_nalloc; i < m_nalloc + m_ngrow; i++)
		m_pdata[i] = 0;

	m_nalloc += m_ngrow;
	return true;
}

//-----------------------------------------------------------------------------
// add ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_str_array::add(const char* p)
{
// make sure word is valid ----------------------------------------------------

#ifdef _DEBUG
//	int n_delim = 0;
//	const char* p_str = p;
//
//	while (*p_str)
//		if (is_delim_x(*p_str++)) n_delim ++;
//
//	if (n_delim > 1) ASSERT(0);
#endif

// add this word --------------------------------------------------------------

	if (m_nsize == m_nalloc) if (!grow()) return false;
	
	if (p) m_pdata[m_nsize] = _strdup(p);
	else m_pdata[m_nsize] =0;

#ifdef CASE_INSENSITIVE
	strlwr(m_pdata[m_nsize]);
#endif

	m_nsize++;
	return true;
}

//-----------------------------------------------------------------------------
// empty ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::empty()
{
	int i;
	for (i = 0; i < m_nsize; i++)
		if (m_pdata[i]) free (m_pdata[i]);

	m_nsize = 0;
}

//-----------------------------------------------------------------------------
// remove_empty ---------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::remove_empty()
{
	int i;
	int n = 0;

	for (i = 0; i < m_nsize; i++)
	{
		if (strlen(m_pdata[i]) == 0) n++;
		else if (n)
		{
			m_pdata[i - n] = m_pdata[i];
		}
	}

	m_nsize -= n;
}

//-----------------------------------------------------------------------------
// contains -------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_str_array::contains(const char* p_str, bool b_case)
{
	int i;
	if (b_case)
	{
		for (i = 0; i < m_nsize; i++)
			if (strcmp(p_str, m_pdata[i]) == 0) return true;
	}
	else
	{
		for (i = 0; i < m_nsize; i++)
			if (_stricmp(p_str, m_pdata[i]) == 0) return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// copy -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::copy(c_str_array* p_origin)
{
	empty();

	int i;
	for (i = 0; i < p_origin->get_size(); i++)
		add(p_origin->get(i));
}

//-----------------------------------------------------------------------------
// remove_at ------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::remove_at(int n)
{
	_ASSERT(n >= 0);
	_ASSERT(n < m_nsize);

	if (m_pdata[n]) free(m_pdata[n]);

	int i;
	for (i = n; i < m_nsize - 1; i++)
		m_pdata[i] = m_pdata[i + 1];

	m_nsize --;
}

//-----------------------------------------------------------------------------
// set_at ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::set_at(int n, const char* p)
{
	_ASSERT(n >= 0 && n < m_nsize);

	if (!p) p = "";
	if (m_pdata[n]) free(m_pdata[n]);
	m_pdata[n] = _strdup(p);
}

//-----------------------------------------------------------------------------
// swap -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::swap (int n1, int n2)
{
	_ASSERT(n1 >= 0 && n1 < m_nsize);
	_ASSERT(n2 >= 0 && n2 < m_nsize);
	
	char* p = m_pdata[n1];
	m_pdata[n1] = m_pdata[n2];
	m_pdata[n2] = p;
}

//-----------------------------------------------------------------------------
// split ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::split(const char* p_str, char c_delim, bool b_trim)
{
	empty();

	const char* p1 = p_str;
	const char* p2 = p_str;
	char* buff = 0;

	if (!p_str) return;

	while(*p2)
	{
		if (*p2 == c_delim)
		{
			if (buff) free(buff);
			buff = (char*) malloc(p2 - p1 + 1);

			memcpy(buff, p1, p2 - p1);
			buff[p2 - p1] = 0;

			if (b_trim) trim(buff);
			add(buff);

			p2++;
			p1 = p2;
		}

		if (*p2) p2++;
	}

	if (p1 != p2)
	{
		if (buff) free(buff);
		buff = (char*) malloc(p2 - p1 + 1);

		memcpy(buff, p1, p2 - p1);
		buff[p2 - p1] = 0;

		if (b_trim) trim(buff);
		add(buff);
	}

	if (buff) free(buff);
}

//-----------------------------------------------------------------------------
// split ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::split(const char* p_str, const char* p_delim)
{
	empty();
	if (!p_str || !*p_str) return;
	if (!p_delim || !*p_delim) return;

	const char* p1 = p_str;
	const char* p2 = 0;
	c_string s;

	while (true)
	{
		p2 = strstr(p1, p_delim);
		if (!p2) break;

		s.empty();
		s.copy(p1, (int)(p2 - p1));
		add(s.get_buffer());
		p1 = p2 + strlen(p_delim);
	}

	add(p1);
}

//-----------------------------------------------------------------------------
// split_lines ----------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::split_lines(const char* p_str)
{
	empty();

	int n_length;
	char* p;

	const char* p1 = p_str;
	const char* p2 = p_str;

	while(*p2)
	{
		if (*p2 == '\r') p2++;

		if (*p2 == '\n')
		{
			n_length = (int)(p2 - p1 - 1);

			if (n_length > 0)
			{
				p = (char*)malloc(n_length + 1);
				memcpy(p, p1, p2 - p1);
				p[n_length] = 0;
			}
			else
			{
				p = (char*)malloc(1);
				*p = 0;
			}

			add(p);

			p1 = p2 + 1;
		}

		p2++;
	}

	add(p1);
}

void c_str_array::insert(int n, const char* p)
{
	if (m_nsize == m_nalloc) grow();

	int i;
	for (i = m_nsize; i >= n ; i--)
		m_pdata[i] = m_pdata[i - 1];

	m_pdata[n] = _strdup(p);
	m_nsize ++;
}

//-----------------------------------------------------------------------------
// fill_variant ---------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::fill_variant(VARIANT* p_var)
{
	_ASSERT(p_var);

	if (p_var->vt != VT_EMPTY)
	{
		// reset or init variant before calling this function
		_ASSERT(0);
		return;
	}

	p_var->vt = (VT_ARRAY | VT_BSTR);
	p_var->parray = SafeArrayCreateVector(VT_BSTR, 0, get_size());
	_ASSERT(p_var->parray);
	_ASSERT(p_var->parray->pvData);

	BSTR* p = (BSTR*)p_var->parray->pvData;

	int i;
	for (i = 0; i < get_size(); i++)
	{
		if (get(i) && strlen(get(i)))
		{
			int n_len = MultiByteToWideChar(CP_ACP, 0, get(i), (int)strlen(get(i)), NULL, NULL);
			p[i] = ::SysAllocStringLen(NULL, n_len);
			_ASSERT(p[i]);
			MultiByteToWideChar(CP_ACP, 0, get(i), (int)strlen(get(i)), p[i], n_len);
		}
		else p[i] = SysAllocString(L"");
	}
}

//-----------------------------------------------------------------------------
// fill_safearray -------------------------------------------------------------
//-----------------------------------------------------------------------------

BSTR alloc_BSTR(const char* p);
void c_str_array::fill_safearray(SAFEARRAY** psa)
{
	if (*psa) SafeArrayDestroy(*psa);
	*psa = SafeArrayCreateVector(VT_BSTR, 0, get_size());
	_ASSERT(*psa);

	long i;
	for (i = 0; i < get_size(); i++)
	{
		BSTR* p_bstr = 0;
		HRESULT hr = SafeArrayPtrOfIndex(*psa, &i, (void**)&p_bstr);
		_ASSERT(!FAILED(hr) && p_bstr);
		*p_bstr = alloc_BSTR(get(i));
	}
}

//-----------------------------------------------------------------------------
// from_safearray -------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_str_array::from_safearray(SAFEARRAY* psa)
{
	empty();

// checks ---------------------------------------------------------------------

	if (!psa) return;
	if (!psa->pvData) return;
	if (!psa->rgsabound[0].cElements) return;

	if (psa->cDims != 1)
	{
		_ASSERT(0);
		return;
	}

	if ( !(psa->fFeatures & FADF_BSTR) )
	{
		_ASSERT(0);
		return;
	}

// get ------------------------------------------------------------------------

	BSTR* p_bstr = 0;
	HRESULT hr = SafeArrayAccessData(psa, (void**)&p_bstr);
	_ASSERT(!FAILED(hr));
	_ASSERT(p_bstr);

	c_string tmp;
	int i;
	for (i = 0; (unsigned)i < psa->rgsabound[0].cElements; i++)
	{
		tmp = p_bstr[i];
		add(tmp.get_buffer());
	}

	hr = SafeArrayUnaccessData(psa);
	_ASSERT(!FAILED(hr));
}
