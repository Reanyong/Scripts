#include "pch.h"
#include "c_dll_func.h"
#include "c_engine.h"

/*
__declspec(naked)
void __stdcall DllCall(void*, void*)
{
	static int __ret_addr;
	_asm
	{

		pop     edx
		mov		__ret_addr, edx
		pop     eax
		pop     ecx
		mov     esp,ecx
		call    eax
		mov		edx, __ret_addr
		mov		[esp], edx
		ret
	}
}
*/

typedef void	(__stdcall *DLL_VOID)();
typedef int		(__stdcall *DLL_INT)();
typedef double	(__stdcall *DLL_DOUBLE)();

//-----------------------------------------------------------------------------
// constructor ----------------------------------------------------------------
//-----------------------------------------------------------------------------

c_dll_func::c_dll_func()
{
	m_hmodule	= 0;
	m_fp		= 0;
	m_ret_type	= VT_EMPTY;
}

c_dll_func::~c_dll_func()
{
	unload();
}

//-----------------------------------------------------------------------------
// load -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_dll_func::load(c_string* p_err)
{
	_ASSERT(!m_hmodule);
	_ASSERT(!m_fp);

	m_hmodule = LoadLibrary(m_dll_name.get_buffer());
	if (!m_hmodule)
	{
		if (p_err)
		{
			*p_err = "Cannot load module '";
			*p_err += m_dll_name;
			*p_err += "'.";
		}

		return false;
	}

	m_fp = GetProcAddress(m_hmodule, m_alias_name.get_buffer());
	if (!m_fp )
	{
		if (p_err)
		{
			*p_err = "Cannot find '";
			*p_err += m_alias_name;
			*p_err += "' in module '";
			*p_err += m_dll_name;
			*p_err += "'.";
		}

		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// unload ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_dll_func::unload()
{
	if (m_hmodule) FreeLibrary(m_hmodule);
	m_hmodule = 0;
	m_fp = 0;
}

//-----------------------------------------------------------------------------
// operator = -----------------------------------------------------------------
//-----------------------------------------------------------------------------

void c_dll_func::operator = (c_dll_func& d)
{
	m_dll_name		= d.m_dll_name;
	m_routine_name	= d.m_routine_name;
	m_alias_name	= d.m_alias_name;

	m_arg_types.reset();
	int i;
	for (i = 0; i < d.m_arg_types.get_size(); i++)
	{
		m_arg_types.add(d.m_arg_types[i]);
		m_byref_args.add(d.m_byref_args[i]);
	}

	m_ret_type = d.m_ret_type;
}

//-----------------------------------------------------------------------------
// exec -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool c_dll_func::exec(int n_args, c_variable** p_args, c_variable* p_result)
{
	_ASSERT(m_arg_types.get_size() == m_byref_args.get_size());

	if (!m_fp) if (!load(0)) return false;

// calc argument sizes --------------------------------------------------------

	int n = 0;
	int i;
	for (i = 0; i < n_args; i++)
	{
		switch (p_args[i]->get_scalar_ptr()->vt)
		{
		case VT_BSTR:
		case VT_INT:
		case VT_BOOL:
		case VT_I4: n += 4;
			break;

		case VT_I2: n += 2;
			break;

		case VT_I1: n += 1;
			break;

		case VT_R8: n += 8;
			break;

		default:
			_ASSERT(0);
			n += 4;
		}
	}

// push args on stack -------------------------------------------------------------------

	char* p_stack = (char*)_alloca(n);
	_ASSERT(p_stack);
	char* p = p_stack;

	// this array stores pointers to string arguments
	c_str_array strings;

	for (i = 0; i < n_args; i++)
	{
		switch (p_args[i]->get_scalar_ptr()->vt)
		{
		case VT_BSTR:
			{
				c_string str;
				p_args[i]->as_string(str);
				strings.add(str.get_buffer());

				const char **p_c = (const char**)p;
				*p_c = strings[strings.get_size() - 1];
				p += 4;
			}
			break;

		case VT_INT:
		case VT_BOOL:
		case VT_I4:
			{
				if (!m_byref_args[i])
				{
					int *p_i = (int*)p;
					*p_i = p_args[i]->as_integer();
					p += 4;
				}
				else
				{
					int **p_i = (int**)p;
					*p_i = &p_args[i]->intVal;
					p += 4;
				}
			}
			break;

		case VT_I2:
			{
				if (!m_byref_args[i])
				{
					short *p_i = (short*)p;
					*p_i = p_args[i]->as_integer();
					p += 2;
				}
				else
				{
					short **p_i = (short**)p;
					*p_i = &p_args[i]->iVal;
					p += 4;
				}
			}
			break;

		case VT_I1:
			{
				if (!m_byref_args[i])
				{
					char *p_i = (char*)p;
					*p_i = p_args[i]->as_integer();
					p += 1;
				}
				else
				{
					char **p_i = (char**)p;
					*p_i = &p_args[i]->cVal;
					p += 4;
				}
			}
			break;

		case VT_R8:
			{
				if (!m_byref_args[i])
				{
					double *p_i = (double*)p;
					*p_i = p_args[i]->as_double();
					p += 8;
				}
				else
				{
					double **p_i = (double**)p;
					*p_i = &p_args[i]->dblVal;
					p += 4;
				}
			}
			break;

		default:
			_ASSERT(0);
			p += 4;
		}
	}

// call function --------------------------------------------------------------

	int n_warn	= _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	int n_error	= _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	int n_assert= _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);

	try
	{
		switch (m_ret_type)
		{
		case VT_EMPTY:
			{
				void (__stdcall *pfn)() = (DLL_VOID) m_fp;
				pfn();
			}
			break;

		case VT_R8:
			{
				double (__stdcall *pfn)() = (DLL_DOUBLE) m_fp;
				*p_result = pfn();
			}
			break;

		default:
			{
				int (__stdcall *pfn)() = (DLL_INT) m_fp;
				*p_result = pfn();
			}
		}
	}
	catch(...)
	{
		return false;
	}

	_CrtSetReportMode(_CRT_WARN, n_warn);
	_CrtSetReportMode(_CRT_ERROR, n_error);
	_CrtSetReportMode(_CRT_ASSERT, n_assert);

// get back byref strings -----------------------------------------------------

	n = 0;
	for (i = 0; i < n_args; i++)
	{
		if (p_args[i]->get_scalar_ptr()->vt != VT_BSTR) continue;

		if (m_byref_args[i]) (*p_args[i]) = strings[n];
		n++;
	}

	return true;
}


#ifdef _DEBUG
void c_dll_func::assert_valid()
{
	_ASSERT(!IsBadWritePtr(this, sizeof(c_dll_func)));
	_ASSERT(m_hmodule);
	_ASSERT(m_fp);
	_ASSERT(m_dll_name.get_length());
}
#endif

void c_dll_caller::exec(c_variable* p_result)
{
// calc arguments -------------------------------------------------------------

	c_array<c_variable> args;
	c_expression::prepare_args(&m_expressions, &args, m_pcall_stack, m_pengine, false);

// get argument pointers ------------------------------------------------------

	c_ptr_array<c_variable>	ptr_args;
	get_argument_pointers(ptr_args, args);

// call -----------------------------------------------------------------------

	if (!m_pdll_func->exec(ptr_args.get_size(), ptr_args.get_data(), p_result))
	{
		m_pengine->runtime_error(CUR_ERR_LINE,
								"runtime error calling '%s' DLL routine.",
								m_pdll_func->get_routine_name());
	}
}
