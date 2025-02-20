#ifndef C_SUB_TABLE_H
#define C_SUB_TABLE_H

#include <windows.h>
#include "c_string.h"
#include "c_array.h"

//-----------------------------------------------------------------------------
// c_routine_entry ------------------------------------------------------------
//-----------------------------------------------------------------------------

class c_routine_entry
{
public:
	int			m_position;
	bool		b_is_sub;
	c_string	m_name;
	int			m_nargs;
	VARENUM		m_type;

	c_routine_entry()
	{
		m_position	= -1;
		b_is_sub	= false;
		m_nargs		= 0;
		m_type		= VT_EMPTY;
	}

	void reset()
	{
		m_position	= -1;
		b_is_sub	= false;
		m_name.empty();
		m_nargs		= 0;
		m_type		= VT_EMPTY;
	}

	void operator = (c_routine_entry& entry)
	{
		m_position	= entry.m_position;
		b_is_sub	= entry.b_is_sub;
		m_name		= entry.m_name;
		m_nargs		= entry.m_nargs;
		m_type		= entry.m_type;
	}
};

#endif