#ifndef C_EXT_SUB_ATOM_H
#define C_EXT_SUB_ATOM_H

#include "extensions.h"
#include "c_atom_table.h"
#include "c_expression.h"

class c_engine;
class c_ext_sub_atom : public c_atom  
{
	PEXTENSION_SUB_FUNCTION			m_pext_sub;			// sub to call
	PEXTENSION_FUNCTION_FUNCTION	m_pext_function;	// if function called in sub context

	c_expressions					m_expressions;		// evaluate these expressions to obtain values to pass to the sub
public:
	c_ext_sub_atom(	c_atom_table* ptable,
					c_call_stack* p_call_stack,
					c_engine* p_engine,
					int nline);

	virtual ~c_ext_sub_atom();

//	c_engine* m_pengine;

	void set_sub(PEXTENSION_SUB_FUNCTION p_sub);
	void set_function(PEXTENSION_FUNCTION_FUNCTION p_function);

	void add_expr(c_expression* p_expr);

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

#endif