#ifndef TERMINAL_H
#define TERMINAL_H

#include "c_string.h"
#include "c_str_array.h"

#define TERMINAL_CLASS_NAME	"BeeTerminal"

class c_engine;

class c_terminal
{
	c_engine*	m_pengine;
	c_str_array	m_strings;

	c_string	m_input_string;

	int			m_nchar_height;
	int			m_nchar_width;
	int			m_ncaret_pos;						// caret position inside input string
	int			m_nlongest_string;

	bool		m_bcaret_visible;

	POINT		m_scroll_pos;

	bool		m_bcontinue;

	void update_caret();
	void calc_char_size();

	void on_paint();
	void on_key_down(WPARAM wParam);
	void on_char(WPARAM wParam);

	void on_enter();

	void update_scroll_bars();

	void update_max_length();

	void ensure_visible_vert();
	void ensure_visible_horz();

	void update_last_line();

	void OnHScroll(WPARAM wParam, LPARAM lParam);
	void OnVScroll(WPARAM wParam, LPARAM lParam);

	static LRESULT __stdcall wndProc(
		HWND hwnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);

public:
	HWND m_hwnd;

	c_terminal(c_engine* p_engine);
	virtual ~c_terminal();

	void on_line_count_changed();
	void create(HINSTANCE h_inst);
	void append(const char* p_string);
	void clear();

	void get_text(c_string* p_str);
	void show();
	void hide();
	void request_input();
	const char* get_input_string();

	bool get_continue() {return m_bcontinue;}

#ifdef _DEBUG
	void assert_valid();
#endif

	friend class c_engine;
};

#endif
