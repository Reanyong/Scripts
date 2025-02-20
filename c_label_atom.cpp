#include "pch.h"

#include <crtdbg.h>
#include "c_label_atom.h"

c_label_atom::c_label_atom(c_atom_table* ptable,
						   c_call_stack* p_call_stack,
						   c_engine* p_engine,
						   int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type			= atom_type::label_atom;
	m_pnext			= 0;
}

c_label_atom::~c_label_atom()
{
}

void c_label_atom::set_func_name(LPCTSTR p_func_name)
{
	m_func_name = p_func_name;
}

void c_label_atom::set_label_name(LPCTSTR p_label_name)
{
	m_label_name = p_label_name;
}

void c_label_atom::exec_()
{
	_ASSERT(this != m_pnext);			// infinite spin
	m_ptable->set_cur_atom(m_pnext);
}

void c_label_atom::prepare_func_helper()
{
}

void c_label_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
}

#ifdef _DEBUG
void c_label_atom::dump(FILE* f)
{ 
	fprintf(f,
"--------------------------------------\n\
type =\t\tlabel\n\
this =\t\t%llX\n\
m_pnext = \t%llX\n\
line =\t\t%d\n",
(UINT64)this,
(UINT64)m_pnext,
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_label_atom::assert_valid()
{
	_ASSERT(m_label_name.get_length());

	c_atom::assert_valid();
}
#endif
