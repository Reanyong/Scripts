#include "pch.h"

#include <crtdbg.h>
#include "c_exit_atom.h"
#include "c_for_atom.h"

c_exit_atom::c_exit_atom(c_atom_table* ptable,
						 c_call_stack* p_call_stack,
						 c_engine* p_engine,
						 int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type		= atom_type::exit_atom;
	m_pnext		= 0;
	m_exit_type	= exit_type::exit_undef;
	m_pfor		= 0;
}

c_exit_atom::~c_exit_atom()
{
}

void c_exit_atom::exec_()
{
	_ASSERT(m_pnext);
	_ASSERT(this != m_pnext);			// infinite spin
	_ASSERT(m_exit_type != exit_type::exit_undef);

#ifdef _DEBUG
	if (m_exit_type == exit_type::exit_routine)
		_ASSERT(m_pnext->m_type == atom_type::end_routine_atom);
#endif

	if (m_exit_type == exit_type::exit_for)
	{
		_ASSERT(m_pfor);
		m_pfor->m_bfirst_pass = true;
	}

	m_ptable->set_cur_atom(m_pnext);
}

void c_exit_atom::prepare_func_helper()
{
}

void c_exit_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
}

#ifdef _DEBUG
void c_exit_atom::dump(FILE* f)
{
	//20220715 YDK ¼öÁ¤
	fprintf(f,
"--------------------------------------\n\
type =\t\tEXIT\n\
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
void c_exit_atom::assert_valid()
{
	_ASSERT(m_exit_type != exit_type::exit_undef);

	c_atom::assert_valid();
}
#endif
