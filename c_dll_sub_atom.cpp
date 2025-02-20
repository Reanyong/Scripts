#include "pch.h"

#include <crtdbg.h>
#include "c_dll_sub_atom.h"
//#include "c_dll_func.h"
#include "c_engine.h"

c_dll_sub_atom::c_dll_sub_atom(c_atom_table* ptable,
							   c_call_stack* p_call_stack,
							   c_engine* p_engine,
							   int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type				= atom_type::ext_sub_atom;
	m_pdll_func			= 0;
	m_pnext				= 0;
	m_pcall_stack		= p_call_stack;
	m_pengine			= p_engine;
}

c_dll_sub_atom::~c_dll_sub_atom()
{
	m_expressions.delete_items();
}

void c_dll_sub_atom::add_expr(c_expression* p_expr)
{
	_ASSERT(p_expr != 0);
	m_expressions.add(p_expr);
}

void c_dll_sub_atom::exec_()
{
	_ASSERT(m_pdll_func != 0);
	_ASSERT(this != m_pnext);

// calc arguments -------------------------------------------------------------

	c_array<c_variable> args;
	c_expression::prepare_args(&m_expressions, &args, m_pcall_stack, m_pengine, false);

// get argument pointers ------------------------------------------------------

	c_ptr_array<c_variable>	ptr_args;
	get_argument_pointers(ptr_args, args);

// call -----------------------------------------------------------------------

	c_variable result; // not used in our case
	if (!m_pdll_func->exec(ptr_args.get_size(), ptr_args.get_data(), &result))
	{
		m_pengine->runtime_error(CUR_ERR_LINE,
								"runtime error calling '%s' DLL routine.",
								m_pdll_func->get_routine_name());

		m_ptable->set_cur_atom(0);
	}
	else m_ptable->set_cur_atom(m_pnext);
}

void c_dll_sub_atom::prepare_func_helper()
{
	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		m_func_caller.from_expr(m_expressions[i]);
}

void c_dll_sub_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		p_expressions->add(m_expressions[i]);
}

#ifdef _DEBUG
void c_dll_sub_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tDLL SUB\n\
this =\t\t%llX\n\
m_pnext = \t%llX\n\
m_nargs = \t%X\n\
line =\t\t%d\n",
(UINT64)this,
(UINT64)m_pnext,
		m_expressions.get_size(),
		m_nline);

	m_func_caller.dump(f);
}

#endif

#ifdef _DEBUG
void c_dll_sub_atom::assert_valid()
{
	_ASSERT(m_pdll_func);
	m_pdll_func->assert_valid();

	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
		m_expressions[i]->assert_valid();

	c_atom::assert_valid();
}
#endif
