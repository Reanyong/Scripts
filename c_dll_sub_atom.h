#ifndef c_dll_sub_atom_H
#define c_dll_sub_atom_H

#include "extensions.h"
#include "c_atom_table.h"
#include "c_expression.h"

class c_engine;
class c_dll_func;

class c_dll_sub_atom : public c_atom  
{
	c_dll_func*			m_pdll_func;
	c_expressions		m_expressions;	// evaluate these expressions to obtain values to pass to the sub

public:
	c_dll_sub_atom(c_atom_table* ptable,
					c_call_stack* p_call_stack,
					c_engine* p_engine,
					int nline);

	virtual ~c_dll_sub_atom();

	inline void set_dll_func(c_dll_func* p_dll_sub);

	void add_expr(c_expression* p_expr);

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

// inlines --------------------------------------------------------------------

inline void c_dll_sub_atom::set_dll_func(c_dll_func* p_dll_func)
{
	_ASSERT(p_dll_func);
	_ASSERT(!m_pdll_func);
	m_pdll_func = p_dll_func;
}

#endif