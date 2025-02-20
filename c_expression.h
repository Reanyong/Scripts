#pragma once
#include "c_variable.h"
#include "c_call_stack.h"

#include "c_ptr_array.h"

class c_routine_entry_atom;
class c_atom_table;

enum class c_action
{
	_variable,
	_const,
	_ext_const,
	_ext_function,
	_disp_get,
	_function,
	_dll_function,
	_pseudo_function,

	_plus,
	_minus,
	_mul,
	_div,
	_cat,

	_mod,

	_pow,

	_greater,
	_less,
	_greatere,
	_lesse,
	_equal,
	_notequal,

	_and,
	_or,
	_xor,

	_not,
	_neg,
};

class c_expression;
class c_ext_func;
class c_disp_get;
class c_atom;
class c_dll_caller;
class c_func_caller;

//-----------------------------------------------------------------------------

class c_expression
{
	c_atom_table*	m_patom_table;			// for function calls
	c_ext_func*		m_pext_func;			// extension function to execute
	c_disp_get*		m_pdisp_get;

	//c_string					m_varname;	// variable name
	c_var_desc					m_var_desc;
	c_ptr_array<c_expression>	m_dims;		// variable dimensions

// function call support
	c_ptr_array<c_expression>	m_func_expr;	// expressions to be evaluated
	c_routine_entry_atom*		m_pfunc_entry;
	c_dll_caller*				m_pdll_caller;

//20210329  private 멤버('c_expression' 클래스에서 선언)에 액세스할 수 없습니다
public:
	c_engine*		m_pengine;
	c_call_stack*	m_pcall_stack;			// a place c_variable values come from



public:
	c_expression(c_call_stack* p_call_stack, c_atom_table* p_atom_table, c_engine* p_engine);
	~c_expression();

	c_expression*	m_pleft;				// left subexpression
	c_expression*	m_pright;				// right subexpression
	c_variable		m_constant;				// constant value
	c_variable*		m_pext_const;
	c_action		m_action;				// kind of action to perform on left & right
	c_func_caller*	m_ppseudo_func;
	int				m_npseudo_func;

	void			set_var_name(LPCTSTR var_name);
	inline void		add_dim(c_expression* p_expr);
	inline bool		is_array();
	void			set_ext_func(c_ext_func* p_ext_func);
	void			set_disp_get (c_disp_get* get);
	inline void		set_dll_caler(c_dll_caller* p_caller);

	void			add_func_expr(c_expression* p_expr);
	int				get_func_expr_count()	{return m_func_expr.get_size();};
	c_expression*	get_func_expr(int n);
	void			set_func_entry(c_routine_entry_atom* p_func_entry);

	void			set_pseudo_func(c_func_caller* p_var, int n);

	void			exec(c_variable* p_result);						// calculate expression
	c_var_desc*		get_var_desc()	{return &m_var_desc;}

	bool			calls_user_routines();

#ifdef _DEBUG
	virtual void assert_valid();
#endif

// static helpers -------------------------------------------------------------

// for internal use
	static void prepare_args(c_ptr_array<c_expression>* p_expressions,
				  c_array<c_variable>* p_args,
				  c_call_stack* p_call_stack,
				  c_engine* p_engine,
				  bool b_reverse);

	static void store_args(c_array<c_variable>* p_args,
						   c_array<c_var_storage>* p_storage);

	static void restore_args(c_array<c_variable>* p_args,
							 c_array<c_var_storage>* p_storage);

	friend class c_func_caller;
	friend class c_assign_atom;
};

// inlines --------------------------------------------------------------------

inline void c_expression::add_dim(c_expression* p_expr)
{
	_ASSERT(p_expr);
	_ASSERT(!m_dims.contains(p_expr));
	m_dims.add(p_expr);
}

inline bool c_expression::is_array()
{
	return m_dims.get_size() > 0;
}

inline void c_expression::set_dll_caler(c_dll_caller* p_caller)
{
	_ASSERT(p_caller);
	_ASSERT(!m_pdll_caller);
	m_pdll_caller = p_caller;
}

//-----------------------------------------------------------------------------

typedef c_ptr_array<c_expression> c_expressions;

