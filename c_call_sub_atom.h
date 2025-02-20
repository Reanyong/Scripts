#ifndef C_CALL_SUB_ATOM_H
#define C_CALL_SUB_ATOM_H

#include "c_atom_table.h"
#include "c_call_stack.h"
#include "c_routine_entry_atom.h"
#include "c_expression.h"

class c_call_sub_atom : public c_atom  
{
	c_routine_entry_atom*	m_psub_entry;

	c_ptr_array<c_expression>	m_expressions;

public:
	c_call_sub_atom(c_atom_table* ptable,
					c_call_stack* p_call_stack,
					c_engine* p_engine,
					int nline);

	virtual ~c_call_sub_atom();

	c_string	m_sub_name;

	void set_sub_name(LPCTSTR p_sub_name);				// called on parse stage
	void set_sub_entry(c_routine_entry_atom* p_sub_entry);	// called on post-parse stage
	void add_expression(c_expression* p_expression);

	int get_arg_count();								// number of arguments for the sub

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

#endif