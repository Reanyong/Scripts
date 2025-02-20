#include "pch.h"
#include "c_expression_var.h"

c_expression_var::c_expression_var(c_call_stack* p_call_stack,
									   c_atom_table* p_atom_table,
									   c_engine* p_engine)
: c_expression(p_call_stack, p_atom_table, p_engine)
{

}

c_expression_var::~c_expression_var()
{
}


void c_expression_var::exec(c_variable* p_result)
{
	if (m_dims.get_size())
	{
		c_variable* p_var;
		m_pcall_stack->get(&m_var_desc, &p_var);
		
		_ASSERT(p_var);
		_ASSERT(p_var->vt == (VT_ARRAY | VT_VARIANT));

		c_variable* p_item = 0;
		get_item(p_var->parray, m_dims.get_data(), m_dims.get_size(), &p_item, m_pengine);
		if (p_item) *p_result = *p_item;
		else p_result->reset();
	}
	else
	{
		c_variable* ptr;
		m_pcall_stack->get(&m_var_desc, &ptr);
		*p_result = *ptr;
	}
}

#ifdef _DEBUG
void c_expression_var::assert_valid()
{
	_ASSERT(m_var_desc.inited());

	c_expression::assert_valid();
}
#endif
