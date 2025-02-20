#include "pch.h"

#include <crtdbg.h>
#include "c_call_stack.h"
#include "c_atom_table.h"
#include "c_engine.h"

c_call_stack::c_call_stack(c_engine* p_engine)
{
	m_pengine = p_engine;
	reset();
}

c_call_stack::~c_call_stack()
{
	m_name_tables.delete_items();
}

bool c_call_stack::find_var(const char* p_name,
							int& n_level,
							int& n_id)
{
	c_name_table* t = m_name_tables[m_nlevel];

	int i;
	for (i = 0; i < t->get_size(); i++)
	{
		if (_stricmp(t->names[i], p_name) == 0)
		{
			n_level = m_nlevel;
			n_id = i;
			return true;
		}
	}

	if (m_nlevel == 0) return false; // current level is 0 (global)

	t = m_name_tables[0];

	for (i = 0; i < t->get_size(); i++)
	{
		if (_stricmp(t->names[i], p_name) == 0)
		{
			n_level = 0;
			n_id = i;
			return true;
		}
	}

	return false;
}

bool c_call_stack::set_global(const char* p_name, const VARIANT* p_var)
{
	c_name_table* t = m_name_tables[0];
	int n = t->find(p_name);

	// check if already exists
	if (n == -1)
	{
		c_variable v;
		v = *((c_variable*)p_var);

		t->vars.add(v);
		t->names.add(p_name);
	}
	else
	{
		c_variable* p = (c_variable*) p_var;
		t->vars[n] = *p;
	}

	return true;
}

bool c_call_stack::get_global(const char* p_name, VARIANT** pp_var)
{
	c_name_table* t = m_name_tables[0];
	int n = t->find(p_name);
	if (n == -1) return false;

	*pp_var = t->vars.get_ptr(n);
	return true;
}

void c_call_stack::add(c_dim_entry* p)
{
	int n_level = -1, n_id;

	if (find_var(p->m_name.get_buffer(), n_level, n_id) && m_nlevel == n_level)
	{
		c_name_table* t = m_name_tables[n_level];
		c_variable* v = t->vars.get_ptr(n_id);

		v->reset();
		v->vt = p->m_type;
		v->set_strict(true);
	}
	else
	{
		c_name_table* t = m_name_tables[m_nlevel];

		c_variable* p_last = t->vars.add_new();
		p_last->set_engine(m_pengine);
		p_last->vt = p->m_type;
		p_last->set_strict(p->m_type != VT_EMPTY);

		t->names.add(p->m_name.get_buffer());

		if (p->m_nsize)
		{
			_ASSERT(p->m_type == VT_VARARRAY);
			_ASSERT(p_last->vt == VT_VARARRAY);
			p_last->parray = SafeArrayCreateVector(VT_VARIANT, 0, p->m_nsize);
			_ASSERT(is_valid_safearray(p_last->parray));
		}
		if (p->m_type == VT_DISPATCH)
		{
			p_last->set_clsid_iid(&p->m_clsid, &p->m_iid);
			if (p->m_bnew) p_last->create_object();
		}
	}
}

void c_call_stack::get(c_var_desc* p_var_desc, c_variable** p_result)
{
	if (p_var_desc->resolved())
	{
		_ASSERT(m_pengine->is_option_explicit());
		c_name_table* t = m_name_tables[p_var_desc->m_bglobal ? 0 : m_nlevel];
		*p_result = t->vars.get_ptr(p_var_desc->m_nidx);
	}
	else
	{
		int n_level, n_id;

		if (find_var(p_var_desc->get_name(), n_level, n_id))
		{
			_ASSERT(n_level == 0 || n_level == m_nlevel);
			if (m_pengine->is_option_explicit())
				p_var_desc->set_idx(n_id, n_level == 0);

			c_name_table* t = m_name_tables[n_level];
			*p_result = t->vars.get_ptr(n_id);
		}
		else
		{
			c_name_table* t = m_name_tables[m_nlevel];

			t->vars.add_new();
			t->names.add(p_var_desc->get_name());

			*p_result = t->vars.get_ptr(t->vars.get_size() - 1);

	//		p_result = 0;
	//		_ASSERT(0);
		}
	}
#ifdef _DEBUG
	(*p_result)->assert_valid();
#endif
}

void c_call_stack::set_var(c_var_desc* p_var_desc, c_variable* p_var)
{
#ifdef _DEBUG
	p_var->assert_valid();
#endif

	if (p_var_desc->resolved())
	{
		_ASSERT(m_pengine->is_option_explicit());
		c_name_table* t = m_name_tables[p_var_desc->m_bglobal ? 0 : m_nlevel];

		c_variable& v = t->vars[p_var_desc->m_nidx];
		v = *p_var;//*(c_variable*)p_var->get_scalar_ptr();
	}
	else
	{
		int n_level, n_id;

		if (find_var(p_var_desc->get_name(), n_level, n_id))
		{
			_ASSERT(n_level == 0 || n_level == m_nlevel);
			if (m_pengine->is_option_explicit())
				p_var_desc->set_idx(n_id, n_level == 0);

			c_name_table* t = m_name_tables[n_level];

			c_variable* p_v = t->vars.get_ptr(n_id);
			*p_v = *p_var;//*(c_variable*)p_var->get_scalar_ptr();

#ifdef _DEBUG
			p_v->assert_valid();
#endif
		}
		else
		{
			c_name_table* t = m_name_tables[m_nlevel];

			t->vars.add(*p_var);
			t->names.add(p_var_desc->get_name());
		}
	}
}

int c_call_stack::get_local_size()
{
	if (m_nlevel == 0) return 0;
	return m_name_tables[m_nlevel]->get_size();
}

int c_call_stack::get_global_size()
{
	return m_name_tables[0]->get_size();
}

bool c_call_stack::get_local_var(int n,
								 c_variable* p_var,
								 const char** pp_name)
{
	if (n < 0 || n >= m_name_tables[m_nlevel]->get_size()) return false;
	if (m_nlevel == 0) return true; // global

	c_name_table* t = m_name_tables[m_nlevel];

	*p_var		= t->vars[n];
	*pp_name	= t->names[n];

	return true;
}

bool c_call_stack::get_global_var(int n,
								  c_variable* p_var,
								  const char** pp_name)
{
	if (n < 0 || n >= m_name_tables[0]->get_size()) return false;

	c_name_table* t = m_name_tables[0];

	*p_var		= t->vars[n];
	*pp_name	= t->names[n];

	return true;
}

bool c_call_stack::get_global_var(const char* p_name,
								  c_variable* p_var)
{
	int i;
	c_name_table* t = m_name_tables[0];

	for (i = 0; i < t->get_size(); i++)
	{
		if (_stricmp(t->names[i], p_name) == 0)
		{
			*p_var = t->vars[i];
			return true;
		}
	}

	return false;
}

bool c_call_stack::call(c_atom* p_ret_atom)
{
	m_rets.add(p_ret_atom);
	m_name_tables.add_new();
	m_nlevel ++;

	return true;
}

bool c_call_stack::is_function_context()
{
	if (m_nlevel == 0) return false;
	return m_rets[m_nlevel - 1] == 0;
}

c_atom* c_call_stack::ret()
{
// get return value -----------------------------------------------------------

	int n_level, n_id;

	if (find_var(m_pengine->m_atom_table.get_routine_name(), n_level, n_id) &&
		(n_level == m_nlevel))
	{
		c_name_table* t = m_name_tables[m_nlevel];
		m_retval = t->vars[n_id];
	}
	else m_retval.reset();

#ifdef _DEBUG
	m_pengine->assert_valid();
#endif

// go one level down ----------------------------------------------------------

#ifdef _DEBUG
	m_pengine->assert_valid();
#endif
	m_name_tables.remove_last(true);
#ifdef _DEBUG
	m_pengine->assert_valid();
#endif

	if (m_nlevel <= 0) return 0; // might be started by 'run_routine' or called from on_end_execution()

	_ASSERT(m_name_tables.get_size() == m_rets.get_size());

	c_atom* p_atom = m_rets[m_nlevel - 1];

	m_rets.remove_last();
	m_nlevel --;

#ifdef _DEBUG
	m_pengine->assert_valid();
#endif
	return p_atom;
}

void c_call_stack::reset()
{
	for (int i = 0; i < m_name_tables.get_size(); i++)
	{
		m_name_tables[i]->reset();
	}

	m_name_tables.delete_items();
	m_name_tables.reset();
	m_rets.reset(true);
	m_nlevel = 0;

	m_name_tables.add_new();
}

bool c_call_stack::declared(const char* p_name, VARENUM* p_type)
{
	int n_level = -1, n_id;
	if (find_var(p_name, n_level, n_id))
	{
		if (p_type)
		{
			c_name_table* t = m_name_tables[n_level];
			if (t->vars[n_id].is_strict()) *p_type = (VARENUM)t->vars[n_id].vt;
		}
		return true;
	}

	return false;
}

bool c_call_stack::declared_array(const char* p_name)
{
	int n_level = -1, n_id;
	if (!find_var(p_name, n_level, n_id)) return false;

	c_name_table* t = m_name_tables[n_level];
	return t->vars[n_id].vt == VT_VARARRAY;
}

#ifdef _DEBUG
void c_call_stack::assert_valid()
{
	_ASSERT(!IsBadWritePtr(this, sizeof(c_call_stack)));

	int i;
	for (i = 0; i < m_name_tables.get_size(); i++)
		m_name_tables[i]->assert_valid();
}
#endif
