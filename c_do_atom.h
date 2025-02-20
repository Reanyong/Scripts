#pragma once
#include "c_atom_table.h"
#include "c_expression.h"
#include "c_variable.h"

class c_do_atom : public c_atom  
{
	c_expression* m_pdo_expr;					// expression that is next to 'DO'
	c_expression* m_ploop_expr;					// expression that is next to 'LOOP'
	bool m_bis_while;							// true if 'WHILE', false if 'UNTIL'
	bool m_bfirst_pass;							// true if it's first time we entered loop
public:
	c_do_atom(c_atom_table* ptable,
			  c_call_stack* p_stack,
			  c_engine* p_engine,
			  int nline);

	virtual ~c_do_atom();

	bool is_while()				{return m_bis_while;}
	void is_while(bool is_while){m_bis_while = is_while;}

	bool do_expression_present(){return m_pdo_expr != 0;}

	void set_do_expr(c_expression* p_expr);
	void set_loop_expr(c_expression* p_expr);

	c_atom*		m_ploop;						// first atom inside loop
	c_atom*		m_pexit;						// first atom after loop

	virtual void pre_step();
	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};
