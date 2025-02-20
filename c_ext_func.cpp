#include "pch.h"

#include <crtdbg.h>
#include "c_ext_func.h"
#include "c_engine.h"

c_ext_func::c_ext_func(c_call_stack* p_call_stack, c_engine* p_engine)
{
	m_pfunction		= 0;
	m_pengine		= p_engine;
	m_pcall_stack	= p_call_stack;
}

c_ext_func::~c_ext_func()
{
	m_expressions.delete_items();
}

void c_ext_func::add_expr(c_expression* p_expr)
{
	_ASSERT(p_expr != 0);
	m_expressions.add(p_expr);
}

void c_ext_func::set_function(PEXTENSION_FUNCTION_FUNCTION p_function)
{
	m_pfunction = p_function;
}

void c_ext_func::exec(c_variable* p_result)
{
	_ASSERT(m_pfunction != 0);

// calc arguments

	c_array<c_variable> args;
	c_expression::prepare_args(&m_expressions, &args, m_pcall_stack, m_pengine, false);

// get argument pointers

	c_ptr_array<c_variable>	ptr_args;
	get_argument_pointers(ptr_args, args);

	m_pfunction(ptr_args.get_size(), ptr_args.get_data(), m_pengine, *p_result);	// call the function	

#ifdef _DEBUG
	int i;
	for (i = 0; i < args.get_size(); i++)
	{
		args[i].assert_valid();
	}
#endif
}
