#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include "c_variable.h"
#include "config.h"
#include "c_array.h"

//-----------------------------------------------------------------------------
// extension_sub --------------------------------------------------------------
//-----------------------------------------------------------------------------

struct extension_sub
{
	c_string m_name;
	PEXTENSION_SUB_FUNCTION m_sub;
	CHECK_ROUTINE m_pcheck;

	extension_sub()
	{
		m_sub	= 0;
		m_pcheck= 0;
	}

	extension_sub(LPCTSTR p_name, PEXTENSION_SUB_FUNCTION p_sub, CHECK_ROUTINE p_check)
	{
		m_sub	= p_sub;
		m_name	= p_name;
		m_pcheck= p_check;
	}

	void operator = (extension_sub& origin)
	{
		m_sub	= origin.m_sub;
		m_name	= origin.m_name;
		m_pcheck= origin.m_pcheck;
	}
};

//typedef c_array<extension_sub> c_extension_sub_table;

//-----------------------------------------------------------------------------
// extension_function ---------------------------------------------------------
//-----------------------------------------------------------------------------

struct extension_function
{
	c_string m_name;
	PEXTENSION_FUNCTION_FUNCTION pfunc;
	CHECK_ROUTINE m_pcheck;

	extension_function()
	{
		pfunc   = 0;
		m_pcheck= 0;
	}

	extension_function(LPCTSTR p_name, PEXTENSION_FUNCTION_FUNCTION p_func, CHECK_ROUTINE p_check)
	{
		pfunc	= p_func;
		m_name	= p_name;
		m_pcheck= p_check;
	}

	void operator = (extension_function& origin)
	{
		pfunc	= origin.pfunc;
		m_name	= origin.m_name;
		m_pcheck= origin.m_pcheck;
	}
};

//typedef c_array<extension_function> c_extension_function_table;

//-----------------------------------------------------------------------------
// extension constants --------------------------------------------------------
//-----------------------------------------------------------------------------

//typedef c_array<c_variable> c_extension_constant_table;

#endif