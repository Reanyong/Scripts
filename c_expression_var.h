#ifndef C_EXPRESSION_VAR
#define C_EXPRESSION_VAR

#include "c_expression.h"

class c_expression_var : public c_expression
{
	c_var_desc					m_var_desc;
	c_ptr_array<c_expression>	m_dims;
public:
	c_expression_var(c_call_stack* p_call_stack, c_atom_table* p_atom_table, c_engine* p_engine);

	virtual ~c_expression_var();

	virtual void		exec(c_variable* p_result);
	inline void			add_dim(c_expression* p_expr);
	inline c_var_desc*	get_var_desc();
	inline bool			is_array();

#ifdef _DEBUG
	virtual void assert_valid();
#endif
};

inline void c_expression_var::add_dim(c_expression* p_expr)
{
	_ASSERT(p_expr);
	_ASSERT(!m_dims.contains(p_expr));
	m_dims.add(p_expr);
}

inline c_var_desc* c_expression_var::get_var_desc()
{
	return &m_var_desc;
}

inline bool c_expression_var::is_array()
{
	return m_dims.get_size() > 0;
}

#endif
