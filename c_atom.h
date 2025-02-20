#pragma once
#include <stdio.h>
#include "c_expression.h"
#include "c_variable.h"
#include "config.h"
//20220715 YDK ¼öÁ¤
enum class atom_type
{
	if_atom,
	for_atom,
	while_atom,
	do_atom,
	assign_atom,
	label_atom,
	goto_atom,
	beep_atom,
	cls_atom,
	dim_atom,
	dialog_atom,
	input_atom,
	print_atom,
	routine_entry_atom,
	call_sub_atom,
	end_routine_atom,
	ext_sub_atom,
	obj_set_atom,
	disp_set_atom,
	case_atom,
	exit_atom,

};

static const char* get_atom_name(atom_type t)
{
	const char* p_atom_name;
	switch (t)
	{
	case atom_type::if_atom				: p_atom_name = "IF";			break;
	case atom_type::for_atom			: p_atom_name = "FOR";			break;
	case atom_type::while_atom			: p_atom_name = "WHILE";		break;
	case atom_type::do_atom				: p_atom_name = "DO";			break;
	case atom_type::assign_atom			: p_atom_name = "ASSIGN";		break;
	case atom_type::label_atom			: p_atom_name = "LABEL";		break;
	case atom_type::goto_atom			: p_atom_name = "GOTO";			break;
	case atom_type::beep_atom			: p_atom_name = "BEEP";			break;
	case atom_type::cls_atom			: p_atom_name = "SLC";			break;
	case atom_type::dim_atom			: p_atom_name = "DIM";			break;
	case atom_type::dialog_atom			: p_atom_name = "DIALOG";		break;
	case atom_type::input_atom			: p_atom_name = "INPUT";		break;
	case atom_type::print_atom			: p_atom_name = "PRINT";		break;
	case atom_type::routine_entry_atom	: p_atom_name = "ROUTINE ENTRY";break;
	case atom_type::call_sub_atom		: p_atom_name = "CALL SUB";		break;
	case atom_type::end_routine_atom	: p_atom_name = "END ROUTINE";	break;
	case atom_type::ext_sub_atom		: p_atom_name = "EXT SUB";		break;
	case atom_type::obj_set_atom		: p_atom_name = "OBJ SET";		break;
	case atom_type::disp_set_atom		: p_atom_name = "DISP SET";		break;
	case atom_type::case_atom			: p_atom_name = "CASE";			break;
	default								: p_atom_name = "!UNKNOWN!";	break;
	}

	return p_atom_name;
}

class c_atom;
class c_atom_table;
class c_call_stack;
class c_engine;

//-----------------------------------------------------------------------------
// c_func_caller --------------------------------------------------------------
//-----------------------------------------------------------------------------

struct c_caller_item
{
	c_array<c_variable>	m_retvals;		// results returned by functions
	int					m_ncount;		// number of expressions already evaluated
	c_array<bool>		m_on;

	c_caller_item()		{m_ncount = 0;}
};

class c_func_caller
{
	c_ptr_array<c_expression>	m_expressions;	// function call expressions
	c_array<c_caller_item>		m_items;

	c_atom*						m_patom;
public:
	c_func_caller(c_atom* p_atom);
	~c_func_caller();

	void add_item();
	void remove_item();

	inline int  get_size();
	inline void enable(int n_expr, bool b_on);
	inline c_variable* get_result(int n);

	void from_expr(c_expression* p_expr);
	bool exec();
	inline void reset();

#ifdef _DEBUG
	void dump(FILE* f);
	void assert_valid();
#endif
};

// inlines --------------------------------------------------------------------

inline int c_func_caller::get_size()
{
//	_ASSERT(m_expressions.get_size() == m_on.get_size());
	return m_expressions.get_size();
}

inline void c_func_caller::enable(int n_expr, bool b_on)
{
	c_caller_item* p_last = m_items.get_last_ptr();
	p_last->m_on[n_expr] = b_on;
}

inline void c_func_caller::reset()
{
	if (!m_items.get_size()) return;
	c_caller_item* p_last = m_items.get_last_ptr();
	p_last->m_ncount = 0;
}

inline c_variable* c_func_caller::get_result(int n)
{
	c_caller_item* p_last = m_items.get_last_ptr();
	return p_last->m_retvals.get_ptr(n);
}

//-----------------------------------------------------------------------------
// c_atom ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_atom
{
protected:
	c_atom_table*	m_ptable;
	c_call_stack*	m_pcall_stack;
	c_func_caller	m_func_caller;
	c_expression*	m_pbp_condition;
	c_engine*		m_pengine;
	bool			m_bbreak;	// breakpoint flag
public:
	c_atom(c_atom_table* ptable, c_call_stack* p_stack, c_engine* p_engine, int nline);
	virtual ~c_atom();

	virtual void pre_step() {}
	virtual void prepare_func_helper() = 0;
	virtual void exec_() = 0;

	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions) = 0;
	void exec();

	inline bool brk();
	inline void set_break(bool b);
	inline bool get_break();
	inline void on_enter_routine();
	inline void on_exit_routine();

	int			m_nline;	// code line
	atom_type	m_type;		// atom type
	c_atom*		m_pnext;	// atom to be executed next

	void set_bp_condition(c_expression* p_condition);

#ifdef _DEBUG
	virtual void dump(FILE* f) = 0;
	virtual void assert_valid();
#endif

	friend class c_func_caller;
};

// inlines --------------------------------------------------------------------

inline bool c_atom::brk()
{
	if (!m_bbreak) return false;
	if (!m_pbp_condition) return true;

	c_variable v;
	m_pbp_condition->exec(&v);
	if (!v) return false;
	return true;

}

inline void c_atom::set_break(bool b)
{
	m_bbreak = b;
	if (!m_bbreak) if (m_pbp_condition)
	{
		delete m_pbp_condition;
		m_pbp_condition = 0;
	}
}

inline bool c_atom::get_break()
{
	return m_bbreak;
}

inline void c_atom::on_enter_routine()
{
	m_func_caller.add_item();
}

inline void c_atom::on_exit_routine()
{
	m_func_caller.remove_item();
}
