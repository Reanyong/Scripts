#include "pch.h"

#include <crtdbg.h>
#include "c_routine_entry_atom.h"

c_routine_entry_atom::c_routine_entry_atom(c_atom_table* ptable,
										   c_call_stack* p_call_stack,
										   c_engine* p_engine,
										   int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type			= atom_type::routine_entry_atom;
	m_pcall_stack	= p_call_stack;
	m_pnext			= 0;

	m_bis_sub		= true;		// sub by default
}

c_routine_entry_atom::~c_routine_entry_atom()
{
}

void c_routine_entry_atom::set_name(LPCTSTR p_name)
{
	m_var_desc = p_name;
}

void c_routine_entry_atom::pass_args(c_variable* p_args)
{
	m_args.reset(true);

	for (int i = 0; i < m_var_desc_array.get_size(); i++)
		m_args.add(p_args[i]);
}

int c_routine_entry_atom::get_arg_count()
{
	return m_var_desc_array.get_size();
}

void c_routine_entry_atom::exec_()
{
	_ASSERT(this != m_pnext);			// infinite spin
	_ASSERT(m_args.get_size() == m_var_desc_array.get_size());

	for (int i = 0; i < m_var_desc_array.get_size(); i++)	// add all the args to the call stack
	{
		c_dim_entry* p_e = m_dim_entries.get_ptr(i);
		m_pcall_stack->add(p_e);

		if (p_e->m_bbyref)
			m_pcall_stack->set_var(m_var_desc_array.get_ptr(i), m_args.get_ptr(i));
		else
			m_pcall_stack->set_var(m_var_desc_array.get_ptr(i), (c_variable*)m_args[i].get_ptr());
	}

	//if (!m_bis_sub) m_pcall_stack->add(m_var_desc.get_name(), m_dim_entry.m_type, m_dim_entry.m_bbar_array, 0);
	if (!m_bis_sub) m_pcall_stack->add(&m_dim_entry);
	m_ptable->set_cur_atom(m_pnext);

	m_args.reset(true);
	enter_routine();
}

bool c_routine_entry_atom::add_arg(c_dim_entry e)
{
// is it already added? ---------------

	for (int i = 0; i < m_var_desc_array.get_size(); i++)
		if (e.m_name.eq(m_var_desc_array[i].get_name())) return false;

	c_var_desc* p_var_desc = m_var_desc_array.add_new();
	p_var_desc->set_name(e.m_name.get_buffer());

	m_dim_entries.add(e);

	return true;
}

void c_routine_entry_atom::is_sub(bool b)
{
	m_bis_sub = b;
}

bool c_routine_entry_atom::is_sub()
{
	return m_bis_sub;
}

void c_routine_entry_atom::prepare_func_helper()
{
}

void c_routine_entry_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
}

void c_routine_entry_atom::enter_routine()
{
	int i;
	for (i = 0; i < m_body.get_size(); i++)
		m_body[i]->on_enter_routine();
}

void c_routine_entry_atom::exit_routine()
{
	int i;
	for (i = 0; i < m_body.get_size(); i++)
		m_body[i]->on_exit_routine();
}

#ifdef _DEBUG
void c_routine_entry_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tSUB ENTRY\n\
m_name = \t%s\n\
this =\t\t%llX\n\
m_pnext = \t%llX\n\
line =\t\t%d\n",
		m_var_desc.get_name(),
		(UINT64)this,
		(UINT64)m_pnext,
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_routine_entry_atom::assert_valid()
{
	int i;
	for (i = 0; i < m_args.get_size(); i++)
		m_args[i].assert_valid();

	for (i = 0; i < m_body.get_size(); i++)
		m_body[i]->assert_valid();

	c_atom::assert_valid();
}
#endif
