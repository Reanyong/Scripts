#include "pch.h"

#include <crtdbg.h>
#include "c_call_sub_atom.h"
#include "c_engine.h"

c_call_sub_atom::c_call_sub_atom(c_atom_table* ptable,
								 c_call_stack* p_call_stack,
								 c_engine* p_engine,
								 int nline)
:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type				= atom_type::call_sub_atom;
	m_psub_entry		= 0;
	m_pnext				= 0;
}

c_call_sub_atom::~c_call_sub_atom()
{
	m_expressions.delete_items();
}

void c_call_sub_atom::set_sub_name(LPCTSTR p_sub_name)
{
	m_sub_name = p_sub_name;
}

void c_call_sub_atom::set_sub_entry(c_routine_entry_atom* p_sub_entry)
{
	m_psub_entry = p_sub_entry;
}

void c_call_sub_atom::add_expression(c_expression* p_expression)
{
	_ASSERT(p_expression != 0);
	m_expressions.add(p_expression);
}

int c_call_sub_atom::get_arg_count()
{
	return m_expressions.get_size();
}

void c_call_sub_atom::exec_()
{
	_ASSERT(m_psub_entry != 0);
	_ASSERT(this != m_pnext);

	c_array<c_variable> args;
	c_expression::prepare_args(&m_expressions, &args, m_pcall_stack, m_pengine, false);

	m_psub_entry->pass_args(args.get_data());	// pass arguments to the sub_entry
	m_pcall_stack->call(m_pnext);				// call_stack goes one level up and stores atom to return to
	m_ptable->set_cur_atom(m_psub_entry);		// next atom to exec is the sub_entry
}

void c_call_sub_atom::prepare_func_helper()
{
	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		m_func_caller.from_expr(m_expressions[i]);
}

void c_call_sub_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		p_expressions->add(m_expressions[i]);
}

#ifdef _DEBUG
void c_call_sub_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tCALL SUB\n\
m_sub_name =\t%s\n\
this =\t\t%llX\n\
m_pnext = \t%llX\t(called by end sub)\n\
m_psub_entry=\t%llX\n\
line =\t\t%d\n",
		(LPSTR)(LPCSTR)m_sub_name,
		(UINT64)this,
		(UINT64)m_pnext,
		(UINT64)m_psub_entry,
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_call_sub_atom::assert_valid()
{
	_ASSERT(m_psub_entry);
	m_psub_entry->assert_valid();

	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		m_expressions[i]->assert_valid();

	c_atom::assert_valid();
}
#endif
