#include "pch.h"

#include <crtdbg.h>
#include "c_while_atom.h"

c_while_atom::c_while_atom(c_atom_table* ptable,
						   c_call_stack* p_call_stack,
						   c_engine* p_engine,
						   int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type	= atom_type::while_atom;
	m_pexpr = 0;
	m_ploop = 0;
	m_pexit = 0;
}

c_while_atom::~c_while_atom()
{
	if (m_pexpr) delete m_pexpr;
}

void c_while_atom::set_expr(c_expression* pexpr)
{
	_ASSERT(pexpr);
	_ASSERT(!m_pexpr);

	m_pexpr = pexpr;
}

void c_while_atom::exec_()
{
	_ASSERT(m_pexpr != 0);

	if (this == m_ploop)		// empty loop. exit immediately
	{
		m_ptable->set_cur_atom(m_pexit);
		return;
	}

	c_variable v;
	m_pexpr->exec(&v);

	if (!v)
	{
		m_ptable->set_cur_atom(m_pexit);
	}
	else
	{
		m_ptable->set_cur_atom(m_ploop);
	}
}

void c_while_atom::prepare_func_helper()
{
	if (m_pexpr) m_func_caller.from_expr(m_pexpr);
}

void c_while_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
	p_expressions->add(m_pexpr);
}

#ifdef _DEBUG
void c_while_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\twhile\n\
this =\t\t%llX\n\
m_ploop = \t%llX\n\
m_pexit = \t%llX\n\
m_pexpr = \t%llX\n\
line =\t\t%d\n",
(UINT64)this,
(UINT64)m_ploop,
(UINT64)m_pexit,
(UINT64)m_pexpr,
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_while_atom::assert_valid()
{
	_ASSERT(m_pexpr);
	m_pexpr->assert_valid();

	if (m_ploop) m_ploop->assert_valid();
	if (m_pexit) m_pexit->assert_valid();

	c_atom::assert_valid();
}
#endif
