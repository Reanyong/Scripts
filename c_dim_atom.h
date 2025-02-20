#pragma once
#include "c_atom_table.h"
#include "c_call_stack.h"
#include "c_variable.h"
#include "c_array.h"

struct c_dim_entry
{
	c_string			m_name;
	VARENUM				m_type;
	int					m_nsize;	// Array sized declaration
	bool				m_bnew;		// new keyword
	bool				m_bbyref;

	CLSID				m_clsid;
	GUID				m_libid;
	GUID				m_iid;

	c_dim_entry()
	{
		memset(this, 0, sizeof(c_dim_entry));
	}

	void operator = (c_dim_entry& e)
	{
		m_name			= e.m_name;
		m_type			= e.m_type;
		m_nsize			= e.m_nsize;
		m_bnew			= e.m_bnew;
		m_bbyref		= e.m_bbyref;

		m_clsid			= e.m_clsid;
		m_libid			= e.m_libid;
		m_iid			= e.m_iid;
	}

	void reset()
	{
		memset(this, 0, sizeof(c_dim_entry));
	}
};

typedef c_array<c_dim_entry> c_sub_namespace;

//-----------------------------------------------------------------------------
// c_dim_atom -----------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_dim_atom : public c_atom  
{
	c_array<c_dim_entry>	m_entries;

public:
	c_dim_atom( c_atom_table* ptable,
				c_call_stack* p_call_stack,
				c_engine* p_engine,
				int nline);

	virtual ~c_dim_atom();

	virtual void prepare_func_helper();
	virtual void exec_();
	virtual void enum_expressions(c_ptr_array<c_expression>* p_expressions);

	void add(c_dim_entry& e) {m_entries.add(e);}

#ifdef _DEBUG
	virtual void dump(FILE* f);
	virtual void assert_valid();
#endif
};

