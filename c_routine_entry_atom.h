#ifndef C_ROUTINE_ENTRY_ATOM_H
#define C_ROUTINE_ENTRY_ATOM_H

#include "c_atom_table.h"
#include "c_call_stack.h"
#include "c_str_array.h"
#include "c_array.h"
#include "c_dim_atom.h"

class c_routine_entry_atom : public c_atom  
{
	c_var_desc			m_var_desc;
	c_dim_entry			m_dim_entry;

	c_array<c_var_desc>	m_var_desc_array;
	c_array<c_variable>	m_args;
	c_array<c_dim_entry>m_dim_entries;
	c_ptr_array<c_atom>	m_body;			// atoms inside routine

	bool				m_bis_sub;		// true if sub, false if function

public:
	c_routine_entry_atom(c_atom_table* ptable,
						 c_call_stack* p_call_stack,
						 c_engine* p_engine,
						 int nline);

	virtual ~c_routine_entry_atom();

	void	set_name(LPCTSTR p_name);
	c_var_desc*	get_var_desc() {return &m_var_desc;}

	inline void	set_dim_entry(c_dim_entry* e);

	bool	add_arg(c_dim_entry e);

	inline bool is_byref_arg(int n);

	void pass_args(c_variable* p_args);	// pass argument values
										// before call

	int get_arg_count();				// number of arguments for the sub

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

	void is_sub(bool b);
	bool is_sub();

	void enter_routine();
	void exit_routine();
	inline void add_body(c_atom* p_atom);

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif

	friend class c_atom_table;
};

// inlines --------------------------------------------------------------------

inline bool c_routine_entry_atom::is_byref_arg(int n)
{
	return m_dim_entries[n].m_bbyref;
}

inline void	c_routine_entry_atom::set_dim_entry(c_dim_entry* e)
{
	m_dim_entry = *e;
}

inline void c_routine_entry_atom::add_body(c_atom* p_atom)
{
	_ASSERT(p_atom);
	_ASSERT(p_atom->m_type != atom_type::routine_entry_atom);
	_ASSERT(!m_body.contains(p_atom));
	m_body.add(p_atom);
}

#endif