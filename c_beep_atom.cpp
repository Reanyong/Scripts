#include "pch.h"

#include <crtdbg.h>
#include "c_beep_atom.h"

c_beep_atom::c_beep_atom(c_atom_table* ptable,
						 c_call_stack* p_call_stack,
						 c_engine* p_engine,
						 int nline)

:c_atom(ptable, p_call_stack, p_engine, nline)
{
	m_type	= atom_type::beep_atom;
	m_pnext = 0;
}

c_beep_atom::~c_beep_atom()
{
}

void c_beep_atom::exec_()
{
	_ASSERT(this != m_pnext);			// infinite spin
	Beep(1000, 100);
//	MessageBeep(MB_ICONINFORMATION);	// you may prefer this
	m_ptable->set_cur_atom(m_pnext);
}

void c_beep_atom::prepare_func_helper()
{
}

void c_beep_atom::enum_expressions(c_ptr_array<c_expression>* p_expressions)
{
}

#ifdef _DEBUG
void c_beep_atom::dump(FILE* f)
{
	fprintf(f,
"--------------------------------------\n\
type =\t\tBEEP\n\
this =\t\t%llX\n\
m_pnext = \t%llX\n\
line =\t\t%d\n",
(UINT64)this,
(UINT64)m_pnext,
		m_nline);

	m_func_caller.dump(f);
}
#endif
