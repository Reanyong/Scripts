#include "pch.h"

#include <crtdbg.h>
#include "c_expression.h"
#include "c_ext_func.h"
#include "c_disp_get.h"
#include "c_routine_entry_atom.h"
#include "c_engine.h"
#include "c_atom_table.h"

//-----------------------------------------------------------------------------
// get_item -------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool get_item(SAFEARRAY* psa,
			  c_expression** p_indexes,
			  int n_indexes,
			  c_variable** pp_result,
			  c_engine* p_engine)
{
	_ASSERT(pp_result);


	if (!psa)
	{
		if (p_engine) p_engine->runtime_error(CUR_ERR_LINE, "trying to get item of an empty array.");
		return false;
	}

	if (n_indexes > psa->cDims) // out of dimensions
	{
		if (p_engine)
		{
			char err[MAX_ERR_LENGTH];
			_snprintf(err, MAX_ERR_LENGTH, "array is %d-dimensioned. you try to access %d dimension.", psa->cDims, n_indexes);
			p_engine->runtime_error(CUR_ERR_LINE, err);
		}
		return false;
	}

	c_variable v;
	c_array<long> indexes;
	int i;
	int n_index;
	for (i = 0; i < n_indexes; i++)
	{
		_ASSERT(p_indexes[i]);
		p_indexes[i]->exec(&v);
		n_index = v.as_integer();

		if ((unsigned)n_index >= psa->rgsabound[n_indexes - i - 1].cElements) // out of bounds
		{
			if (p_engine)
			{
				char err[MAX_ERR_LENGTH];
				_snprintf(err, MAX_ERR_LENGTH, "out of bounds. dimension: %d. max index allowed: %d. you try to access: %d.", i + 1, psa->rgsabound[n_indexes - i - 1].cElements - 1, n_index);
				p_engine->runtime_error(CUR_ERR_LINE, err);
			}
			return false;
		}

		indexes.add_const(n_index + psa->rgsabound[i].lLbound);
	}

	for (; i < psa->cDims; i++)
		indexes.add_const(0 + psa->rgsabound[i].lLbound);

	HRESULT hr = SafeArrayLock(psa);
	_ASSERT(!FAILED(hr));
	hr = SafeArrayPtrOfIndex(	psa,
								indexes.get_data(),
								(void**)pp_result);

	if (FAILED(hr))
	{
		_ASSERT(0); // hmm...
		return false;
	}

	hr = SafeArrayUnlock(psa);
	_ASSERT(!FAILED(hr));
	return true;
}

//-----------------------------------------------------------------------------
// set_item -------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool set_item(SAFEARRAY* psa,
			  c_expression** p_indexes,
			  int n_indexes,
			  c_variable* p_val,
			  c_engine* p_engine)
{
	_ASSERT(p_val);
	if (!psa)
	{
		if (p_engine) p_engine->runtime_error(CUR_ERR_LINE, "trying to set item of an empty array.");
		return false;
	}

	if (n_indexes > psa->cDims) // out of dimensions
	{
		if (p_engine)
		{
			char err[MAX_ERR_LENGTH];
			_snprintf(err, MAX_ERR_LENGTH, "array is %d-dimensioned. you try to access %d dimension.", psa->cDims, n_indexes);
			p_engine->runtime_error(CUR_ERR_LINE, err);
		}
		return false;
	}

	c_variable v;
	c_array<long> indexes;
	int i;
	int n_index;
	for (i = 0; i < n_indexes; i++)
	{
		_ASSERT(p_indexes[i]);
		p_indexes[i]->exec(&v);
		n_index = v.as_integer();

		if ((unsigned)n_index >= psa->rgsabound[n_indexes - i - 1].cElements) // out of bounds
		{
			if (p_engine)
			{
				char err[MAX_ERR_LENGTH];
				_snprintf(err, MAX_ERR_LENGTH, "out of bounds. dimension: %d. max index allowed: %d. you try to access: %d.", i + 1, psa->rgsabound[n_indexes - i - 1].cElements - 1, n_index);
				p_engine->runtime_error(CUR_ERR_LINE, err);
			}
			return false;
		}

		indexes.add_const(n_index + psa->rgsabound[i].lLbound);
	}

	for (; i < psa->cDims; i++)
		indexes.add_const(0 + psa->rgsabound[i].lLbound);

	c_variable* p_item = 0;
	HRESULT hr = SafeArrayLock(psa);
	_ASSERT(!FAILED(hr));
	hr = SafeArrayPtrOfIndex(	psa,
								indexes.get_data(),
								(void**)&p_item);

	if (FAILED(hr) || !p_val)
	{
		_ASSERT(0); // hmm...
		return false;
	}

	*p_item = *p_val;

	hr = SafeArrayUnlock(psa);
	_ASSERT(!FAILED(hr));

	return true;
}

// c_expression ---------------------------------------------------------------

c_expression::c_expression(c_call_stack* p_call_stack, c_atom_table* p_atom_table, c_engine* p_engine)
{
	m_pleft			= 0;
	m_pright		= 0;
	m_pext_func		= 0;
	m_pdisp_get		= 0;
	m_pengine		= p_engine;
	m_pcall_stack	= p_call_stack;
	m_patom_table	= p_atom_table;

	m_ppseudo_func	= 0;
	m_npseudo_func	= -1;

	m_pfunc_entry	= 0;
	//m_pglobal_expr	= 0;
	m_pext_const	= 0;

	m_pdll_caller	= 0;
}

c_expression::~c_expression()
{
	if (m_pleft)		delete m_pleft;
	if (m_pright)		delete m_pright;
	if (m_pext_func)	delete m_pext_func;
	if (m_pdisp_get)	delete m_pdisp_get;
	//if (m_pglobal_expr)	delete m_pglobal_expr;
	if (m_pdll_caller)	delete m_pdll_caller;

	m_func_expr.delete_items();
	m_dims.delete_items();
}

void c_expression::set_var_name(LPCTSTR var_name)
{
	m_var_desc = var_name;
}

void c_expression::set_ext_func(c_ext_func* p_ext_func)
{
	m_pext_func = p_ext_func;
}

void c_expression::set_disp_get (c_disp_get* get)
{
	_ASSERT(!m_pdisp_get);
	_ASSERT(get);
	m_pdisp_get = get;
}

void c_expression::add_func_expr(c_expression* p_expr)
{
	_ASSERT(p_expr);
	m_func_expr.add(p_expr);
}

c_expression* c_expression::get_func_expr(int n)
{
	return m_func_expr[n];
}

void c_expression::set_func_entry(c_routine_entry_atom* p_func_entry)
{
	_ASSERT(p_func_entry != 0);
	_ASSERT(m_pfunc_entry == 0);
	m_pfunc_entry = p_func_entry;
}

void c_expression::set_pseudo_func(c_func_caller* p_var, int n)
{
	_ASSERT(p_var);
	_ASSERT(!m_ppseudo_func);
	m_ppseudo_func = p_var;
	m_npseudo_func = n;
}

void c_expression::exec(c_variable* p_result)
{
	_ASSERT (this != 0);
	HRESULT hr;

	switch(m_action)
	{
	case c_action::_disp_get:
		_ASSERT(m_pdisp_get);
		m_pdisp_get->exec(p_result);
		break;

	case c_action::_ext_function:
		_ASSERT(m_pext_func);
		m_pext_func->exec(p_result);
		break;

	case c_action::_dll_function:
		_ASSERT(m_pdll_caller);
		m_pdll_caller->exec(p_result);
		break;

	case c_action::_const:
		*p_result = m_constant;
		break;

	case c_action::_ext_const:
		_ASSERT(m_pext_const);
		*p_result = *m_pext_const;
		break;

	case c_action::_variable :		// return c_variable value
		//todo. jkh
		if (m_var_desc.m_name[0] == '@')
		{
			CString szName = m_var_desc.m_name;
			if (szName.Left(1) == "@")
				szName = szName.Right(szName.GetLength() - 1);
			if (!CheckValidTagName(szName))
			{
				break;
			}
			else
			{
				KWGetTagValue(szName, *p_result);
				break;
			}
			break;
		}
		else if (m_dims.get_size())
		{
			c_variable* p_var;
			m_pcall_stack->get(&m_var_desc, &p_var);

			_ASSERT(p_var);
			_ASSERT(p_var->vt == (VT_ARRAY | VT_VARIANT));

			c_variable* p_item = 0;
			get_item(p_var->parray, m_dims.get_data(), m_dims.get_size(), &p_item, m_pengine);
			if (p_item) *p_result = *p_item;
			else p_result->reset();
		}
		else
		{
			c_variable* ptr = 0;
			m_pcall_stack->get(&m_var_desc, &ptr);
			_ASSERT(ptr);
			*p_result = *ptr;
		}
		break;

	case c_action::_plus:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			hr = c_variable::var_add(&l, &r, p_result);
			m_pengine->runtime_error(hr);
		}
		break;

	case c_action::_cat:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			hr = c_variable::var_cat(&l, &r, p_result);
			m_pengine->runtime_error(hr);
		}
		break;

	case c_action::_minus:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			hr = c_variable::var_sub(&l, &r, p_result);
			m_pengine->runtime_error(hr);
		}
		break;

	case c_action::_mul:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			hr = c_variable::var_mul(&l, &r, p_result);
			m_pengine->runtime_error(hr);
		}
		break;

	case c_action::_div:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			hr = c_variable::var_div(&l, &r, p_result);
			m_pengine->runtime_error(hr);
		}
		break;

	case c_action::_mod:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			hr = c_variable::var_mod(&l, &r, p_result);
			m_pengine->runtime_error(hr);
		}
		break;

	case c_action::_pow:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			hr = c_variable::var_pow(&l, &r, p_result);
			m_pengine->runtime_error(hr);
		}
		break;

	case c_action::_greater:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			*p_result = (bool)(l >  r);
		}
		break;

	case c_action::_less:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			*p_result = (bool)(l <  r);
		}
		break;

	case c_action::_greatere :
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			*p_result = (bool)(l >= r);
		}
		break;

	case c_action::_lesse:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			*p_result = (bool)(l <= r);
		}
		break;

	case c_action::_equal:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			*p_result = (bool)(l == r);
		}
		break;

	case c_action::_notequal:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			*p_result = (bool)(l != r);
		}
		break;

	case c_action::_and:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			hr = c_variable::var_and(&l, &r, p_result);
			m_pengine->runtime_error(hr);
		}
		break;

	case c_action::_or:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			hr = c_variable::var_or(&l, &r, p_result);
			m_pengine->runtime_error(hr);
		}
		break;

	case c_action::_xor:
		{
			c_variable l, r;
			m_pleft->exec(&l);
			m_pright->exec(&r);
			hr = c_variable::var_xor(&l, &r, p_result);
			m_pengine->runtime_error(hr);
		}
		break;

	case c_action::_not:
		{
			c_variable v;
			m_pright->exec(&v);
			*p_result = !v;
		}
		break;

	case c_action::_neg:
		{
			c_variable v;
			m_pright->exec(&v);
			*p_result = -v;
		}
		break;

	case c_action::_pseudo_function:
		{
			_ASSERT(m_ppseudo_func);
			c_variable* p_res = m_ppseudo_func->get_result(m_npseudo_func);

			if (m_dims.get_size())
			{
				_ASSERT(m_dims.get_size() == 1);// only one dim allowed here
				_ASSERT(p_res->vt == VT_VARARRAY);

				// calculate index
				c_variable v_idx;
				m_dims[0]->exec(&v_idx);
				int n_idx = v_idx.as_integer();

				// get array element
				c_variable* p_element = 0;
				get_element(p_res->parray, n_idx, &p_element);
				if (p_element) *p_result = *p_element;
			}
			else *p_result = *p_res;
		}
		break;

	case c_action::_function :
		{
			// this should never work. calling function is c_func_caller's job
			_ASSERT(0);
		}
		break;

	default:_ASSERT(false);
	}
}

bool c_expression::calls_user_routines()
{
	if (m_action == c_action::_function) return true;
	if (m_pleft) if (m_pleft->calls_user_routines()) return true;
	if (m_pright) if (m_pright->calls_user_routines()) return true;

	int i;
	for (i = 0; i < m_dims.get_size(); i++)
		if (m_dims[i]->calls_user_routines()) return true;

	return false;
}

//-----------------------------------------------------------------------------
// prepare_args ---------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_expression::prepare_args(c_ptr_array<c_expression>* p_expressions,
				  c_array<c_variable>* p_args,
				  c_call_stack* p_call_stack,
				  c_engine* p_engine,
				  bool b_reverse)
{
	p_args->reset(false);

	int i, n_idx;
	int n = p_expressions->get_size();
	p_args->alloc(n);

	for (i = 0; i < n; i++)
	{
		n_idx = b_reverse ? (n - i - 1) : i;

		c_expression* p_expr = p_expressions->get(i);
		c_variable* p_target = p_args->get_ptr(n_idx);
		p_target->reset();

		if (p_expr->m_action == c_action::_variable)
		{
			if (p_expr->is_array())
			{
				c_variable* p_var;
				p_call_stack->get(p_expr->get_var_desc(), &p_var);
				_ASSERT(p_var);

				SAFEARRAY* p_array = p_var->parray;
				_ASSERT(p_array);

				c_variable* p_item = 0;
				get_item(p_array, p_expr->m_dims.get_data(), p_expr->m_dims.get_size(), &p_item, p_engine);
				if (p_item) p_target->ref(p_item);
				else p_target->reset();
			}
			else
			{
				_ASSERT(p_expr->get_var_desc()->inited());

				c_variable* p_var = 0;
				p_call_stack->get(p_expr->get_var_desc(), &p_var);
				_ASSERT(p_var);

				if (p_var) p_target->ref(p_var);
			}
		}
		else
			p_expr->exec(p_target);
	}
#ifdef _DEBUG
	for (i = 0; i < p_args->get_size(); i++)
		p_args->get_ptr(i)->assert_valid();
#endif
}

//-----------------------------------------------------------------------------
// store_args -----------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_expression::store_args(c_array<c_variable>* p_args,
							  c_array<c_var_storage>* p_storage)
{
	int i;
	int n = p_args->get_size();

	p_storage->reset();
	p_storage->alloc(n);

	for (i = 0; i < n; i++)
	{
		c_variable* p_val = p_args->get_ptr(i);
		c_var_storage* p_stor = p_storage->get_ptr(i);
		p_val->store(p_stor);
	}
}

//-----------------------------------------------------------------------------
// restore_args ---------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_expression::restore_args(c_array<c_variable>* p_args,
								c_array<c_var_storage>* p_storage)
{
	int i;
	int n = p_args->get_size();

	for (i = 0; i < n; i++)
	{
		c_variable* p_val = p_args->get_ptr(i);
		c_var_storage* p_stor = p_storage->get_ptr(i);
		p_val->restore(p_stor);
	}
}

//-----------------------------------------------------------------------------
// assert_valid ---------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef _DEBUG
void c_expression::assert_valid()
{
	_ASSERT(!IsBadWritePtr(this, sizeof(c_expression)));
}
#endif