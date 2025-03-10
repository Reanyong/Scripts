#pragma once
#include "c_char_stream.h"
#include "token.h"
#include "c_call_stack.h"
#include "extensions.h"
#include "c_sub_table.h"
#include "c_atom_table.h"
#include "c_expression.h"
#include "c_dim_atom.h"
#include "c_variable.h"
#include "c_ext_func.h"
#include "c_if_atom.h"
#include "c_routine_entry_atom.h"
#include "c_vector_table.h"
#include "c_disp_get.h"
#include "c_dll_func.h"
#include "c_exit_atom.h"
#include "c_com_hint.h"

#include "config.h"

#include "Terminal.h"
#include "InputDialog.h"



#include "pch.h"

#define		MAX_TERMINAL_STRINGS 256

// these codes stop branches --------------------------------------------------

#define		TO_GO		0x0000
#define		ERR_		0x0001
#define		SUCCESS		0x0002
#define		ELSE		0x0004
#define		ENDIF		0x0008
#define		NEXT		0x0010
#define		WEND		0x0020
#define		ENDSUB		0x0040
#define		LOOP		0x0080
#define		EOF_TOKEN	0x0100
#define		ENDFUNCTION	0x0100
#define		CASE		0x0200
#define		ENDSELECT	0x0400

#define		SINGLE_ATOM	0xFFFF	// process only one atom. (for singleline if)

typedef void (__stdcall *ENGINE_NOTIFY_FN)(void*);

// misc helpers ---------------------------------------------------------------

const char* get_type_name(VARENUM vt);
void get_type_name(c_variable* pv, c_string* p_str);
SAFEARRAY* safearray_from_variant(const VARIANT* p);

void get_argument_pointers(c_ptr_array<c_variable>& ptr_args, c_array<c_variable>& args);
void get_dispatch_type_info(ITypeInfo** ppti, TYPEATTR** ppta);

struct bar_array
{
	SAFEARRAY*	m_parray;
	c_string	m_name;
	int			m_nupdate;
	bool		m_bcalls_tsprim;	// close, open, etc. keywords from TSPrim.DLL
	int			m_nchild;

	bar_array()
	{
		m_parray		= 0;
		m_nupdate		= 0;
		m_nchild		= -1;
		m_bcalls_tsprim	= false;
	}

	void operator = (bar_array& b)
	{
		m_parray		= b.m_parray;
		m_name			= b.m_name;
		m_nupdate		= b.m_nupdate;
		m_nchild		= b.m_nchild;
		m_bcalls_tsprim	= b.m_bcalls_tsprim;
	}
};

//-----------------------------------------------------------------------------

class c_engine
{
	c_char_stream				m_char_stream;

	c_call_stack				m_call_stack;
	c_name_table				m_ext_consts;
	c_atom_table				m_atom_table;
	c_array<extension_function>	m_ext_funcs;
	c_array<extension_sub>		m_ext_subs;
	c_array<c_dll_func>			m_dll_funcs;
	c_vector_table				m_vector_table;

	// per-routine parsing stuff

	c_array<c_routine_entry>	m_sub_table;						// array of position where routines start
	c_routine_entry_atom*		m_pcur_routine_entry;				// entry to routine currently parsed
	c_sub_namespace				m_sub_namespace;					// VarName-VarType pares for routine currently parsed
	c_sub_namespace				m_global_namespace;					// same for global context

	c_token						prevtok;							// previous token
	c_token						curtok;								// current token

	HFONT						m_hfont;							// terminal window font

public:
	//CBasicEditWnd*				m_pwnd;
protected:
	c_string					m_str_error;

	c_terminal*					m_pTerminal;						// terminal for 'input' & 'print'
	c_input_dialog*				m_pInputDialog;

	c_com_hint					m_com_hint;

	c_name_table				m_ext_objects;
	c_array<bool>				m_ext_obj_flags;					// true if default
	bool						m_bstopped;							// stop was called
	bool						m_brun;								// inside run operation

	void on_end_execution();

// parsing --------------------------------------------------------------------

	// this is the list of all the expressions which call functions.
	// during parsing we save names of the functions and pointers to expressions
	// that have to call them. after parsing is done and all routine entries are
	// ready, we set routine entries to these expressions.
	c_ptr_array<c_expression>	m_func_expressions;
	c_str_array					m_func_names;
	c_array<int>				m_line_numbers;


	int	m_nentry_pos;												// position inside source where main program (not subs) begin

	bool _pre_parse();												// enumerate subs. find entry point.
	void prepare_global_namespace();
	bool prepare_dll_routines();

	bool parse();													// main parsing
	DWORD _parse      (c_vector_table& last, DWORD stop_at = 0);	// parsing iteration
	bool _post_parse();												// join goto-label, call-sub

	DWORD parse_option();

	DWORD parse_if    (c_vector_table& last, DWORD stop_at = 0);
	DWORD _then       (c_vector_table& last, c_if_atom* p_if);
	DWORD _else       (c_vector_table& last, c_if_atom* p_if);

	DWORD parse_assign(c_vector_table& last, DWORD stop_at = 0);
	DWORD parse_end   (c_vector_table& last, DWORD stop_at);

	DWORD parse_print (c_vector_table& last, DWORD stop_at);
	DWORD parse_input (c_vector_table& last, DWORD stop_at);

	DWORD parse_for   (c_vector_table& last, DWORD stop_at);
	DWORD parse_while (c_vector_table& last, DWORD stop_at);

	DWORD parse_do    (c_vector_table& last, DWORD stop_at);
	DWORD parse_loop  (c_vector_table& last, DWORD stop_at);

	DWORD parse_dialog(c_vector_table& last, DWORD stop_at);

	DWORD parse_dim   (c_vector_table& last, DWORD stop_at);
	DWORD parse_select(c_vector_table& last, DWORD stop_at);

	DWORD parse_beep  (c_vector_table& last, DWORD stop_at);
	DWORD parse_cls   (c_vector_table& last, DWORD stop_at);

	DWORD parse_routine			(bool b_is_sub);
	DWORD parse_endsub			(c_vector_table& last, DWORD stop_at);
	DWORD parse_endfunction		(c_vector_table& last, DWORD stop_at);
	DWORD parse_call_routine	(c_vector_table& last, DWORD stop_at, bool b_is_sub);

	DWORD create_exit_atom		(c_vector_table& last, DWORD stop_at, exit_type type);
	DWORD parse_exit_sub		(c_vector_table& last, DWORD stop_at);
	DWORD parse_exit_function	(c_vector_table& last, DWORD stop_at);
	DWORD parse_exit_for		(c_vector_table& last, DWORD stop_at);
	DWORD parse_exit_while		(c_vector_table& last, DWORD stop_at);
	DWORD parse_exit_do			(c_vector_table& last, DWORD stop_at);

	DWORD parse_ext_sub(c_vector_table& last, DWORD stop_at);
	DWORD parse_dll_sub(c_vector_table& last, DWORD stop_at);

	DWORD parse_next  (c_vector_table& last, DWORD stop_at);
	DWORD parse_else  (c_vector_table& last, DWORD stop_at);
	DWORD parse_endif (c_vector_table& last, DWORD stop_at);
	DWORD parse_case  (c_vector_table& last, DWORD stop_at);
	DWORD parse_endselect(c_vector_table& last, DWORD stop_at);
	DWORD parse_wend  (c_vector_table& last, DWORD stop_at);
	DWORD parse_declare(c_vector_table& last, DWORD stop_at);

	DWORD parse_label (c_vector_table& last, DWORD stop_at);
	DWORD parse_goto  (c_vector_table& last, DWORD stop_at);

	c_ext_func*		_ext_func();
	c_dll_caller*	_dll_caller();

// LPDISPATCH-related parsing -------------------------------------------------

	ITypeInfo*		get_type_info(IDispatch* lpd);

	bool			is_property_get_context();
	bool			parse_disp_levels(c_string* p_parent,
									  c_disp_levels* p_levels,
									  unsigned int n_mask,
									  ITypeInfo* def_pti,
									  bool b_get);

	bool			check_arg_count(FUNCDESC* pfd,
									c_ptr_array<c_expression>* p_expressions,
									const char* p_method_name);

	c_disp_get*		disp_get(bool& b_var, ITypeInfo* def_pti);
	DWORD			parse_disp_set(bool* p_bvar, c_vector_table& last, DWORD stop_at, ITypeInfo* def_pti);
	c_dll_func*		get_dll_routine(const char* p_name, bool b_sub);

	bool parse_arg_list(c_expressions& expressions,
						const char* p_name,
						CHECK_ROUTINE p_check,
						bool b_sub);

	bool parse_arg_declaration(	bool b_sub,
								const char*			p_name,
								c_array<c_dim_entry>* p_args,
								VARENUM*			p_type,
								bool				b_dll = false);

// expression parsing ---------------------------------------------------------

	c_expression* _primary();
	c_expression* _term();
	c_expression* _mathem();
	c_expression* _bool();
	c_expression* _expr_and();
	c_expression* _expr();
	VARENUM get_expr_type(c_expression* p_expr);
	bool parse_dims(c_ptr_array<c_expression>& dims);

//-------------------------------------

	int  gettok();
	void create_terminal(HINSTANCE h_inst);
	void create_dialog(HINSTANCE h_inst);
	void create_dbg_wnd();

	bool is_routine			(const char* p_name, bool b_sub);
	VARENUM get_routine_type(const char* p_name);
	bool is_sub				(const char* p_name);
	bool is_function		(const char* p_name);
	int  get_routine_arg_count(const char* p_name);

	bool is_ext_const		(const char* p_name);
	bool is_ext_object		(const char* p_name);
	bool is_ext_func		(const char* p_name);
	bool is_ext_sub			(const char* p_name);
	bool is_type			(const char* p_name);
	bool is_object_variable	(const char* p_name, CLSID* p_clsid, GUID* p_libid);
	VARENUM get_type		(const char* pstr);
	bool is_declared_local_var(const char* p_name);
	bool is_declared_array	(const char* p_name);

	bool is_dll_routine		(const char* p_name);
	bool is_dll_func		(const char* p_name);
	bool is_dll_sub			(const char* p_name);

	// checks if a string is a valid identifier (i.e. conforms to naming
	// convention and is not yet declared).
	bool can_be_identifier	(const char* p_name, bool b_verbose);

	bool get_ext_function(const char* p_name, extension_function& func);
	bool get_ext_sub(const char* p_name, extension_sub& sub);
	bool get_ext_constant(const char* p_name, c_variable& var);

	void internal_reset();


public:
	c_engine();
	~c_engine();

	void reset();

// Debugger Help support ------------------------------------------------------

private:
	c_string	m_dbg_help_file_name;

public:
	inline const char* get_dbg_help();
	inline void set_dbg_help(const char* p_file_name);

// check_syntax ---------------------------------------------------------------

public:
	bool check_syntax(const char* p_code);

// run ------------------------------------------------------------------------

public:
	inline bool run_go();
	inline bool step_over();
	inline bool step_into();
	inline bool step_out();
	inline bool run_to_cursor(int n_line);
	bool run_routine(
					const char* p_name,
					c_variable* p_args,
					int n_args,
					bool b_stop,
					VARIANT* pRes);

	bool run_routine(
					const char* p_name,
					const VARIANT* p_safearray,
					bool b_stop,
					VARIANT* pRes);

	bool run_routine(const char* p_string, bool b_dbg,
					VARIANT* pRes);

	void stop();
	bool stopped() {return m_bstopped;}

	run_mode get_run_mode();

//-----------------------------------------------------------------------------

	bool is_waiting();												// true if in Step mode
	bool has_code_parsed()	{return m_atom_table.has_code_parsed();}
	int  get_cur_line()	{return m_atom_table.get_cur_line();}		// execution line

	const char* get_error()			{return m_str_error.get_buffer();}
	const char* get_error_descr();
	void get_line_context(int n, int* p_nline, c_string* p_routine);
	void get_error_context(int* p_nline, c_string* p_routine);
	void clear_error()	{m_str_error.empty();}

	bool dbg_wnd_visible();
	void dbg_wnd_visible(bool b);
	void dbg_print(const char* p_str);

private:
	bool m_ballow_debugging;

public:
	void set_allow_debugging(bool b)	{m_ballow_debugging = b;}
	bool get_allow_debugging()			{return m_ballow_debugging;}

	void allow_runtime(bool b)			{m_bruntime = b;}

// font -----------------------------------------------------------------------

	HFONT	get_terminal_font()			{return m_hfont;}
	void	set_terminal_font(HFONT f)	{DeleteObject(m_hfont); m_hfont = f;}

// ext function syntax --------------------------------------------------------

public:

	HWND			get_terminal_HWND();
	c_terminal*		get_terminal();
	c_input_dialog*	get_input_dialog();

	const char* get_buffer();
	const char* get_internal_buffer();
	bool set_buffer(const char* pData);									// passes code to engine and parses it
	void set_edit_buffer(const char* pData);							// sets text in editor window
	bool ready()	{return m_atom_table.ready();}						// ready to execute

// variable watch -------------------------------------------------------------

	bool is_valid_hint(const char* p_name);

	int  get_local_var_count();										// returns number of local variables in current scope
	bool get_local_var(	int n,
						c_variable* p_var,
						const char** pp_name);

	int  get_global_var_count();
	bool get_global_var(int n,
						c_variable* p_var,
						const char** pp_name);

	void get_var_watch(const char* p_name, c_string& str);

	bool eval_watch(const char* p_str, c_variable* p_val);

	void watch_obj_members(	LPDISPATCH lpd,
							c_str_array* p_members,
							c_array<c_variable>* p_values);

	void list_all_obj_members(bool b_functions, bool b_subs, bool b_props,
								c_str_array* p_array);

// breakpoints ----------------------------------------------------------------

	int  set_breakpoint(int n_line, bool b_from_dbgwnd = true);
	int  remove_breakpoint(int n_line, bool b_from_dbgwnd = true);
	int  toggle_breakpoint(int n_line, bool* p_bset = 0, bool b_from_dbgwnd = true);
	int  get_breakpoint_count();
	int  get_breakpoint(int n_index);
	void remove_all_breakpoints();
	bool set_bp_condition(int n_line, const char* p_expr, bool b_from_dbgwnd = true);
	void get_breakpoint_context(int n, int* p_nline, c_string* p_routine);
	void format_bp_list(c_str_array* p_list);
	void set_bp_list(c_str_array* p_list);

// error ----------------------------------------------------------------------
private:
	bool m_bruntime; // should we process runtime errors?
	const char* get_VB_error(int n_code);

public:
	void error(int n_line, const char *format, ...);
	void dbg_break();
	void runtime_error(int n_line, const char *format, ...);
	void runtime_error(HRESULT hr);
	void disp_error(int n_line, HRESULT hr, EXCEPINFO* p_exc, const char *p_descr = 0);

	void on_terminal_enter();	//called by terminal window
	void on_window_enter();		//called by dialog window

// syntax coloring ------------------------------------------------------------

	bool is_keyword(const char* p_str, int n_length);
	bool is_routine(const char* p_str, int n_length);
	void capitalize_keyword(char* p);

// option explicit ------------------------------------------------------------

private:
	bool m_bexplicit;
	bool m_bexplicit_set;
	bool variable_declared(const char* p_name);

public:
	inline bool is_option_explicit();

// hints ----------------------------------------------------------------------

private:
	c_str_array	m_hint_kwds;
	c_str_array	m_hint_descs;
	int n_default_hints;

public:
	bool add_hint(const char* p_keyword, const char* p_hint);
	void delete_hint(const char* p_keyword);
	void delete_all_hints();	// all, but not the default ones
	void get_hint(const char* p_keyword, const char** pp_hint);
	void get_hint(c_str_array* p_levels, const char** pp_hint, const char* p_type);

	void list_members(	const char* p_class_name,
						c_str_array* p_levels,
						c_str_array* p_members,
						c_array<bool>*	p_methods);

// extensions -----------------------------------------------------------------

private:

	void add_standard_extension_constants();						// called automatically (standard constants)
	void add_standard_extension_functions();						// called automatically (standard functions)
	void add_standard_extension_subs();								// called automatically (standard subs)
	void add_standard_hints();

public:

	c_variable* get_extension_constant(const char* p_name);

//	bool set_global_var(const char* p_name, const VARIANT* p_var);
//	bool get_global_var(const char* p_name, VARIANT** pp_var);

	bool add_extension_constant(const char* lpszname, c_variable* p_val);
	bool add_extension_constant(const char* lpszname, int		val);
	bool add_extension_constant(const char* lpszname, double	val);
	bool add_extension_constant(const char* lpszname, const char*	val);

	bool add_extension_function(const char* lpszname,					// function name
								PEXTENSION_FUNCTION_FUNCTION pfunc,	// implementation function
								CHECK_ROUTINE p_check);				// function to check passes arguments

	bool add_extension_sub(		const char* lpszname,					// sub name
								PEXTENSION_SUB_FUNCTION psub,		// implementation function
								CHECK_ROUTINE p_check);				// function to check passes arguments

	bool add_object(const char* p_name, LPDISPATCH p_o, bool b_default);
	IDispatch* get_object(const char* p_name);
	IDispatch* get_default_parent(const char* p_member);
	bool is_added_object(IDispatch* pd);

	void list_ext_consts(c_str_array& a);
	void list_ext_funcs(c_str_array& a);
	void list_ext_subs(c_str_array& a);

// terminal lines count -------------------------------------------------------

private:
	int m_nterminal_lines;

public:
	void set_terminal_lines(int n);
	int  get_terminal_lines();

// persistent DBG watch -------------------------------------------------------

private:
	c_str_array					m_dbg_watches;
	// Valid_TagName

public:
	const char*					dbg_watch_get(int n);
	bool						dbg_watch_set(int n, const char* p);
	bool						dbg_watch_remove(int n);
	int							dbg_watch_get_count();
	void						dbg_watch_clear();

// notifications --------------------------------------------------------------

private:
	void*						m_pnotify_ptr;
	ENGINE_NOTIFY_FN			m_pfn_step;
	ENGINE_NOTIFY_FN			m_pfn_end;
	ENGINE_NOTIFY_FN			m_pfn_close;
	ENGINE_NOTIFY_FN			m_pfn_about;

public:

	void set_notify_ptr(void* p)					{m_pnotify_ptr	= p;}
	void set_step_notify(ENGINE_NOTIFY_FN p_fn)		{m_pfn_step		= p_fn;}
	void set_end_notify(ENGINE_NOTIFY_FN p_fn)		{m_pfn_end		= p_fn;}
	void set_close_notify(ENGINE_NOTIFY_FN p_fn)	{m_pfn_close	= p_fn;}
	void set_about_notify(ENGINE_NOTIFY_FN p_fn)	{m_pfn_about	= p_fn;}

	void on_dbg_close() {if (m_pfn_close) m_pfn_close(m_pnotify_ptr);}
	void on_dbg_about()	{if (m_pfn_about) m_pfn_about(m_pnotify_ptr);}

#ifdef _DEBUG
	void assert_valid();
#endif

	friend class c_call_stack;
	friend class c_atom_table;
};

inline bool c_engine::is_option_explicit()
{
	return m_bexplicit;
}

// inlines --------------------------------------------------------------------

inline void c_engine::set_dbg_help(const char* p_file_name)
{
	m_dbg_help_file_name	= p_file_name;
}

const char* c_engine::get_dbg_help()
{
	return m_dbg_help_file_name.get_buffer();
}

inline bool c_engine::run_go()
{
	if (m_brun) return true;
	m_brun = true;

	m_bstopped = false;
	m_str_error.empty();
	bool b = m_atom_table.run_go();

//	if (!m_atom_table.m_pcur_atom &&
//		!m_atom_table.m_binput) // make sure we stopped not because of 'input'
//				on_end_execution();

	m_brun = false;
	//if (m_pwnd && is_waiting()) m_pwnd->PostStepUpdate();

	return b;
}

inline bool c_engine::step_over()
{
	if (m_brun) return true;
	m_brun = true;

	m_bstopped = false;
	m_str_error.empty();
	bool b = m_atom_table.step_over();

//	if (!m_atom_table.m_pcur_atom &&
//		!m_atom_table.m_binput) // make sure we stopped not because of 'input'
//				on_end_execution();

	if (m_pfn_step) m_pfn_step(m_pnotify_ptr);
	m_brun = false;
	//if (m_pwnd && is_waiting()) m_pwnd->PostStepUpdate();

	return b;
}

inline bool c_engine::step_into()
{
	if (m_brun) return true;
	m_brun = true;

	m_bstopped = false;
	m_str_error.empty();
	bool b = m_atom_table.step_into();

//	if (!m_atom_table.m_pcur_atom &&
//		!m_atom_table.m_binput) // make sure we stopped not because of 'input'
//				on_end_execution();

	if (m_pfn_step) m_pfn_step(m_pnotify_ptr);
	m_brun = false;
	//if (m_pwnd && is_waiting()) m_pwnd->PostStepUpdate();

	return b;
}

inline bool c_engine::step_out()
{
	if (m_brun) return true;
	m_brun = true;

	m_bstopped = false;
	m_str_error.empty();
	bool b = m_atom_table.step_out();

//	if (!m_atom_table.m_pcur_atom &&
//		!m_atom_table.m_binput) // make sure we stopped not because of 'input'
//				on_end_execution();

	if (m_pfn_step) m_pfn_step(m_pnotify_ptr);
	m_brun = false;
	//if (m_pwnd && is_waiting()) m_pwnd->PostStepUpdate();

	return b;
}

inline bool c_engine::run_to_cursor(int n_line)
{
	if (m_brun) return true;
	m_brun = true;

	m_bstopped = false;
	m_str_error.empty();
	bool b = m_atom_table.run_to_cursor(n_line);

//	if (!m_atom_table.m_pcur_atom &&
//		!m_atom_table.m_binput) // make sure we stopped not because of 'input'
//				on_end_execution();

	if (m_pfn_step) m_pfn_step(m_pnotify_ptr);
	m_brun = false;
	//if (m_pwnd && is_waiting()) m_pwnd->PostStepUpdate();

	return b;
}

//-----------------------------------------------------------------------------

#define PUSH_CURTOK() \
	int _n_char_stream_pos = m_char_stream.pos();\
	c_token _curtok = curtok;\
	c_token _prevtok = prevtok;\

#define POP_CURTOK() \
{\
	m_char_stream.pos(_n_char_stream_pos);\
	curtok = _curtok;\
	prevtok = _prevtok;\
}\

