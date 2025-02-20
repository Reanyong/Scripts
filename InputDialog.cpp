#include "pch.h"

#include <windows.h>
#include <crtdbg.h>
#include "InputDialog.h"
#include "c_atom_table.h"
#include "c_engine.h"

c_input_dialog::c_input_dialog(c_engine* p_engine)
{
	m_hwnd = NULL;
	m_hinst			= 0;
	m_pengine		= p_engine;
	m_bcontinue		= true;

	m_hWndOK		= 0;
	m_hWndCancel	= 0;

	m_hfont			= (HFONT)GetStockObject(DEFAULT_GUI_FONT);
}

c_input_dialog::~c_input_dialog()
{
	DestroyWindow(m_hwnd);
}

void c_input_dialog::SetTitle(LPCTSTR p_title)
{
	SetWindowText(m_hwnd, p_title);
}

void c_input_dialog::Add(LPCTSTR p_title, c_variable* p_var)
{
	m_titles.add(p_title);
	m_vars.add(*p_var);

	RECT rect;		// rectangle for controls
	HWND h_wnd;

	c_string str;
	int n = m_titles.get_size() - 1;

	// static--------------------------------

	rect.top = n * 25 + 5;
	rect.bottom = rect.top + 20;
	rect.left = 5;
	rect.right = DLG_WIDTH / 2 - 5;

	h_wnd = create_static(&rect, m_titles[n], FIRST_STATIC_ID + n, m_hinst);
	m_Statics.add(h_wnd);
	SendMessage(h_wnd, WM_SETFONT, (WPARAM)m_hfont, 0);

	// edit ---------------------------------

	rect.left = DLG_WIDTH/2 ;
	rect.right = DLG_WIDTH - 10;

	m_vars[n].as_string(str);

	h_wnd = create_edit(&rect, str, FIRST_EDIT_ID + 1, m_hinst);
	m_Edits.add(h_wnd);
	SendMessage(h_wnd, WM_SETFONT, (WPARAM)m_hfont, 0);
}

LPCTSTR c_input_dialog::GetValue(int i)
{
	return m_values[i];
}

void c_input_dialog::hide()
{
	ShowWindow(m_hwnd, SW_HIDE);
//	reset_contents();
}

void c_input_dialog::show()
{
// show visible ---------------------------------

	c_string str;

	for (int i = 0; i < m_titles.get_size(); i++)
	{
		ShowWindow(m_Edits[i],   SW_SHOW);
		ShowWindow(m_Statics[i], SW_SHOW);

		m_vars[i].as_string(str);

		SetWindowText(m_Statics[i], m_titles[i]);
		SetWindowText(m_Edits[i], str);
	}

// hide invisible -------------------------------

//	for (i = m_titles.get_size(); i < MAX_INPUT_VARS; i++)
//	{
//		ShowWindow(m_Edits[i], SW_HIDE);
//		ShowWindow(m_Statics[i], SW_HIDE);
//	}

// move buttons ---------------------------------

	RECT rect;

	rect.left = DLG_WIDTH/2;
	rect.top = m_titles.get_size() * 25 + 10;
	rect.right = DLG_WIDTH * 3/4 - 10;
	rect.bottom = m_titles.get_size() * 25 + 34;

	MoveWindow(
		m_hWndCancel,
		DLG_WIDTH/2,			// x
		m_titles.get_size() * 25 + 10,		// y
		DLG_WIDTH/4 - 5,		// width
		25,						// height
		false);					// repaint

	MoveWindow(
		m_hWndOK,
		DLG_WIDTH * 3/4,		// x
		m_titles.get_size() * 25 + 10,		// y
		DLG_WIDTH/4 - 10,		// width
		25,						// height
		false);					// repaint

// window size ----------------------------------

	SetWindowPos(
		m_hwnd,						// handle to window
		HWND_TOPMOST,				// placement-order handle
		100,						// horizontal position
		100,						// vetical position
		DLG_WIDTH,					// width
		m_titles.get_size() * 25 + 67,			// height
		SWP_ASYNCWINDOWPOS);		// window-positioning options

	ShowWindow(m_hwnd, SW_SHOW);
}

void c_input_dialog::create(HINSTANCE h_inst)
{
	m_hinst = h_inst;

	WNDCLASS clsx;

	clsx.style			= CS_HREDRAW | CS_VREDRAW;
	clsx.lpfnWndProc	= wndProc;
	clsx.cbClsExtra		= 0;
	clsx.cbWndExtra		= 0;
	clsx.hInstance		= h_inst;
	clsx.hIcon			= 0;//__h_dialog_icon;
	clsx.hCursor		= LoadCursor(0, IDC_ARROW);
	clsx.hbrBackground	= GetSysColorBrush(COLOR_BTNFACE);
	clsx.lpszMenuName	= 0;
	clsx.lpszClassName	= DLG_CLASS_NAME;

	ATOM atom = RegisterClass(&clsx);
//	_ASSERT(atom != 0); // this actually can fail if we try to register more than once.

	m_hwnd = CreateWindow(
		DLG_CLASS_NAME,						// registered class name
		"",									// window name
		WS_CLIPCHILDREN|WS_POPUP|
		WS_CAPTION|WS_DLGFRAME|
		WS_SYSMENU|WS_MINIMIZEBOX,			// window style
		100,								// horizontal position of window
		100,								// vertical position of window
		400,								// window width
		300,								// window height
		0,									// handle to parent or owner window
		0,									// menu handle or child identifier
		h_inst,								// handle to application instance
		0);									// window-creation data

	_ASSERT(m_hwnd != 0);
	UpdateWindow(m_hwnd);

#ifdef _WIN64
	SetWindowLong(m_hwnd, GWLP_USERDATA, (LONG)(LONGLONG)this);
#else
	SetWindowLong(m_hwnd, GWL_USERDATA, (long)this);
#endif // _WIN64

}

void c_input_dialog::create_controls(HINSTANCE h_inst)
{
//	_ASSERT(m_titles.get_size() == m_values.get_size();

	RECT rect;		// rectangle for controls
//	HWND h_wnd;

/*
	c_string str;
	for (int i = 0; i < m_titles.get_size(); i++)
	{
		// static--------------------------------

		rect.top = i * 25 + 5;
		rect.bottom = rect.top + 20;
		rect.left = 5;
		rect.right = DLG_WIDTH / 2 - 5;

		h_wnd = create_static(&rect, m_titles[i], FIRST_STATIC_ID + i, h_inst);
		m_Statics.add(h_wnd);
//		SendMessage(m_Statics[i], WM_SETFONT, (WPARAM)__h_dialog_font, 0);

		// edit ---------------------------------

		rect.left = DLG_WIDTH/2 ;
		rect.right = DLG_WIDTH - 10;

		m_vars[i].as_string(str);

		h_wnd = create_edit(&rect, str, FIRST_EDIT_ID + 1, h_inst);
		m_Edits.add(h_wnd);
//		SendMessage(m_Edits[i], WM_SETFONT, (WPARAM)__h_dialog_font, 0);
	}
*/

// OK & Cancel ----------------------------------

	rect.left = DLG_WIDTH/2;
	rect.top = m_titles.get_size() * 25 + 10;
	rect.right = DLG_WIDTH * 3/4 - 10;
	rect.bottom = m_titles.get_size() * 25 + 34;

	m_hWndCancel = create_button(&rect, "Terminate", IDCANCEL, h_inst);
	SendMessage(m_hWndCancel, WM_SETFONT, (WPARAM)m_hfont, 0);

	rect.left = DLG_WIDTH * 3/4;
	rect.top = m_titles.get_size() * 25 + 10;
	rect.right = DLG_WIDTH - 10;
	rect.bottom = m_titles.get_size() * 25 + 34;

	m_hWndOK = create_button(&rect, "OK", IDOK, h_inst);
	SendMessage(m_hWndOK, WM_SETFONT, (WPARAM)m_hfont, 0);
}

HWND c_input_dialog::create_static(RECT* lpRect,
								   const char* p_title,
								   int i,
								   HINSTANCE h_inst)
{
	return CreateWindowEx(
		0,								// extended window style
		"STATIC",						// registered class name
		p_title,						// window name
		WS_VISIBLE | WS_CHILD,			// window style
		lpRect->left,					// horizontal position of window
		lpRect->top,					// vertical position of window
		lpRect->right - lpRect->left,	// window width
		lpRect->bottom - lpRect->top,	// window height
		m_hwnd,							// handle to parent or owner window
		(HMENU)(INT64)i,						// menu handle or child identifier
		h_inst,							// handle to application instance
		0);								// window-creation data
}

HWND c_input_dialog::create_edit(RECT* lpRect,
								 const char* p_title,
								 int i,
								 HINSTANCE h_inst)
{
	return CreateWindowEx(
		WS_EX_CLIENTEDGE,				// extended window style
		"EDIT",							// registered class name
		p_title,						// window name
		WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP,
		lpRect->left,					// horizontal position of window
		lpRect->top,					// vertical position of window
		lpRect->right - lpRect->left,	// window width
		lpRect->bottom - lpRect->top,	// window height
		m_hwnd,							// handle to parent or owner window
		(HMENU)(INT64)i,						// menu handle or child identifier
		h_inst,							// handle to application instance
		0);								// window-creation data
}

HWND c_input_dialog::create_button (RECT* lpRect,
									const char* p_title,
									int ID,
									HINSTANCE h_inst)
{
	return CreateWindowEx(
		0,								// extended window style
		"BUTTON",						// registered class name
		p_title,						// window name
		ID == IDOK ? BS_DEFPUSHBUTTON | WS_VISIBLE | WS_CHILD : WS_VISIBLE | WS_CHILD |WS_TABSTOP,
		lpRect->left,					// horizontal position of window
		lpRect->top,					// vertical position of window
		lpRect->right - lpRect->left,	// window width
		lpRect->bottom - lpRect->top,	// window height
		m_hwnd,							// handle to parent or owner window
		(HMENU) (INT64)ID,						// menu handle or child identifier
		h_inst,
		0);							// window-creation data
}

void c_input_dialog::prepare_results()
{
	_ASSERT(m_values.get_size() == 0);

	char buff[256];

	for (int i = 0; i < m_titles.get_size(); i++)
	{
		GetWindowText(m_Edits[i], buff, sizeof(buff));
		m_values.add(buff);
	}
}

void c_input_dialog::reset_contents()
{
	for (int i = 0; i < m_Edits.get_size(); i++)
	{
		DestroyWindow(m_Edits[i]);
		DestroyWindow(m_Statics[i]);
	}

	m_Edits.reset(true);
	m_Statics.reset(true);
	m_vars.reset(true);
	m_titles.empty();
	m_values.empty();
}

void c_input_dialog::on_OK()
{
	prepare_results();
	m_bcontinue = true;
//	SetEvent(m_pengine->get_io_event());
	m_pengine->on_window_enter();
}

void c_input_dialog::on_Cancel()
{
	prepare_results();
	m_bcontinue = false;		// stop script execution
//	SetEvent(m_pengine->get_io_event());
	m_pengine->on_window_enter();
}

LRESULT __stdcall c_input_dialog::wndProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam)
{
#ifdef _WIN64
	c_input_dialog* _this = (c_input_dialog*)(LONGLONG)GetWindowLong(hWnd, GWLP_USERDATA);
#else
	c_input_dialog* _this = (c_input_dialog*)GetWindowLong(hWnd, GWL_USERDATA);
#endif // _WIN64


	if (!_this) return DefWindowProc(hWnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_COMMAND:
		switch( LOWORD( wParam ))
		{
		case IDOK: _this->on_OK();
			break;

		case IDCANCEL: _this->on_Cancel();
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		break;

	case WM_CLOSE: _this->on_Cancel();
		break;

	case WM_GETDLGCODE: return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_WANTTAB;

	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

#ifdef _DEBUG
void c_input_dialog::assert_valid()
{
	_ASSERT(!IsBadWritePtr(this, sizeof(c_input_dialog)));
	_ASSERT(IsWindow(m_hwnd));
}
#endif

