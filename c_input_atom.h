#pragma once
#include "c_atom_table.h"
#include "terminal.h"
#include "c_call_stack.h"
#include "c_variable.h"
#include "c_str_array.h"
#include "c_atom_table.h"

class c_input_atom : public c_atom  
{
	c_string		m_string;						// string before c_variable list

	//c_str_array		m_vars;							// variables to be input
	c_array<c_var_desc>	m_var_desc_array;
	run_mode		m_prev_run_mode;				// atom_table mode before input
public:
	c_input_atom(c_atom_table* ptable,
				 c_call_stack* p_call_stack,
				 c_engine* p_engine,
				 int nline);

	virtual ~c_input_atom();

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

	void set_string(LPCTSTR pstr);
	void add_var(LPCTSTR pvar_name);

	void on_terminal_enter();

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};
