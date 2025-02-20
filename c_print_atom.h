#ifndef C_PRINT_ATOM_H
#define C_PRINT_ATOM_H

#include "c_atom_table.h"
#include "c_expression.h"
#include "terminal.h"

class c_print_atom : public c_atom  
{
	c_ptr_array<c_expression>	m_expressions;
	c_string					m_string;	// string to be printed

public:
	c_print_atom(c_atom_table* ptable,
				 c_call_stack* p_call_stack,
				 c_engine* p_engine,
				 int nline);

	virtual ~c_print_atom();

	void add(c_expression* pexpression);

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

#endif