#pragma once
#include "c_atom_table.h"
#include "c_call_stack.h"
#include "c_expression.h"
#include "c_variable.h"

class c_for_atom  :  public c_atom
{
	c_expression* m_pinit_expr;					// expression that defines init value
	c_expression* m_pfinal_expr;				// expression that defines final value
	c_expression* m_pstep_expr;					// expression that defines step value

	c_variable	m_final_val;					// final value of the loop c_variable
	c_variable	m_step_val;						// step value
	c_var_desc	m_var_desc;
	bool		m_bfirst_pass;
	bool		m_bcount_down;
public:
	c_for_atom(	c_atom_table* ptable,
				c_call_stack* p_call_stack,
				c_engine* p_engine,
				int nline);

	virtual ~c_for_atom();

	c_atom*		m_ploop;						// first atom inside loop
	c_atom*		m_pexit;						// first atom after loop

	void set_var_name(LPCTSTR varname);
	void set_init_expr (c_expression* pinit_expr);
	void set_final_expr(c_expression* pfinal_expr);
	void set_step_expr (c_expression* pstep_expr);

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif

	friend class c_exit_atom;
};

