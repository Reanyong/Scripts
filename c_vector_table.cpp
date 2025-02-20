#include "pch.h"

#include "c_vector_table.h"

c_vector_table::c_vector_table()
{
	m_pvectors	= 0;
	m_nvectors	= 0;
	m_nalloc	= 0;
	m_ngrow		= 50;
}

c_vector_table::~c_vector_table()
{
	if (m_pvectors != 0) free(m_pvectors);
}

void c_vector_table::grow()
{
	m_pvectors = (c_atom***)realloc(m_pvectors, sizeof(c_atom**) * (m_nalloc + m_ngrow));

	m_nalloc += m_ngrow;
}

void c_vector_table::push(c_atom** vector)
{
	if (m_nvectors == m_nalloc) grow();
	m_pvectors[m_nvectors ++] = vector;
}

c_atom** c_vector_table::pop()
{
	if (m_nvectors > 0)
		return m_pvectors[-- m_nvectors];
	else return 0;
}

void c_vector_table::operator = (c_vector_table& origin)
{
	if (m_pvectors != 0)
	{
		m_pvectors = 0;
		free(m_pvectors);
	}
	m_nvectors	= 0;
	m_nalloc	= 0;

	for (int i = 0; i < origin.m_nvectors; i++)
		push(origin.m_pvectors[i]);
}

void c_vector_table::empty()
{
	m_nvectors	= 0;
}

void c_vector_table::append(c_vector_table* porigin)
{
	for (int i = 0; i < porigin->m_nvectors; i++)
		push(porigin->m_pvectors[i]);
}