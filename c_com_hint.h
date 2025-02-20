#ifndef C_COM_HINT_H
#define C_COM_HINT_H

#include "c_array.h"
#include "c_str_array.h"
#include "c_string.h"

class c_engine;

class c_com_hint
{
	c_engine*	m_pengine;
	c_string	m_format;

	bool is_byref(TYPEDESC td);
	const char* get_type_name(ITypeInfo* pti, TYPEDESC td);

	void get_member_type(ITypeInfo* pti,
						 const char* p_member,
						 VARTYPE* p_type,
						 HREFTYPE* p_href_type);

	void list_members(ITypeInfo* pti,
					  c_str_array* p_levels,
					  c_str_array* p_members,
					  c_array<bool>*	p_methods);

	void get_typeinfo(const char* p_class,
					  c_str_array* p_levels,
					  bool b_for_list,
					  ITypeLib** pp_tl,
					  ITypeInfo** pp_ti);

	void get_func_desc(ITypeInfo* pti,
					   const char* p_member,
					   FUNCDESC **pp_fd);

	void format_funcdesc(ITypeInfo* pti,
						 FUNCDESC* pfd,
						 const char** pp_hint);

public:

	c_com_hint(c_engine* p_engine);

	void list_members(const char* p_class,
					  c_str_array* p_levels,
					  c_str_array* p_members,
					  c_array<bool>*	p_methods);

	void get_hint(const char* p_class,
				  c_str_array* p_levels,
				  const char** pp_hint);
};

#endif