#pragma once
#include "c_atom_table.h"
#include "c_variable.h"

class c_goto_atom : public c_atom  
{
public:
	c_goto_atom(c_atom_table* ptable,
				c_call_stack* p_stack,
				c_engine* p_engine,
				int nline);

	virtual ~c_goto_atom();

	c_string m_label_name;	// label to jump to
	c_string m_func_name;	// function we are inside

	void set_func_name(LPCTSTR p_func_name);
	void set_label_name(LPCTSTR p_label_name);

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};
