#include "pch.h"

#include <crtdbg.h>
#include "c_dim_atom.h"

c_dim_atom::c_dim_atom(c_atom_table* ptable,
					   c_call_stack* p_call_stack,
					   c_engine* p_engine,
					   int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type			= atom_type::dim_atom;
	m_pcall_stack	= p_call_stack;
	m_pnext			= 0;
}

c_dim_atom::~c_dim_atom()
{
}
/*
void c_dim_atom::add(LPCTSTR p_name, VARENUM type, unsigned short reserved)
{
	c_dim_entry e;

	e.m_name		= p_name;
	e.m_type		= type;
	e.m_reserved	= reserved;

	m_entries.add(e);
}
*/
void c_dim_atom::exec_()
{
	_ASSERT(this != m_pnext);			// infinite spin

	for (int i = 0; i < m_entries.get_size(); i++)
	{
		m_pcall_stack->add(m_entries.get_ptr(i));
	}

	m_ptable->set_cur_atom(m_pnext);
}

void c_dim_atom::prepare_func_helper()
{
}

void c_dim_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
}

#ifdef _DEBUG
void c_dim_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tDIM\n\
this =\t\t%llX\n\
m_pnext =\t%llX\n\
line =\t\t%d\n",
(UINT64)this,
(UINT64)m_pnext,
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_dim_atom::assert_valid()
{
	_ASSERT(m_entries.get_size());

	c_atom::assert_valid();
}
#endif
