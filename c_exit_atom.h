#ifndef C_EXIT_ATOM
#define C_EXIT_ATOM

#include "c_atom_table.h"

//20220715 YDK ¼öÁ¤
enum class exit_type
{
	exit_undef,
	exit_routine,
	exit_for,
	exit_while,
	exit_do
};

class c_for_atom;

class c_exit_atom : public c_atom
{
	exit_type m_exit_type;

public:
	c_exit_atom(c_atom_table* ptable,
				c_call_stack* p_call_stack,
				c_engine* p_engine,
				int nline);

	virtual ~c_exit_atom();

	c_for_atom*	m_pfor;

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

	inline void			set_type(exit_type t);
	inline exit_type	get_type();

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

// inlines --------------------------------------------------------------------

inline void c_exit_atom::set_type(exit_type t)
{
	_ASSERT(m_exit_type == exit_type::exit_undef);
	m_exit_type = t;
}

inline exit_type c_exit_atom::get_type()
{
	return m_exit_type;
}

#endif