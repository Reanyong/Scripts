#ifndef C_ATOM_TABLE_H
#define C_ATOM_TABLE_H

#include "c_atom.h"

//-----------------------------------------------------------------------------
// c_atom_table ---------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_routine_entry_atom;
class c_end_routine_atom;
class c_call_stack;
class c_engine;

class c_atom_table
{
	c_atom*				m_pentry_atom;
	c_atom*				m_pcur_atom;		// current atom
	c_ptr_array<c_atom>	m_atoms;
	c_call_stack*		m_pcall_stack;

	int				m_nlevel;		// number of routines we are in when stepping over
	run_mode		m_run_mode;
	int				m_nto_line;		// used with 'runt_to_line' mode
	c_engine*		m_pengine;
	bool			m_bwaiting;		// witing for event
	bool			m_bforce_stop;

	void run_();					// main loop where atoms are executed

	bool find_entry_atom();
public:
	inline bool run_go();
	inline bool step_over();
	inline bool step_into();
	inline bool step_out();
	inline bool run_to_cursor(int n_line);
	void stop();

	c_atom_table(c_call_stack* p_call_stack, c_engine* p_engine);
	virtual ~c_atom_table();

	bool	m_binput;				// input operation

	int		get_count();			// get count of items
	c_atom*	get_atom(int n_atom);
	void	add(c_atom* patom);

	void	run();					// validates stuff and calls "run_"

	void	reset();
	void	free_all();

	c_routine_entry_atom*	get_routine_entry(LPCTSTR p_sub_name, bool b_is_sub);
	c_end_routine_atom*		get_matching_endsub(c_routine_entry_atom* p_sub_entry);

	c_variable call_function(
		c_routine_entry_atom* p_atom,
		c_variable* p_variable,
		int n_variable);

	void prepare_function_callers(int n_first_atom = 0);

	bool	has_code_parsed()	{return m_atoms.get_size() != 0;}
	bool	ready();
	bool	is_waiting()		{return m_pcur_atom != 0;}
	int		get_cur_line();

	void set_cur_atom(c_atom* p_atom);
	bool is_empty();

	const char* get_routine_name();	// name of the routine we are in
	c_atom*		get_routine_atom();
	c_atom*		find_prev_atom(atom_type type, int n_line);

	bool	is_parsing_sub();
	bool	is_parsing_function();
	bool	is_parsing_for();
	bool	is_parsing_while();
	bool	is_parsing_do();

	run_mode get_run_mode()		{return m_run_mode;}

// breakpoints ----------------------------------------------------------------

	int  set_breakpoint(int n_line);								// returns line
	int  remove_breakpoint(int n_line);								// returns line
	int  toggle_breakpoint(int n_line, bool* p_bset = 0);		// returns line
	int  get_breakpoint_count();
	int  get_breakpoint(int n_index);
	void remove_all_breakpoints();
	bool set_bp_condition(int n_line, c_expression* p_expr);

//-----------------------------------------------------------------------------
public:
#ifdef _DEBUG
	void dump();
#endif

	friend class c_engine;
	friend class c_atom;
};

// inlines --------------------------------------------------------------------

inline void c_atom_table::set_cur_atom(c_atom* p_atom)
{
	m_pcur_atom = p_atom;
}

inline bool c_atom_table::is_empty()
{
	return m_atoms.get_size() == 0;
}

inline bool c_atom_table::run_go()
{
	if (m_atoms.get_size() == 0)
		return false;	// source was not parsed

	m_run_mode = run_mode::run_run;
	run();

	return true;
}

inline bool c_atom_table::step_over()
{
	if (m_atoms.get_size() == 0)
		return false;	// source was not parsed

	if (m_pcur_atom && m_pcur_atom->m_type == atom_type::routine_entry_atom)
		m_run_mode = run_mode::run_step_into;
	else
		m_run_mode = run_mode::run_step_over;
	run();

	return true;
}

inline bool c_atom_table::step_into()
{
	if (m_atoms.get_size() == 0)
		return false;	// source was not parsed

	m_run_mode = run_mode::run_step_into;
	run();

	return true;
}

inline bool c_atom_table::step_out()
{
	if (m_atoms.get_size() == 0)
		return false;	// source was not parsed

	m_run_mode = run_mode::run_step_out;
	run();

	return true;
}

inline bool c_atom_table::run_to_cursor(int n_line)
{
	if (m_atoms.get_size() == 0)
		return false;	// source was not parsed

	m_run_mode = run_mode::run_to_line;
	m_nto_line = n_line;

	run();

	return true;
}

#endif