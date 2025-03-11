#ifndef C_PTR_ARRAY_H
#define C_PTR_ARRAY_H

//#include "misc.h"
#include <stdlib.h>	// fealloc, free
#include <crtdbg.h>	// _ASSERT
#include <malloc.h>

template <class T> class c_ptr_array
{
	int		m_nsize;
	int		m_nalloc;
	T**		m_pdata;

	void	grow();
public:
	c_ptr_array();
	~c_ptr_array();

	void free_extra();

	T* get(int i)
	{
		_ASSERT(i >= 0 && i < m_nsize);
		return m_pdata[i];
	}

	T* operator[] (int i)
	{
		return get(i);
	}

	void delete_items();
	void add(T* p);
	void alloc(int n_size);
	void add_new();
	void insert(T* p, int n_pos);
	int get_size() {return m_nsize;}
	void reset();

	void remove_at(int n_pos, bool b_delete = false);
	void remove(T* p);
	void remove_last(bool b_delete = false) {remove_at(m_nsize - 1, b_delete);}

	void set_at(int n_pos, T* p);
	void set_last(T* p);
	bool contains(T* p);
	int  get_index(T* p);
	void swap (int n1, int n2);

	c_ptr_array<T>*	duplicate();
	void			copy(c_ptr_array<T>* origin);

	T*   get_last();
	T**  get_data() {return m_pdata;}
};

//-----------------------------------------------------------------------------
// construction/destruction ---------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
c_ptr_array<T>::c_ptr_array()
{
	m_nsize		= 0;
	m_nalloc	= 0;
	m_pdata		= 0;
}

template <class T>
c_ptr_array<T>::~c_ptr_array()
{
	if (m_pdata) free(m_pdata);
}

template <class T>
void c_ptr_array<T>::delete_items()
{
	int i;
	for (i = 0; i < m_nsize; i++)
	{
		if (m_pdata[i]) delete m_pdata[i];
		m_pdata[i] = 0;
	}
}

//-----------------------------------------------------------------------------
// grow -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::grow()
{
	int n_grow = __max(10, m_nsize / 3);
	T** tmp = (T**)realloc(m_pdata, sizeof(T*) * (m_nalloc + (INT64)n_grow));
	m_pdata = tmp;

	int i;
	for (i = m_nalloc; i < m_nalloc + n_grow; i++)
	{
		if (m_pdata != NULL)
		{
			m_pdata[i] = 0;
		}
	}


	m_nalloc += n_grow;
}

//-----------------------------------------------------------------------------
// alloc -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::alloc(int n_size)
{
	if (m_nalloc >= n_size) return;

	m_pdata = (T**)realloc(m_pdata, sizeof(T*) * (n_size));

	int i;
	for (i = m_nsize; i < n_size; i++)
		m_pdata[i] = 0;

	m_nalloc	= n_size;
	m_nsize		= n_size;
}

//-----------------------------------------------------------------------------
// add_new --------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::add_new()
{
	add(new T);
}

//-----------------------------------------------------------------------------
// reset ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::reset()
{
	m_nsize = 0;
}

//-----------------------------------------------------------------------------
// free_extra -----------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::free_extra()
{
	if (m_nalloc == m_nsize) return;

	if (m_nsize)
	{
		_expand(m_pdata, m_nsize * sizeof(T*));
		m_nalloc = m_nsize;
	}
	else
	{
		if (m_pdata) free(m_pdata);
		m_pdata = 0;
	}
}

//-----------------------------------------------------------------------------
// add ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::add(T* p)
{
	if (m_nsize == m_nalloc) grow();
	m_pdata[m_nsize++] = p;
}

//-----------------------------------------------------------------------------
// insert ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::insert(T* p, int n_pos)
{
	_ASSERT(n_pos >= 0);
	_ASSERT(n_pos <= m_nsize);

	if (m_nsize == m_nalloc) grow();
	memmove(&m_pdata[n_pos + 1], m_pdata[n_pos], m_nsize - n_pos);

	m_pdata[n_pos] = p;

	m_nsize ++;
}

//-----------------------------------------------------------------------------
// remove_at ------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::remove_at(int n_pos, bool b_delete)
{
	_ASSERT(n_pos >= 0);
	_ASSERT(n_pos < m_nsize);

	if (b_delete) delete m_pdata[m_nsize - 1];

	int i;
	for (i = n_pos + 1; i < m_nsize; i++)
		m_pdata[i - 1] = m_pdata[i];

	m_nsize --;
}

//-----------------------------------------------------------------------------
// remove ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::remove(T* p)
{
	int n_index = get_index(p);
	_ASSERT(n_index != NO_INDEX);
	remove_at(n_index);
}

//-----------------------------------------------------------------------------
// set_at ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::set_at(int n_pos, T* p)
{
	_ASSERT(n_pos >= 0);
	_ASSERT(n_pos < m_nsize);

	m_pdata[n_pos] = p;
}

//-----------------------------------------------------------------------------
// set_last -------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::set_last(T* p)
{
	_ASSERT(m_nsize);
	m_pdata[m_nsize - 1] = p;
}

//-----------------------------------------------------------------------------
// contains -------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
bool c_ptr_array<T>::contains(T* p)
{
	register int i;
	for (i = 0; i < m_nsize; i++)
		if (m_pdata[i] == p) return true;

	return false;
}

//-----------------------------------------------------------------------------
// get_last -------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
T* c_ptr_array<T>::get_last()
{
	if (m_nsize == 0) return 0;
	return m_pdata[m_nsize - 1];
}

//-----------------------------------------------------------------------------
// get_index ------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
int c_ptr_array<T>::get_index(T* p)
{
	register int i;
	for (i = 0; i < m_nsize; i++)
		if (p == m_pdata[i]) return i;

	return NO_INDEX;
}

//-----------------------------------------------------------------------------
// copy -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::copy(c_ptr_array<T>* p_origin)
{
	_ASSERT(m_nsize == 0);

	int i;
	for (i = 0; i < p_origin->m_nsize; i++)
		add(p_origin->get(i));
}

//-----------------------------------------------------------------------------
// duplicate ------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
c_ptr_array<T>*	c_ptr_array<T>::duplicate()
{
	c_ptr_array<T>* p_result = new c_ptr_array<T>;

	p_result->copy(this);

	return p_result;
}

//-----------------------------------------------------------------------------
// swap -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

template <class T>
void c_ptr_array<T>::swap (int n1, int n2)
{
	_ASSERT(n1 >= 0 && n1 < m_nsize);
	_ASSERT(n2 >= 0 && n2 < m_nsize);

	T* p = m_pdata[n1];
	m_pdata[n1] = m_pdata[n2];
	m_pdata[n2] = p;
}

#endif
