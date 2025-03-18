#pragma once
#include "c_variable.h"
#include "c_ptr_array.h"
#include "c_array.h"

class c_atom;
class c_engine;
struct c_dim_entry;

class c_call_stack
{
	c_ptr_array<c_name_table>	m_name_tables;		// nametable for each level
	c_array<c_atom*>			m_rets;				// returns from routines

	int							m_nlevel;			// current level (0 for globals)

	bool find_var(	const char* p_name,
					int& n_level,
					int& n_id);

	c_engine*					m_pengine;

	c_variable					m_retval;			// result of a function
public:
	c_call_stack(c_engine* p_engine);
	virtual ~c_call_stack();

	void add(c_dim_entry* p);

	bool set_global(const char* p_name, const VARIANT* p_var);
	bool get_global(const char* p_name, VARIANT** pp_var);

	bool call(c_atom* p_ret_atom);				// call routine (one level up)
	bool is_function_context();					// are we inside function or sub?

	c_atom* ret();								// return (one evel down).

	//void get (c_var_desc* p_var_desc, c_variable& result);
	void get (c_var_desc* p_var_desc, c_variable** p_result);

	void set_var(c_var_desc* p_var_desc, c_variable* p_var);

	int get_local_size();						// number of variables at the current level
	int get_global_size();						// number of global vars

	bool get_local_var(	int n,					// get local variable
						c_variable* p_var,
						const char** pp_name);

	bool get_global_var(int n,					// get global variable
						c_variable* p_var,
						const char** pp_name);

	bool get_global_var(const char* p_name,
						c_variable* p_var);

	c_variable*	get_ret_val() {return &m_retval;}

	bool declared(const char* p_name, VARENUM* p_type);
	bool declared_array(const char* p_name);
	void reset();

#ifdef _DEBUG
	void assert_valid();
#endif
};

