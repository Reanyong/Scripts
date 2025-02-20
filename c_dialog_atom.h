#pragma once
#include "InputDialog.h"
#include "c_atom_table.h"
#include "c_expression.h"
#include "c_variable.h"
#include "c_str_array.h"
#include "c_atom_table.h"

class c_dialog_atom : public c_atom  
{
	c_str_array			m_titles;
	//c_str_array	m_names;
	c_array<c_var_desc>	m_var_desc_array;
	c_string			m_title;

	run_mode	m_prev_run_mode;	// atom_table mode before input

	c_input_dialog* m_pdialog;
public:
	c_dialog_atom(c_atom_table* ptable,
				  c_call_stack* p_call_stack,
				  c_engine* p_engine,
				  int nline,
				  c_input_dialog* p_dialog);

	virtual ~c_dialog_atom();

	void set_title(LPCTSTR p_title);
	void add(LPCTSTR p_title, LPCTSTR p_varname);
	void on_window_enter();

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

