#include "pch.h"

#include <crtdbg.h>
#include "c_cls_atom.h"
#include "c_engine.h"

c_cls_atom::c_cls_atom(c_atom_table* ptable,
					   c_call_stack* p_call_stack,
					   c_engine* p_engine,
					   int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type			= atom_type::cls_atom;
	m_pnext			= 0;
}

c_cls_atom::~c_cls_atom()
{
}

void c_cls_atom::exec_()
{
	_ASSERT(this != m_pnext);			// infinite spin
	c_terminal* p_terminal = m_pengine->get_terminal();
	p_terminal->clear();
	m_ptable->set_cur_atom(m_pnext);
}

void c_cls_atom::prepare_func_helper()
{
}

void c_cls_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
}

#ifdef _DEBUG
void c_cls_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tCLS\n\
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
void c_cls_atom::assert_valid()
{
	c_atom::assert_valid();
}
#endif
