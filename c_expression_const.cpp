#include "pch.h"
#include "c_expression_const.h"


c_expression_const::c_expression_const(c_call_stack* p_call_stack,
									   c_atom_table* p_atom_table,
									   c_engine* p_engine,
									   int n)
: c_expression(p_call_stack, p_atom_table, p_engine)
{
	m_constant = n;
}

c_expression_const::c_expression_const(c_call_stack* p_call_stack,
									   c_atom_table* p_atom_table,
									   c_engine* p_engine,
									   double d)
: c_expression(p_call_stack, p_atom_table, p_engine)
{
	m_constant = d;
}

c_expression_const::c_expression_const(c_call_stack* p_call_stack,
									   c_atom_table* p_atom_table,
									   c_engine* p_engine,
									   const char* p)
: c_expression(p_call_stack, p_atom_table, p_engine)
{
	m_constant = p;
}


c_expression_const::~c_expression_const()
{
}


void c_expression_const::exec(c_variable* p_result)
{
	*p_result = m_constant;
}

#ifdef _DEBUG
void c_expression_const::assert_valid()
{
	m_constant.assert_valid();

	c_expression::assert_valid();
}
#endif
