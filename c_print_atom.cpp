#include "pch.h"

#include <crtdbg.h>
#include "c_print_atom.h"
#include "c_engine.h"

c_print_atom::c_print_atom(c_atom_table* ptable,
						   c_call_stack* p_call_stack,
						   c_engine* p_engine,
						   int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type			= atom_type::print_atom;
	m_pnext			= 0;
}

c_print_atom::~c_print_atom()
{
	m_expressions.delete_items();
}

void c_print_atom::add(c_expression* pexpression)
{
	_ASSERT(pexpression);
	m_expressions.add(pexpression);
}

void c_print_atom::exec_()
{
	c_variable v;
	c_string item;

	m_string.empty();

	for (int i = 0; i < m_expressions.get_size(); i++)
	{
		v.reset();
		m_expressions[i]->exec(&v);
		v.as_string(item);
		m_string += item;
	}

	// m_variable now contains string in bstrVal.

	c_terminal* p_terminal = m_pengine->get_terminal();
	_ASSERT(p_terminal);
	p_terminal->show();
	p_terminal->append(m_string);

	_ASSERT(this != m_pnext);			// infinite spin
	m_ptable->set_cur_atom(m_pnext);
}

void c_print_atom::prepare_func_helper()
{
	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		m_func_caller.from_expr(m_expressions[i]);
}

void c_print_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		p_expressions->add(m_expressions[i]);
}

#ifdef _DEBUG
void c_print_atom::dump(FILE* f)
{
	fprintf(f, 
"--------------------------------------\n\
type =\t\tprint\n\
this =\t\t%llX\n\
m_pnext = \t%llX\n\
m_nexpressions=\t%d\n\
line =\t\t%d\n",
(UINT64)this,
(UINT64)m_pnext,
		m_expressions.get_size(),
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_print_atom::assert_valid()
{
	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		m_expressions[i]->assert_valid();

	c_atom::assert_valid();
}
#endif
