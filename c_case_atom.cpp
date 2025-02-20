#include "pch.h"
#include "c_case_atom.h"
#include <crtdbg.h>

c_case_atom::c_case_atom(c_atom_table* ptable,
						 c_call_stack* p_call_stack,
						 c_engine* p_engine,
						 int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type			= atom_type::case_atom;
	m_pexpression	= 0;
	m_pdef_branch	= 0;
	m_pnext			= 0;
}

c_case_atom::~c_case_atom()
{
	if (m_pexpression) delete m_pexpression;

	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		m_expressions[i].delete_items();
}

void c_case_atom::add_branch(c_atom* p_atom,
							 c_case_expr_ptr_array* p_expressions)
{
	m_branches.add(p_atom);
	c_case_expr_ptr_array* p = m_expressions.add_new();

	int i;
	for (i = 0; i < p_expressions->get_size(); i++)
		p->add(p_expressions->get(i));
}

bool c_case_atom::add_def_branch(c_atom* p_atom)
{
	if (m_pdef_branch) return false;
	m_pdef_branch = p_atom;
	return true;
}

void c_case_atom::exec_()
{
	c_variable v, v1, v2;
	m_pexpression->exec(&v);

	int i, j;
	for (i = 0; i < m_expressions.get_size(); i++)
	{
		c_case_expr_ptr_array& e = m_expressions[i];
		for (j = 0; j < e.get_size(); j++)
		{
			if (e[j]->is_range())
			{
				e[j]->m_e1->exec(&v1);
				e[j]->m_e2->exec(&v2);
				if (v >= v1 && v <= v2)
				{
					m_ptable->set_cur_atom(m_branches[i]);
					return;
				}
			}
			else
			{
				e[j]->m_e1->exec(&v1);
				if (v == v1)
				{
					m_ptable->set_cur_atom(m_branches[i]);
					return;
				}
			}
		}
	}

	if (m_pdef_branch) m_ptable->set_cur_atom(m_pdef_branch);
	else m_ptable->set_cur_atom(m_pnext);
}

void c_case_atom::prepare_func_helper()
{
	_ASSERT(m_pexpression);
	m_func_caller.from_expr(m_pexpression);

	int i, j;
	for (i = 0; i < m_expressions.get_size(); i++)
	{
		for (j = 0; j < m_expressions[i].get_size(); j++)
		{
			c_case_expr* p_case_expr = m_expressions[i].get(j);
			if (p_case_expr->m_e1) m_func_caller.from_expr(p_case_expr->m_e1);
			if (p_case_expr->m_e2) m_func_caller.from_expr(p_case_expr->m_e2);
		}
	}
}

void c_case_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
	_ASSERT(m_pexpression);
	p_expressions->add(m_pexpression);

	int i, j;
	for (i = 0; i < m_expressions.get_size(); i++)
	{
		for (j = 0; j < m_expressions[i].get_size(); j++)
		{
			c_case_expr* p_case_expr = m_expressions[i].get(j);
			if (p_case_expr->m_e1) p_expressions->add(p_case_expr->m_e1);
			if (p_case_expr->m_e2) p_expressions->add(p_case_expr->m_e2);
		}
	}
}

#ifdef _DEBUG
void c_case_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tCASE\n\
this =\t\t%llX\n\
next =\t\t%llX\n\
m_pexpression =\t%llX\n\
line =\t\t%d\n",
(UINT64)this,
(UINT64)m_pnext,
(UINT64)m_pexpression,
		m_nline);

	int i;
	for (i = 0; i < m_branches.get_size(); i++)
	{
		fprintf(f, "branch=\t\t%llX\n", (UINT64)m_branches[i]);
	}
	fprintf(f, "def_branch=\t%llX\n", (UINT64)m_pdef_branch);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_case_atom::assert_valid()
{
	_ASSERT(m_pexpression);
	m_pexpression->assert_valid();

	int i, j;
	for (i = 0; i < m_branches.get_size(); i++)
		m_branches[i]->assert_valid();

	for (i = 0; i < m_expressions.get_size(); i++)
		for (j = 0; j < m_expressions[i].get_size(); j++)
			m_expressions[i][j]->assert_valid();

	if (m_pdef_branch)                                                                        m_pdef_branch->assert_valid();

	c_atom::assert_valid();
}
#endif
