#ifndef CONFIG_H
#define CONFIG_H

class c_variable;
class c_string;
class c_engine;

enum class run_mode
{
	run_undef		= 0,
	run_run			= 1,
	run_step_out	= 2,
	run_step_into	= 3,
	run_step_over	= 4,
	run_to_line		= 5
};

#define MAX_ERR_LENGTH		512			// max length of error report string
#define CUR_ERR_LINE		-1			// m_char_stream's current line
#define NO_ERR_LINE			-2			// don't report line

#define DEBUG_WND_CLASS_NAME	"DEBUG_WND_CLASS_NAME"
#define HTML_HELP_SUPPORT

typedef void (__stdcall *PEXTENSION_FUNCTION_FUNCTION)(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
typedef void (__stdcall *PEXTENSION_SUB_FUNCTION)(int nargs, c_variable** pargs, c_engine* p_engine);
typedef bool (*CHECK_ROUTINE)(int, VARENUM*, c_string*, c_engine*);

#endif