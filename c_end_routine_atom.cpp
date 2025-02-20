#include "pch.h"

#include "c_end_routine_atom.h"
#include "c_routine_entry_atom.h"

c_end_routine_atom::c_end_routine_atom(c_atom_table* ptable,
									   c_call_stack* p_call_stack,
									   c_engine* p_engine,
									   int nline)
:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type			= atom_type::end_routine_atom;
	m_pcall_stack	= p_call_stack;
	m_pentry		= 0;
}

c_end_routine_atom::~c_end_routine_atom()
{
}

void c_end_routine_atom::exec_()
{
	_ASSERT(m_pentry);

	// if we are inside sub, just let callstack go one level down
	// and continue execution
	if (!m_pcall_stack->is_function_context())
	{
		c_atom* p_next = m_pcall_stack->ret();		// call_stack goes level down and returns atom to execute next
		m_ptable->set_cur_atom(p_next);
	}
	// if we are inside function, make c_atom_table::run_ exit
	// and c_atom_table::call_function will go level down
	else m_ptable->set_cur_atom(0);

	_ASSERT(m_pentry);
	m_pentry->exit_routine();
}

void c_end_routine_atom::prepare_func_helper()
{
}

void c_end_routine_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
}

#ifdef _DEBUG
void c_end_routine_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tEND SUB\n\
this =\t\t%llX\n\
line =\t\t%d\n",
(UINT64)this,
		m_nline);

	m_func_caller.dump(f);
}
#endif

#ifdef _DEBUG
void c_end_routine_atom::assert_valid()
{
	_ASSERT(m_pentry);
	m_pentry->assert_valid();

	c_atom::assert_valid();
}
#endif
