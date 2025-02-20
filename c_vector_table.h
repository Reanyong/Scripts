#ifndef C_VECTOR_TABLE_H
#define C_VECTOR_TABLE_H

#include "c_atom_table.h"

class c_vector_table
{
	c_atom***	m_pvectors;
	int			m_nvectors;
	int			m_nalloc;
	int			m_ngrow;

	void		grow();

public:
	c_vector_table();
	virtual ~c_vector_table();

	void		push(c_atom** vector);
	c_atom**	pop();
	void		empty();
	void		append(c_vector_table* porigin);

	void		operator = (c_vector_table& origin);
};

#endif