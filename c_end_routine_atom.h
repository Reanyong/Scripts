#ifndef C_END_ROUTINE_ATOM_H
#define C_END_ROUTINE_ATOM_H

#include "c_atom_table.h"
#include "c_call_stack.h"

class c_routine_entry_atom;

class c_end_routine_atom : public c_atom  
{
	c_routine_entry_atom*	m_pentry;
public:
	c_end_routine_atom(	c_atom_table* ptable,
						c_call_stack* p_call_stack,
						c_engine* p_engine,
						int nline);

	virtual ~c_end_routine_atom();

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

	inline void set_entry(c_routine_entry_atom* p_atom);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

// inlines --------------------------------------------------------------------

inline void c_end_routine_atom::set_entry(c_routine_entry_atom* p_atom)
{
	_ASSERT(!m_pentry);
	_ASSERT(p_atom);

	m_pentry = p_atom;
}

#endif