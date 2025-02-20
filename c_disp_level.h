#pragma once
#include "c_variable.h"
#include "c_ptr_array.h"
#include "c_expression.h"

class c_disp_level
{
	c_call_stack*		m_pcall_stack;
	c_expressions		m_expressions;	// arguments of function

	FUNCDESC			m_funcdesc;

	c_expression*		m_passign_expr;

	c_string			m_member_name; // todo :delete

public:
	c_disp_level(FUNCDESC* p_fd, c_call_stack* p_call_stack);
	~c_disp_level();

	inline void		set_member_name(const char* p_name);
	inline const char* get_member_name();

	inline void		set_assign_expr(c_expression* p_expr);

	bool		can_be_used_in_set();

	bool		get(LPDISPATCH lpd,
					c_variable* p_result,
					c_engine* p_engine,
					int n_line);

	bool		set(LPDISPATCH lpd,
					c_engine* p_engine,
					int n_line);

	inline void add_expr(c_expression* p_expr);

	inline bool is_sub();

#ifdef _DEBUG
	void assert_valid();
#endif

	friend class c_disp_set_atom;
	friend class c_func_caller;
};

// inlines --------------------------------------------------------------------

inline void c_disp_level::set_member_name(const char* p_name)
{
	_ASSERT(!m_member_name.get_length());
	m_member_name = p_name;
}

inline const char* c_disp_level::get_member_name()
{
	return m_member_name.get_buffer();
}

inline void c_disp_level::set_assign_expr(c_expression* p_expr)
{
	_ASSERT(m_expressions.get_size() == 0);
	_ASSERT(!m_passign_expr);
	m_passign_expr = p_expr;
}

inline void c_disp_level::add_expr(c_expression* p_expr)
{
	m_expressions.add(p_expr);
}

inline bool c_disp_level::is_sub()
{
	return m_funcdesc.invkind == DISPATCH_METHOD &&
		m_funcdesc.elemdescFunc.tdesc.vt == VT_VOID;
}

typedef c_ptr_array<c_disp_level> c_disp_levels;

