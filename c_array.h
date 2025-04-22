#ifndef C_ARRAY_H
#define C_ARRAY_H

#include "new.h"
#include <crtdbg.h>

#ifndef NO_INDEX
#define NO_INDEX (-3)
#endif

template <class T> class c_array
{
	T*	m_pdata;
	int	m_nsize;
	int	m_nalloc;
public:
	c_array();
	~c_array();

	void		free_extra();
	void		reset(bool b_free = false);

	void		alloc(int n_size);
	T*			insert(T& element, int n_pos);
	T*			add(T& element);
	void		add_const(const T element);
	inline T&	operator[] (int n);
	inline T&	get(int n);
	T&			get_last() {return get(m_nsize - 1);}
	inline int	get_size();
	void		remove_at(int n, int n_count = 1);
	void		remove_last() {remove_at(m_nsize - 1, 1);}

	int			find(T& element);
	T*			get_data() {return m_pdata;}
	inline T*	get_ptr(int n);
	inline T*	get_last_ptr();
	T*			add_new();
	void		swap(int n1, int n2);
	bool		contains(T& element);
};

//-----------------------------------------------------------------------------
// construction/destruction ---------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
c_array<T>::c_array()
{
	m_pdata		= 0;
	m_nsize		= 0;
	m_nalloc	= 0;
}

template <class T>
c_array<T>::~c_array()
{
	if (m_pdata) reset(true);
}

//-----------------------------------------------------------------------------
// reset ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_array<T>::reset(bool b_free)
{
	int i;
	for (i = 0; i < m_nsize; i++)
		m_pdata[i].~T();

	if (b_free)
	{
		if (m_pdata) free(m_pdata);
		m_pdata = 0;
		m_nalloc = 0;
	}
	m_nsize = 0;
}

//-----------------------------------------------------------------------------
// free_extra -----------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_array<T>::free_extra()
{
	if (m_nalloc = m_nsize) return;

	if (m_nsize)
	{
		_expand(m_pdata, sizeof(T) * m_nsize);
		m_nalloc = m_nsize;
	}
	else
	{
		if (m_pdata) free (m_pdata);
		m_pdata = 0;
		m_nalloc = 0;
	}
}

//-----------------------------------------------------------------------------
// alloc ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_array<T>::alloc(int n_size)
{
	// shrink

	if (n_size < m_nsize)
	{
		remove_at(n_size, m_nsize - n_size);
		return;
	}

	// expand

	if (m_nalloc < n_size)
		m_pdata = (T*) realloc(m_pdata, sizeof(T) * (n_size));

	for (int i = m_nsize; i < n_size; i++)
		::new (&m_pdata[i]) T;

	m_nalloc	= n_size;
	m_nsize		= n_size;
}

//-----------------------------------------------------------------------------
// insert ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
T* c_array<T>::insert(T& element, int n_pos)
{
	_ASSERT(n_pos >= 0 && n_pos <= m_nsize);

	if (m_nsize == m_nalloc)
	{
		int n_grow = __max(10, m_nsize / 3);

		m_pdata = (T*) realloc(m_pdata, sizeof(T) * (m_nalloc + n_grow));
		_ASSERT(m_pdata);
		m_nalloc += n_grow;
	}

	memmove(&m_pdata[n_pos + 1], &m_pdata[n_pos], sizeof(T) * (m_nsize - n_pos));

	T* p_ret = ::new (&m_pdata[n_pos]) T;
	m_pdata[n_pos] = element;
	m_nsize ++;
	return p_ret;
}

//-----------------------------------------------------------------------------
// add ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
T* c_array<T>::add(T& element)
{
	if (m_nsize == m_nalloc)
	{
		int n_grow = __max(10, m_nsize / 3);
		T* tmp = (T*)realloc(m_pdata, sizeof(T) * (m_nalloc + (INT64)n_grow));
		m_pdata = tmp;
		_ASSERT(m_pdata);
		m_nalloc += n_grow;
	}

	T* p_ret = ::new (&m_pdata[m_nsize]) T;
	m_pdata[m_nsize] = element;
	m_nsize ++;
	return p_ret;
}

//-----------------------------------------------------------------------------
// add_const ------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_array<T>::add_const(const T element)
{
	if (m_nsize == m_nalloc)
	{
		int n_grow = __max(10, m_nsize / 3);
		T* tmp = (T*)realloc(m_pdata, sizeof(T) * (m_nalloc + (INT64)n_grow));
		m_pdata = tmp;
		_ASSERT(m_pdata);
		m_nalloc += n_grow;
	}

	::new (&m_pdata[m_nsize]) T;
	m_pdata[m_nsize] = element;
	m_nsize ++;
}

//-----------------------------------------------------------------------------
// get ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
inline T& c_array<T>::get(int n)
{
	_ASSERT(n >= 0 && n < m_nsize);
	return m_pdata[n];
}

//-----------------------------------------------------------------------------
// get_ptr --------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
inline T* c_array<T>::get_ptr(int n)
{
	_ASSERT(n >= 0 && n < m_nsize);
	return &m_pdata[n];
}

//-----------------------------------------------------------------------------
// get_last_ptr ---------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
inline T* c_array<T>::get_last_ptr()
{
	_ASSERT(m_nsize);
	return &m_pdata[m_nsize - 1];
}

//-----------------------------------------------------------------------------
// operator[] -----------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
inline T& c_array<T>::operator[] (int n)
{
	return get(n);
}

//-----------------------------------------------------------------------------
// get_size -------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
inline int c_array<T>::get_size()
{
	return m_nsize;
}

//-----------------------------------------------------------------------------
// remove_at ------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_array<T>::remove_at(int n, int n_count)
{
	int n_to_move;
	int i;

	if (!n_count) return;

	_ASSERT(n_count > 0);
	_ASSERT(n >= 0);
	_ASSERT(n + n_count <= m_nsize);

	for (i = n; i < n + n_count; i++)
		m_pdata[i].~T();

	n_to_move = m_nsize - (n + n_count);
	_ASSERT(n_to_move >= 0);

	if (n_to_move) memmove(	&m_pdata[n],
							&m_pdata[n + n_count],
							sizeof(T) * n_to_move);

	m_nsize -= n_count;
}

//-----------------------------------------------------------------------------
// find -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
int c_array<T>::find(T& element)
{
	register int i;
	for (i = 0; i < m_nsize; i++)
		if (element == m_pdata[i]) return i; // T must have operator ==

	return NO_INDEX;
}

//-----------------------------------------------------------------------------
// add_new --------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
T* c_array<T>::add_new()
{
	if (m_nsize == m_nalloc)
	{
		int n_grow = __max(10, m_nsize / 3);
		T* tmp = (T*)realloc(m_pdata, sizeof(T) * (m_nalloc + (INT64)n_grow));
		m_pdata = tmp;
		_ASSERT(m_pdata);
		m_nalloc += n_grow;
	}

	T* p_ret = ::new (&m_pdata[m_nsize]) T;
	m_nsize ++;
	return p_ret;
}

//-----------------------------------------------------------------------------
// swap -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_array<T>::swap(int n1, int n2)
{
	_ASSERT(n1 >= 0 && n1 < m_nsize);
	_ASSERT(n2 >= 0 && n2 < m_nsize);

	T* p = (T*)malloc(sizeof(T));
	if (p!=NULL)
	{
		memcpy(p, &m_pdata[n1], sizeof(T));
		memcpy(&m_pdata[n1], &m_pdata[n2], sizeof(T));
		memcpy(&m_pdata[n2], p, sizeof(T));
		free(p);
	}

}

//-----------------------------------------------------------------------------
// contains -------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
bool c_array<T>::contains(T& element)
{
	register int i;
	for (i = 0; i < m_nsize; i++)
	{
		// T must provide operator ==
		if (m_pdata[i] == element) return true;
	}

	return false;
}
#endif