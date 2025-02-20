#include "pch.h"

#include <crtdbg.h>
#include "c_if_atom.h"

c_if_atom::c_if_atom(c_atom_table* ptable,
					 c_call_stack* p_call_stack,
					 c_engine* p_engine,
					 int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type			= atom_type::if_atom;
	m_ptrue			= 0;
	m_pfalse		= 0;
	m_pexpression	= 0;
}

c_if_atom::~c_if_atom()
{
	if (m_pexpression) delete m_pexpression;
}

void c_if_atom::exec_()
{
	_ASSERT(this != m_ptrue);				// infinite spin
	_ASSERT(this != m_pfalse);

	c_variable v;
	m_pexpression->exec(&v);

	if (!v)
		m_ptable->set_cur_atom(m_pfalse);	// jump to false
	else
		m_ptable->set_cur_atom(m_ptrue);	// jump to true
}

void c_if_atom::prepare_func_helper()
{
	_ASSERT(m_pexpression);
	m_func_caller.from_expr(m_pexpression);
}

void c_if_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
	p_expressions->add(m_pexpression);
}

#ifdef _DEBUG
void c_if_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tif\n\
this =\t\t%llX\n\
m_ptrue = \t%llX\n\
m_pfalse = \t%llX\n\
m_pexpression =\t%llX\n\
line =\t\t%d\n",
(UINT64)this,
(UINT64)m_ptrue,
(UINT64)m_pfalse,
(UINT64)m_pexpression,
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_if_atom::assert_valid()
{
	_ASSERT(m_pexpression);
	m_pexpression->assert_valid();

	if (m_ptrue) m_ptrue->assert_valid();
	if (m_pfalse) m_pfalse->assert_valid();

	c_atom::assert_valid();
}
#endif
