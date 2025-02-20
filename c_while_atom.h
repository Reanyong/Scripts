#ifndef C_WHILE_ATOM_H
#define C_WHILE_ATOM_H

#include "c_atom_table.h"
#include "c_expression.h"
#include "c_variable.h"

class c_while_atom : public c_atom  
{
	c_expression* m_pexpr;						// condition
public:
	c_while_atom(c_atom_table* ptable,
				 c_call_stack* p_call_stack,
				 c_engine* p_engine,
				 int nline);

	virtual ~c_while_atom();

	c_atom*		m_ploop;						// first atom inside loop
	c_atom*		m_pexit;						// first atom after loop

	void set_expr(c_expression* pexpr);

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

#endif