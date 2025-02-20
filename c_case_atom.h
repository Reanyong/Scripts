#ifndef C_CASE_ATOM_H
#define C_CASE_ATOM_H

#include "c_atom_table.h"
#include "c_expression.h"

// c_case_expr ----------------------------------------------------------------

class c_case_expr
{
public:

	c_expression*		m_e1;
	c_expression*		m_e2;

	c_case_expr()
	{
		m_e1 = 0;
		m_e2 = 0;
	}

	~c_case_expr()
	{
		if (m_e1) delete m_e1;
		if (m_e2) delete m_e2;
	}

	bool is_range()
	{
		return m_e1 && m_e2;
	}

#ifdef _DEBUG
	void assert_valid()
	{
		_ASSERT(m_e1 || m_e2);
		if (m_e1) m_e1->assert_valid();
		if (m_e2) m_e2->assert_valid();
	}
#endif
};

typedef c_ptr_array<c_case_expr> c_case_expr_ptr_array;

// c_case_atom ----------------------------------------------------------------

class c_case_atom  :  public c_atom
{
public:
	c_case_atom(c_atom_table* ptable,
				c_call_stack* p_stack,
				c_engine* p_engine,
				int nline);

	virtual ~c_case_atom();

	c_expression* m_pexpression;	// condition

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

	c_array<c_case_expr_ptr_array>	m_expressions;
	c_ptr_array<c_atom>				m_branches;

	c_atom*							m_pdef_branch;

	inline void						set_expr(c_expression* p_expr);

	void add_branch(c_atom* p_atom,
					c_case_expr_ptr_array* p_expressions);

	bool add_def_branch(c_atom* p_atom);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

//-----------------------------------------------------------------------------

inline void c_case_atom::set_expr(c_expression* p_expr)
{
	_ASSERT(!m_pexpression);
	m_pexpression = p_expr;
}

#endif