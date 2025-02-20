#pragma once
#include "c_expression.h"
#include "c_variable.h"
#include "extensions.h"

class c_engine;

class c_ext_func
{
	c_expressions		m_expressions;					// evaluate these expressions to obtain values to pass to the function
	PEXTENSION_FUNCTION_FUNCTION m_pfunction;			// function to pass values

	c_call_stack*		m_pcall_stack;
	c_engine*			m_pengine;
public:
	c_ext_func(c_call_stack* p_call_stack, c_engine* p_engine);
	~c_ext_func();

	void add_expr(c_expression* p_expr);
	void set_function(PEXTENSION_FUNCTION_FUNCTION p_function);

	void exec(c_variable* p_result);

	friend class c_func_caller;
};

