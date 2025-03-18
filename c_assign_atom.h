#pragma once
#include "c_atom_table.h"
#include "c_expression.h"
#include "c_variable.h"

class c_assign_atom : public c_atom
{
	c_var_desc					m_var_desc;
	c_ptr_array<c_expression>	m_dims;			// variable dimensions
	c_expression*				m_pexpression;	// expression to be assigned

	enum class type
	{
		_undef,
		_prop,
		_var,
	}	_type;

public:
	c_assign_atom(c_atom_table* ptable,
				  c_call_stack* p_call_stack,
				  c_engine* p_engine,
				  int nline);

	virtual ~c_assign_atom();

	void set_var_name(LPCTSTR p_var_name);
	inline void	add_dim(c_expression* p_expr);

	void set_expression(c_expression* p_expression);

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif

private:
	c_string m_system_path;						// System ��ü ���
	bool m_is_system_object = false;		    // System ��ü ���� �÷���
	bool m_is_get_property;					    // �Ӽ� �� �������� ����
	c_ptr_array<c_expression> m_expr_params;    // ��� �Ķ���� ǥ���ĵ� (�����, ��ü�� ��)

public:
	void set_system_path(const char* path) { m_system_path = path; }
	const char* get_system_path() { return m_system_path.get_buffer(); }

	void is_system_object(bool value) { m_is_system_object = value; }
	bool is_system_object() { return m_is_system_object; }

	void is_get_property(bool value) { m_is_get_property = value; }
	bool is_get_property() { return m_is_get_property; }

	void add_expression_param(c_expression* p_expr) { m_expr_params.add(p_expr); }

	// const ������ ����
	c_expression* get_expression_param(int index) {
		if (index >= 0 && index < m_expr_params.get_size())
			return m_expr_params[index];
		return NULL;
	}

	// const ������ ����
	int get_expression_param_count() {
		return m_expr_params.get_size();
	}
};

// inlines --------------------------------------------------------------------

inline void c_assign_atom::add_dim(c_expression* p_expr)
{
	_ASSERT(p_expr);
	_ASSERT(!m_dims.contains(p_expr));
	m_dims.add(p_expr);
}