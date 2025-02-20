#ifndef C_EXPRESSION_CONST
#define C_EXPRESSION_CONST

#include "c_expression.h"

class c_expression_const : public c_expression
{
	c_variable m_constant;

public:
	c_expression_const(c_call_stack* p_call_stack, c_atom_table* p_atom_table, c_engine* p_engine, int n);
	c_expression_const(c_call_stack* p_call_stack, c_atom_table* p_atom_table, c_engine* p_engine, double d);
	c_expression_const(c_call_stack* p_call_stack, c_atom_table* p_atom_table, c_engine* p_engine, const char* p);

	virtual ~c_expression_const();

	virtual void exec(c_variable* p_result);

#ifdef _DEBUG
	virtual void assert_valid();
#endif
};

#endif
