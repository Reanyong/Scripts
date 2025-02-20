#include "pch.h"
#include "c_atom.h"
#include "c_engine.h"
#include "c_ext_func.h"
#include "c_disp_get.h"
#include "c_routine_entry_atom.h"

//-----------------------------------------------------------------------------
// constructor ----------------------------------------------------------------
//-----------------------------------------------------------------------------

c_func_caller::c_func_caller(c_atom* p_atom)
{
	m_patom		= 0;
	_ASSERT(p_atom);
	m_patom		= p_atom;
}

c_func_caller::~c_func_caller()
{
//	m_expressions.delete_items();
}

//-----------------------------------------------------------------------------
// add_item -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_func_caller::add_item()
{
	c_caller_item* p_new = m_items.add_new();

	p_new->m_ncount = 0;

	while (p_new->m_on.get_size() < m_expressions.get_size())
	{
		p_new->m_on.add_const(true);
		p_new->m_retvals.add_new();
	}
}

//-----------------------------------------------------------------------------
// remove_item ----------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_func_caller::remove_item()
{
	m_items.remove_last();
}

//-----------------------------------------------------------------------------
// assert_valid ---------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef _DEBUG
void c_func_caller::assert_valid()
{
	_ASSERT(!IsBadWritePtr(this, sizeof(c_func_caller)));
	_ASSERT(m_patom);

	int i;
	for (i = 0; i < m_items.get_size(); i++)
	{
		_ASSERT(m_items[i].m_ncount >= 0);
        _ASSERT(m_items[i].m_on.get_size() == m_expressions.get_size());
		_ASSERT(m_items[i].m_retvals.get_size() == m_expressions.get_size());
	}
}
#endif

//-----------------------------------------------------------------------------
// exec -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_func_caller::exec()
{
	if (!m_expressions.get_size()) return true;
	c_caller_item* p_last = m_items.get_last_ptr();

	_ASSERT(p_last->m_ncount >= 0);
	_ASSERT(p_last->m_ncount <= m_expressions.get_size());
	_ASSERT(p_last->m_retvals.get_size() == m_expressions.get_size());
	_ASSERT(p_last->m_on.get_size() == m_expressions.get_size());

// if we just returned from subroutine, we need to get return value from callstack.

	if (p_last->m_ncount > 0)
	{
		p_last->m_retvals[p_last->m_ncount - 1] = *m_patom->m_pcall_stack->get_ret_val();
	}

	if (p_last->m_ncount == m_expressions.get_size())
		return true; // all evaluated. time to execute atom.

// prepare stuff to call function ---------------------------------------------

	if (p_last->m_on[p_last->m_ncount])
	{
// get function expression that will be evalueted

		c_expression* p_expr = m_expressions[p_last->m_ncount];
		_ASSERT(p_expr->m_action == c_action::_pseudo_function); // '_function' was replaced with '_pseudo_function' in 'c_func_caller::from_expr'
		_ASSERT(p_expr->m_pfunc_entry);

// calc arguments for the function

		c_array<c_variable> args;
		c_expression::prepare_args(&p_expr->m_func_expr, &args, p_expr->m_pcall_stack, m_patom->m_pengine, false);
		_ASSERT(args.get_size() == p_expr->m_func_expr.get_size());

// pass them to the function entry atom. it will be ready to start execution.

		p_expr->m_pfunc_entry->pass_args(args.get_data());

// make a call. tell call stack to return to current atom.
// tell atom table next atom is routine entry.

		_ASSERT(m_patom);
		p_expr->m_pcall_stack->call(m_patom);
		m_patom->m_ptable->set_cur_atom(p_expr->m_pfunc_entry);
	}

// one more function was called, so increment count

	p_last->m_ncount ++;

	return false;
}

//-----------------------------------------------------------------------------
// from_expr ------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_func_caller::from_expr(c_expression* p_expr)
{
	int i, j;
	_ASSERT(p_expr);

	if (p_expr->m_pleft) from_expr(p_expr->m_pleft);
	if (p_expr->m_pright) from_expr(p_expr->m_pright);

	for (i = 0; i < p_expr->m_dims.get_size(); i++)
		from_expr(p_expr->m_dims[i]);

	switch(p_expr->m_action)
	{
	case c_action::_ext_function:
		{
			_ASSERT(p_expr->m_pext_func);

			for (i = 0; i < p_expr->m_pext_func->m_expressions.get_size(); i++)
				from_expr(p_expr->m_pext_func->m_expressions[i]);
		}
		break;

	case c_action::_disp_get:
		{
			_ASSERT(p_expr->m_pdisp_get);

			for (i = 0; i < p_expr->m_pdisp_get->m_disp_levels.get_size(); i++)
			{
				c_disp_level* p_disp_level = p_expr->m_pdisp_get->m_disp_levels[i];

				for (j = 0; j < p_disp_level->m_expressions.get_size(); j++)
					from_expr(p_disp_level->m_expressions[j]);
			}
		}
		break;

	case c_action::_function:
		{
			_ASSERT(p_expr->m_pfunc_entry);

			for (i = 0; i < p_expr->m_func_expr.get_size(); i++)
				from_expr(p_expr->m_func_expr[i]);

			// the main point

			_ASSERT(!m_expressions.contains(p_expr));
			m_expressions.add(p_expr);
			add_item();

			p_expr->m_action = c_action::_pseudo_function;
			p_expr->set_pseudo_func(this, m_expressions.get_size() - 1);
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// dump -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef _DEBUG
void c_func_caller::dump(FILE* f)
{
	if (!m_expressions.get_size()) return;

	fprintf(f, "calls functions: ");

	int i;
	for (i = 0; i < m_expressions.get_size(); i++)
	{
		_ASSERT(m_expressions[i]->m_pfunc_entry);
		fprintf(f, m_expressions[i]->m_pfunc_entry->get_var_desc()->get_name());
		fprintf(f, " ");
	}
	fprintf(f, "\n");
}
#endif

//-----------------------------------------------------------------------------
// c_atom ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

c_atom::c_atom(c_atom_table* ptable, c_call_stack* p_stack, c_engine* p_engine, int nline)
: m_func_caller(this)
{
	_ASSERT(ptable);
	_ASSERT(p_stack);
	_ASSERT(p_engine);
	_ASSERT(nline >= 0);

	m_ptable		= ptable;
	m_pcall_stack	= p_stack;
	m_pengine		= p_engine;
	m_nline			= nline;
	m_bbreak		= false;
	m_pbp_condition	= 0;
	m_pnext = 0;
	m_type = atom_type::if_atom;
}

c_atom::~c_atom()
{
	if (m_pbp_condition) delete m_pbp_condition;
}

//-----------------------------------------------------------------------------
// set_bp_condition -----------------------------------------------------------
//-----------------------------------------------------------------------------

void c_atom::set_bp_condition(c_expression* p_condition)
{
	if (m_pbp_condition) delete m_pbp_condition;
	m_pbp_condition = p_condition;
}

//-----------------------------------------------------------------------------
// exec -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_atom::exec()
{
#ifdef _DEBUG
	if (m_pengine!= NULL) m_pengine->assert_valid();
#endif
	pre_step();

	if (m_func_caller.exec())
	{
		m_func_caller.reset();

		exec_();
	}
#ifdef _DEBUG
	if (m_pengine) m_pengine->assert_valid();
#endif
}

#ifdef _DEBUG
void c_atom::assert_valid()
{
	_ASSERT(!IsBadWritePtr(this, sizeof(c_atom)));
	_ASSERT(m_ptable);
	_ASSERT(m_pcall_stack);
	_ASSERT(m_pengine);
	_ASSERT(m_nline >= 0);

	m_func_caller.assert_valid();
}
#endif