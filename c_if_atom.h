#ifndef C_IF_ATOM_H
#define C_IF_ATOM_H

#include "c_atom_table.h"
#include "c_expression.h"

class c_if_atom  :  public c_atom
{
public:
	c_if_atom(	c_atom_table* ptable,
				c_call_stack* p_stack,
				c_engine* p_engine,
				int nline);

	virtual ~c_if_atom();

	c_atom* m_ptrue;				// atom to jump if condition is true
	c_atom* m_pfalse;				// atom to jump if condition is false
	c_expression* m_pexpression;	// condition

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

#endif