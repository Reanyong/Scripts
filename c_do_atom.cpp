#include "pch.h"

#include <crtdbg.h>
#include "c_do_atom.h"

c_do_atom::c_do_atom(c_atom_table* ptable,
					 c_call_stack* p_call_stack,
					 c_engine* p_engine,
					 int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type			= atom_type::do_atom;
	m_pdo_expr		= 0;
	m_ploop_expr	= 0;
	m_bis_while		= true;
	m_bfirst_pass	= true;

	m_ploop			= 0;
	m_pexit			= 0;
}

c_do_atom::~c_do_atom()
{
	if (m_pdo_expr)		delete m_pdo_expr;
	if (m_ploop_expr)	delete m_ploop_expr;
}

void c_do_atom::set_do_expr(c_expression* p_expr)
{
	m_pdo_expr = p_expr;
}

void c_do_atom::set_loop_expr(c_expression* p_expr)
{
	m_ploop_expr = p_expr;
}

void c_do_atom::exec_()
{
	_ASSERT(this != m_pexit);
	_ASSERT(m_ploop != m_pexit);
	_ASSERT(!m_pdo_expr || !m_ploop_expr);				// both 'loop' and 'do' not supported
	c_variable v;

	if (m_bis_while)					// 'while'
	{
		if (m_pdo_expr != 0)			// 'do'
		{
			m_pdo_expr->exec(&v);

			if (v.as_integer())			// if 'do' expression is true
				m_ptable->set_cur_atom(m_ploop);		// go inside loop
			else
				m_ptable->set_cur_atom(m_pexit);		// else exit it
		}
		else if (m_ploop_expr != 0)	// 'loop
		{
			if (m_bfirst_pass)
			{
				m_ptable->set_cur_atom(m_ploop);		// first time we go inside loop unconditionally
				m_bfirst_pass = false;
			}
			else
			{
				m_ploop_expr->exec(&v);

				if (v.as_integer())						// if 'loop' expression is true
					m_ptable->set_cur_atom(m_ploop);	// go inside loop
				else
				{
					m_bfirst_pass = true;
					m_ptable->set_cur_atom(m_pexit);	// else exit it
				}
			}
		}
		else m_ptable->set_cur_atom(m_ploop);			// neither 'loop' nor 'do' - loop forever
	}
	else								// 'until'
	{
		if (m_pdo_expr != 0)			// 'do'
		{
			m_pdo_expr->exec(&v);

			if (v.as_integer())			// if 'do' expression is true
				m_ptable->set_cur_atom(m_pexit);		// exit loop
			else
				m_ptable->set_cur_atom(m_ploop);		// else enter it
		}
		else if (m_ploop_expr != 0)	// 'loop
		{
			if (m_bfirst_pass)
			{
				m_ptable->set_cur_atom(m_ploop);		// first time we go inside loop unconditionally
				m_bfirst_pass = false;
			}
			else
			{
				m_ploop_expr->exec(&v);

				if (v.as_integer())						// if 'loop' expression is true
				{
					m_bfirst_pass = true;
					m_ptable->set_cur_atom(m_pexit);	// exit loop
				}
				else
					m_ptable->set_cur_atom(m_ploop);	// else enter it
			}
		}
		else m_ptable->set_cur_atom(m_ploop);			// neither 'loop' nor 'do' - loop forever
	}
}

void c_do_atom::prepare_func_helper()
{
	if (m_pdo_expr)		m_func_caller.from_expr(m_pdo_expr);
	if (m_ploop_expr)	m_func_caller.from_expr(m_ploop_expr);
}

void c_do_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
	if (m_pdo_expr)		p_expressions->add(m_pdo_expr);
	if (m_ploop_expr)	p_expressions->add(m_ploop_expr);
}

void c_do_atom::pre_step()
{
	if (m_func_caller.get_size())
		m_func_caller.enable(0, !m_bfirst_pass);
}

#ifdef _DEBUG
void c_do_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tDO\n\
this =\t\t%llX\n\
m_ploop = \t%llX\n\
m_pexit = \t%llX\n\
line =\t\t%d\n",
(UINT64)this,
(UINT64)m_ploop,
(UINT64)m_pexit,
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_do_atom::assert_valid()
{
	if (m_pdo_expr) m_pdo_expr->assert_valid();
	if (m_ploop_expr) m_ploop_expr->assert_valid();

	c_atom::assert_valid();
}
#endif
