#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include "c_variable.h"
//#include "..\config.h"
#include "c_array.h"
#include "c_str_array.h"

#define DLG_WIDTH			400
#define FIRST_STATIC_ID		300
#define FIRST_EDIT_ID		400
#define DLG_CLASS_NAME		"BeeInputDlg"

class c_engine;

class c_input_dialog
{
	HINSTANCE			m_hinst;
	c_engine*			m_pengine;
	c_array<c_variable>	m_vars;
	c_str_array			m_titles;
	c_str_array			m_values;

	c_array<HWND>		m_Statics;
	c_array<HWND>		m_Edits;

	bool			m_bcontinue;

	HWND			m_hwnd;

	HWND			m_hWndOK;								// OK button
	HWND			m_hWndCancel;							// Cancel button

	void prepare_results();									// reads input from edits to m_values

	HWND create_static	(RECT* lpRect, const char* p_title, int n, HINSTANCE h_inst);
	HWND create_edit	(RECT* lpRect, const char* p_title, int n, HINSTANCE h_inst);
	HWND create_button	(RECT* lpRect, const char* p_title, int ID, HINSTANCE h_inst);

	static LRESULT __stdcall wndProc(
		HWND hwnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);

	void on_OK();
	void on_Cancel();

	HFONT m_hfont;

public:
	c_input_dialog(c_engine* p_engine);
	~c_input_dialog();

	void show();
	void hide();

	void SetTitle(LPCTSTR p_title);
	void Add(LPCTSTR p_title, c_variable* p_var);

	LPCTSTR GetValue(int i);

	bool get_continue() {return m_bcontinue;}

	void create(HINSTANCE h_inst);							// creates the window itself
	void create_controls(HINSTANCE h_inst);					// creates window's children

	void reset_contents();

#ifdef _DEBUG
	void assert_valid();
#endif

	friend class c_engine;
};

#endif