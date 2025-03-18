#ifndef VARIABLE_H
#define VARIABLE_H

#include "math.h"
#include "com.h"
#include <oaidl.h>	// LPDISPATCH
#include "c_string.h"
#include "c_array.h"
#include "c_ptr_array.h"
#include "c_str_array.h"
#include "c_array.h"

//-----------------------------------------------------------------------------
// array support --------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_expression;
class c_variable;
class c_engine;

bool get_item(SAFEARRAY* psa,
			  c_expression** p_indexes,
			  int n_indexes,
			  c_variable** pp_result,
			  c_engine* p_engine);

bool set_item(SAFEARRAY* psa,
			  c_expression** p_indexes,
			  int n_indexes,
			  c_variable* p_val,
			  c_engine* p_engine);

void get_element(SAFEARRAY* p_array, long n, c_variable** p_var);
void set_element(SAFEARRAY* p_array, long n, c_variable* p_var);

bool compatible_arrays(SAFEARRAY* psa1, SAFEARRAY* psa2);

bool is_valid_safearray(SAFEARRAY* psa);

enum arr_operator
{
	_arr_add,
	_arr_sub,
	_arr_mul,
	_arr_div,
	_arr_mod,
	_arr_and,
	_arr_or,
	_arr_xor,
	_arr_cat
};
void arr_add(c_variable* p_left, c_variable* p_right, c_variable* p_result, arr_operator action);

// BeeBasic type names --------------------------------------------------------

struct type_map
{
	const char*		_name;
	VARENUM			_type;
	bool			_bar_array;
};

#define VT_VARARRAY	(VT_VARIANT | VT_ARRAY)

static type_map types[]=
{
	{"Variant",		VT_EMPTY,				false},
	{"String",		VT_BSTR,				false},
	{"Integer",		VT_I4,					false},
	{"Float",		VT_R4,					false},
	{"Double",		VT_R8,					false},
	{"Boolean",		VT_BOOL,				false},
	{"Date",		VT_DATE,				false},
	{"Array",		(VARENUM)VT_VARARRAY,	false},
};

//-----------------------------------------------------------------------------
// c_var_storage --------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_var_storage
{
public:
	unsigned m_r1;
	unsigned m_r2;
	unsigned m_r3;
	SAFEARRAY* m_psa;
	SAFEARRAY* m_psa_old;
	c_engine* m_pengine;

	c_var_storage() {memset(this, 0, sizeof(c_var_storage));}
};

//
#define PUSH_RESERVED(x) \
c_var_storage __storage;\
(x)->store(&__storage);

//
#define POP_RESERVED(x) \
(x)->restore(&__storage);

//-----------------------------------------------------------------------------
// c_var_desc -----------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_var_desc
{
	c_string	m_name;
	int			m_nidx;
	bool		m_bglobal;

public:
	c_var_desc()
	{
		m_nidx		= -1;
		m_bglobal	= (bool) -1;
	}

	c_var_desc(c_var_desc& x)
	{
		m_name		= x.m_name;
		m_nidx		= x.m_nidx;
		m_bglobal	= x.m_bglobal;
	}

	void operator = (const char* p)
	{
		_ASSERT(!m_name.get_length());
		_ASSERT(m_nidx == -1);
		_ASSERT(m_bglobal == (bool) -1);

		m_name = p;
	}

	void set_name(const char* p_name)
	{
		_ASSERT(!m_name.get_length());
		m_name = p_name;
	}

	const char* get_name()
	{
		return m_name.get_buffer();
	}

	void set_idx(int n_idx, bool b_global)
	{
		m_nidx		= n_idx;
		m_bglobal	= b_global;
	}

	bool inited()
	{
		return m_name.get_length() != 0;
	}

	bool resolved()
	{
		return m_nidx != -1;
	}

	friend class c_call_stack;
	friend class c_assign_atom;
	friend class c_expression;
};

//-----------------------------------------------------------------------------
// c_var_data -----------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_var_data
{
	bool		m_bstrict;
	c_engine*	m_pengine;
	CLSID		m_clsid;
	GUID		m_iid;

	void reset()
	{
		m_bstrict = false;
		m_pengine = 0;
		memset(&m_clsid, 0, sizeof(CLSID));
		memset(&m_iid, 0, sizeof(GUID));
	}

	c_var_data()
	{
		//memset(this, 0, sizeof(c_var_data));
		reset();
	}

	c_var_data(c_engine* p_engine)
	{
		//memset(this, 0, sizeof(c_var_data));
		reset();
		m_pengine = p_engine;
	}

#ifdef _DEBUG
	void assert_valid()
	{
		_ASSERT(!IsBadWritePtr(this, sizeof(c_var_data)));
	}
#endif

	friend class c_variable;
};

//-----------------------------------------------------------------------------
// c_variable -----------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_variable : public VARIANT
{
	c_var_data* pVarData;
	inline c_var_data* get_var_data();
	inline void set_var_data(c_var_data* p_var_data);

	void cast(c_variable* p_val);
	void assign(c_variable* p_val);
public:
	c_variable();
	c_variable(c_engine* p_engine);
	c_variable(const int i);
	c_variable(const double d);
	c_variable(LPCTSTR lpsz);
	c_variable(BSTR bstr);
	c_variable(c_variable& v);

	~c_variable();

	inline void reset();
	void store(c_var_storage* p_storage);
	void restore(c_var_storage* p_storage);

private:
	inline void set_strict(bool b);

public:
	inline bool is_strict();

	inline c_variable* get_ptr();
	inline c_variable* get_scalar_ptr();

	inline void set_engine(c_engine*);
	inline c_engine* get_engine();

	inline void set_clsid_iid(CLSID* p_clsid, GUID* p_iid);

	void from_input(LPCTSTR str);			// assign value from input string
											// automaticaly detecting type

	void create_object();

	void operator = (const int val);
	void operator = (const double val);
	void operator = (LPCTSTR val);
	void operator = (BSTR val);
	void operator = (c_variable& val);
	void operator = (LPDISPATCH p_disp);
	void operator = (bool val);
	void operator = (SAFEARRAY* p_array);

	c_variable operator -  ();
	void operator += (c_variable& val);

	bool operator <  (c_variable& val);
	bool operator >  (c_variable& val);
	bool operator <= (c_variable& val);
	bool operator >= (c_variable& val);
	bool operator == (c_variable& val);
	bool operator != (c_variable& val);

	bool operator ! ();
	void ref(c_variable* p_var);

	// typecast

	void	as_string(c_string& str);
	int		as_integer();
	double	as_double();

	static HRESULT var_add(c_variable* p_left, c_variable* p_right, c_variable* p_result);
	static HRESULT var_cat(c_variable* p_left, c_variable* p_right, c_variable* p_result);
	static HRESULT var_sub(c_variable* p_left, c_variable* p_right, c_variable* p_result);
	static HRESULT var_mul(c_variable* p_left, c_variable* p_right, c_variable* p_result);
	static HRESULT var_div(c_variable* p_left, c_variable* p_right, c_variable* p_result);
	static HRESULT var_mod(c_variable* p_left, c_variable* p_right, c_variable* p_result);
	static HRESULT var_and(c_variable* p_left, c_variable* p_right, c_variable* p_result);
	static HRESULT var_or (c_variable* p_left, c_variable* p_right, c_variable* p_result);
	static HRESULT var_xor(c_variable* p_left, c_variable* p_right, c_variable* p_result);
	static HRESULT var_pow(c_variable* p_left, c_variable* p_right, c_variable* p_result);

#ifdef _DEBUG
	void assert_valid();
#endif

	friend class c_call_stack;
};

inline c_var_data* c_variable::get_var_data()
{
	//return (c_var_data*)MAKELONG(wReserved2, wReserved1);
	return pVarData;
}

inline void c_variable::set_var_data(c_var_data* p_var_data)
{
	// delete only in destructur
	//if (get_var_data()) delete get_var_data();
#ifdef _DEBUG
	if (p_var_data) p_var_data->assert_valid();
#endif
	wReserved1 = HIWORD(p_var_data);
	wReserved2 = LOWORD(p_var_data);
	wReserved3 = 0;

	pVarData = p_var_data;
}

inline c_variable* c_variable::get_scalar_ptr()
{
	if (vt == (VT_BYREF | VT_VARIANT))
	{
		_ASSERT(this != byref);
		return ((c_variable*)get_ptr())->get_scalar_ptr();
	}

	if (vt != VT_VARARRAY) return this;
	if (!parray) return this;
	if (!parray->cDims) return this;
	if (!parray->rgsabound[0].cElements) return this;

	return (c_variable*)parray->pvData;
}

inline c_variable* c_variable::get_ptr()
{
	if (vt == (VT_BYREF | VT_VARIANT))
	{
		_ASSERT(this != byref);
		return ((c_variable*)byref)->get_ptr();
	}

	return this;
}

inline void c_variable::reset()
{
#ifdef _DEBUG
	assert_valid();
#endif

	c_var_data* p_data = get_var_data();
	VariantClear(this);
	if (p_data) delete p_data;
	set_var_data(0);
}

inline c_engine* c_variable::get_engine()
{
	if (get_var_data()) if (get_var_data()->m_pengine) return get_var_data()->m_pengine;

	c_variable* ptr = (c_variable*)get_ptr();
	if (ptr != this) return ptr->get_engine();

	return 0;
}

inline void c_variable::set_engine(c_engine* p_engine)
{
	if (!get_var_data() && p_engine) set_var_data(new c_var_data);
	//if (get_var_data()) get_var_data()->m_pengine = p_engine;
	c_var_data* p_data = get_var_data();
	if (p_data)
		p_data->m_pengine = p_engine;
}

inline void c_variable::set_clsid_iid(CLSID* p_clsid, GUID* p_iid)
{
	_ASSERT(p_clsid);
	_ASSERT(!is_NULL_GUID(p_clsid));

	_ASSERT(p_iid);
	_ASSERT(!is_NULL_GUID(p_iid));

	if (!get_var_data()) set_var_data(new c_var_data);

	c_var_data* p_var_data = get_var_data();

	p_var_data->m_clsid = *p_clsid;
	p_var_data->m_iid = *p_iid;
}

inline void c_variable::set_strict(bool b)
{
	if (!get_var_data() && b) set_var_data(new c_var_data);
	if (get_var_data()) get_var_data()->m_bstrict = b;
}

inline bool c_variable::is_strict()
{
	if (get_var_data()) return (get_var_data()->m_bstrict);
	return false;
}

//-----------------------------------------------------------------------------
// c_name_table ---------------------------------------------------------------
//-----------------------------------------------------------------------------

struct c_name_table
{
	c_str_array			names;
	c_array<c_variable>	vars;

	int get_size()
	{
		_ASSERT(names.get_size() == vars.get_size());
		return vars.get_size();
	}

	void reset()
	{
		names.empty();
		vars.reset(true);
	}

	bool contains(const char* p_str) {return names.contains(p_str);}

	int find(const char* p_str)
	{
		int i;
		for (i = 0; i < names.get_size(); i++)
			//20210326 warning
			//if (stricmp(names[i], p_str) == 0)
			if (_stricmp(names[i], p_str) == 0)
				return i;

		return -1;
	}

	void add(const char* p_name, LPDISPATCH p_disp)
	{
		_ASSERT(p_name);
		_ASSERT(p_disp);
		_ASSERT(!contains(p_name));

		c_variable v;
		v = p_disp;

		vars.add(v);
		names.add(p_name);
	}

#ifdef _DEBUG
	void assert_valid()
	{
		_ASSERT(!IsBadWritePtr(this, sizeof(c_name_table)));

		int i;
		for (i = 0; i < vars.get_size(); i++)
			vars[i].assert_valid();
	}
#endif
};

//-----------------------------------------------------------------------------
// arithmetics ----------------------------------------------------------------
//-----------------------------------------------------------------------------

#endif