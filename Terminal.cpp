#include "pch.h"
#include "commdlg.h"

#include <crtdbg.h>
#include <windowsx.h>
#include "Terminal.h"
#include "c_engine.h"

c_terminal::c_terminal(c_engine* p_engine)
{
	m_pengine			= p_engine;
	m_hwnd				= 0;

	m_nchar_height		= 0;
	m_nchar_width		= 0;
	m_ncaret_pos		= 0;
	m_bcaret_visible	= false;

	m_scroll_pos.x		= 0;
	m_scroll_pos.y		= 0;
	m_nlongest_string	= 0;

	m_strings.add("");
	m_bcontinue			= true;
}

c_terminal::~c_terminal()
{
	DestroyWindow(m_hwnd);
}

void c_terminal::show()
{
	m_bcontinue = true;
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
}

void c_terminal::hide()
{
	ShowWindow(m_hwnd, SW_HIDE);
}

void c_terminal::request_input()
{
	m_bcaret_visible = true;
	PostMessage(m_hwnd, WM_SETFOCUS, 0, 0);
}


void c_terminal::update_scroll_bars()
{
	RECT rect;
	GetClientRect(m_hwnd, &rect);

	int x = rect.right - rect.left;
	int y = rect.bottom - rect.top;

// show/hide ------------------------------------

	if (m_nchar_height * m_strings.get_size() < y)
		EnableScrollBar(m_hwnd, SB_VERT, ESB_DISABLE_BOTH);
	else
	{
		ShowScrollBar(m_hwnd, SB_VERT, true);
		EnableScrollBar(m_hwnd, SB_VERT, ESB_ENABLE_BOTH);
	}

	if (m_nchar_width * m_nlongest_string < x)
		EnableScrollBar(m_hwnd, SB_HORZ, ESB_DISABLE_BOTH);
	else
	{
		ShowScrollBar(m_hwnd, SB_HORZ, true);
		EnableScrollBar(m_hwnd, SB_HORZ, ESB_ENABLE_BOTH);
	}

// scroll bars ----------------------------------

	SCROLLINFO si;
	si.cbSize	= sizeof(si); 
	si.fMask	= SIF_RANGE | SIF_PAGE |SIF_POS;
	si.nPage	= m_nchar_height == 0 ? 1 : y / m_nchar_height;
	si.nMin		= 0;
	si.nMax		= m_strings.get_size();
	si.nPos		= m_scroll_pos.y;

	SetScrollInfo(m_hwnd, SB_VERT, &si, true);

	si.nPage	= m_nchar_width == 0 ? 0 : x / m_nchar_width;
	si.nMax		= m_nlongest_string;
	si.nPos		= m_scroll_pos.x;

	SetScrollInfo(m_hwnd, SB_HORZ, &si, true);
}

const char* c_terminal::get_input_string()
{
	return (LPCTSTR)m_input_string;
}

void c_terminal::create(HINSTANCE h_inst)
{
	WNDCLASS clsx;

	clsx.style			= 0;
	clsx.lpfnWndProc	= wndProc;
	clsx.cbClsExtra		= 0;
	clsx.cbWndExtra		= 0;
	clsx.hInstance		= h_inst;
	clsx.hIcon			= 0;//__h_terminal_icon;
	clsx.hCursor		= LoadCursor(0, IDC_ARROW);
	clsx.hbrBackground	= GetSysColorBrush(COLOR_WINDOW);
	clsx.lpszMenuName	= 0;
	clsx.lpszClassName	= TERMINAL_CLASS_NAME;

	ATOM atom = RegisterClass(&clsx);

	m_hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,					// extended style
		TERMINAL_CLASS_NAME,				// registered class name
		"Terminal",							// window name
		WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,
		100,								// horizontal position of window
		100,								// vertical position of window
		400,								// window width
		300,								// window height
		0,									// handle to parent or owner window
		0,									// menu handle or child identifier
		h_inst,								// handle to application instance
		0);									// window-creation data

	_ASSERT(m_hwnd != 0);

	//HMENU h_menu = LoadMenu(h_inst, MAKEINTRESOURCE(IDR_TERMINAL));
	//SetMenu(m_hwnd, h_menu);

	UpdateWindow(m_hwnd);

#ifdef _WIN64
	SetWindowLong(m_hwnd, GWLP_USERDATA, (LONG)(LONGLONG)this);
#else
	SetWindowLong(m_hwnd, GWL_USERDATA, (long)this);
#endif // _WIN64
	
}

void c_terminal::append(const char* p_string)
{
	RECT rect;
	GetClientRect(m_hwnd, &rect);

	if (m_strings.get_size() == m_pengine->get_terminal_lines())
		m_strings.remove_at(0);

	c_string str_last;
	str_last = m_strings[m_strings.get_size() - 1];
	str_last += m_input_string;
	m_strings.set_at(m_strings.get_size() - 1, str_last.get_buffer());

	m_strings.add(p_string);

	m_input_string.empty();

	int i, n;
	m_nlongest_string = 0;
	for (i = 0; i < m_strings.get_size(); i++)
	{
		n = (int)strlen(m_strings[i]);
		if (m_nlongest_string < n) m_nlongest_string = n;
	}

	if (m_strings.get_size() == m_pengine->get_terminal_lines())
		ScrollWindow(m_hwnd, 0, - m_nchar_height, 0, 0);

	if ((m_strings.get_size() - m_scroll_pos.y) * m_nchar_height >= (rect.bottom - rect.top))
	{
		ensure_visible_vert();
		update_scroll_bars();
	}
			
	update_last_line();

	if (m_scroll_pos.x != 0)
	{
		ensure_visible_horz();
		update_scroll_bars();
	}
}

void c_terminal::on_line_count_changed()
{
	while (m_strings.get_size() > m_pengine->get_terminal_lines())
		m_strings.remove_at(0);

	if (m_nchar_height)
	{
		ensure_visible_vert();
		ensure_visible_horz();
		update_scroll_bars();
		update_last_line();
	}
}

void c_terminal::clear()
{
	m_scroll_pos.x		= 0;
	m_scroll_pos.y		= 0;

	m_nlongest_string	= 0;

	m_strings.empty();
	m_strings.add("");

	m_input_string.empty();
	RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE|RDW_ERASE);

	update_scroll_bars();
}

void c_terminal::calc_char_size()
{
	HDC hdc = GetDC(m_hwnd);
	_ASSERT(hdc);

	SelectObject(hdc, m_pengine->get_terminal_font());

	SIZE size;
	GetTextExtentPoint(hdc, "0", 1, &size);
	m_nchar_width = size.cx;
	m_nchar_height = size.cy;

	ReleaseDC(m_hwnd, hdc);
}

void c_terminal::update_caret()
{
	if (m_nchar_width == 0) calc_char_size();

	if (!m_bcaret_visible)
	{
		HideCaret(m_hwnd);
		m_ncaret_pos = 0;
	}
	else
	{
		VERIFY(CreateCaret(m_hwnd, 0, m_nchar_width, m_nchar_height));

		int x = (int)strlen(m_strings[m_strings.get_size() - 1]) + m_ncaret_pos - m_scroll_pos.x;
		x *= m_nchar_width;
		int y = (m_strings.get_size() - m_scroll_pos.y - 1) * m_nchar_height;

		SetCaretPos(x, y);
		VERIFY(ShowCaret(m_hwnd));
	}
}

void c_terminal::update_max_length()
{
	int n_max_length = 0;
	for (int i = 0; i < m_strings.get_size(); i++)
		if ((unsigned)n_max_length < strlen(m_strings[i])) n_max_length = (int)strlen(m_strings[i]);

	int n_last_length = m_input_string.get_length() + (int)strlen(m_strings[m_strings.get_size() - 1]);

	if (n_max_length < n_last_length) n_max_length = n_last_length;

	if (n_max_length != m_nlongest_string)
	{
		m_nlongest_string = n_max_length;
		update_scroll_bars();
	}
}

void c_terminal::ensure_visible_vert()
{
	RECT rect;
	GetClientRect(m_hwnd, &rect);

	int n_screen_height = rect.bottom - rect.top;
	int lines_on_screen = n_screen_height / m_nchar_height;

	if (m_scroll_pos.y + lines_on_screen < m_strings.get_size())
	{
		int dy = m_strings.get_size() * m_nchar_height - (m_scroll_pos.y * m_nchar_height + n_screen_height);
		
		if (dy < m_nchar_height) dy = m_nchar_height;
		m_scroll_pos.y = m_strings.get_size() - lines_on_screen;

		ScrollWindow(m_hwnd, 0, - dy, 0, 0);
		update_last_line();
	}
}

void c_terminal::ensure_visible_horz()
{
	RECT rect, update_rect;
	GetClientRect(m_hwnd, &rect);

	int x = (m_ncaret_pos + (int)strlen(m_strings[m_strings.get_size() - 1])) * m_nchar_width;
	int n_screen_width = rect.right - rect.left;

	if (x + m_nchar_width * 2 >= n_screen_width + m_scroll_pos.x * m_nchar_width)
	{
		int chars_in_line = (rect.right - rect.left) / m_nchar_width;
		int n_new_scroll_pos = x / m_nchar_width - chars_in_line + 2;

		ScrollWindow(m_hwnd,
			-(n_new_scroll_pos - m_scroll_pos.x) * m_nchar_width,
			0, 0, 0);

		update_rect.top = rect.top;
		update_rect.bottom = rect.bottom;
		update_rect.left = (chars_in_line - (n_new_scroll_pos - m_scroll_pos.x)) * m_nchar_width;
		update_rect.right = rect.right;

		m_scroll_pos.x = n_new_scroll_pos;
		RedrawWindow(m_hwnd, &update_rect, 0, RDW_INVALIDATE|RDW_ERASE);
	}

	if (x - m_nchar_width < m_scroll_pos.x * m_nchar_width && m_scroll_pos.x > 0)
	{
		ScrollWindow(m_hwnd, m_scroll_pos.x * m_nchar_width - x, 0, 0, 0);
		m_scroll_pos.x -= m_scroll_pos.x - x / m_nchar_width;

		update_rect.top = rect.top;
		update_rect.bottom = rect.bottom;
		update_rect.left = 0;
		update_rect.right = x;

		RedrawWindow(m_hwnd, &update_rect, 0, RDW_INVALIDATE|RDW_ERASE);
	}
}

void c_terminal::update_last_line()
{
	RECT rect, update_rect;
	GetClientRect(m_hwnd, &rect);
	
	update_rect.left = rect.left;
	update_rect.right = rect.right;
	update_rect.top = (m_strings.get_size() - m_scroll_pos.y - 1) * m_nchar_height;
	update_rect.bottom = update_rect.top + m_nchar_height;

	RedrawWindow(m_hwnd, &update_rect, 0, RDW_INVALIDATE|RDW_ERASE);
}

void c_terminal::on_enter()
{
	m_bcaret_visible = false;
	update_caret();
//	SetEvent(m_pengine->get_io_event());
	m_pengine->on_terminal_enter();
}

void c_terminal::on_key_down(WPARAM wParam)
{
	if (!m_bcaret_visible) return;

	bool b_redraw		= false;
	bool b_update_caret	= false;

	switch (wParam) 
	{ 
		case VK_LEFT:
			if (m_ncaret_pos > 0)
			{
				m_ncaret_pos --;
				b_update_caret = true;
			}
			break; 
 
		case VK_RIGHT:
			if (m_ncaret_pos < m_input_string.get_length())
			{
				m_ncaret_pos ++;
				b_update_caret = true;
			}
			break;

		case VK_DELETE:
			m_input_string.kill(m_ncaret_pos);
			b_update_caret = true;
			b_redraw = true;
			break;

		case VK_HOME:
			m_ncaret_pos = 0;
			b_update_caret = true;
			break;

		case VK_END:
			m_ncaret_pos = m_input_string.get_length();
			b_update_caret = true;
			break;
	}

	ensure_visible_horz();
	update_max_length();

	if (b_update_caret)
		update_caret();

	if (b_redraw)
		update_last_line();
}

void c_terminal::on_char(WPARAM wParam)
{
	if (!m_bcaret_visible) return;

	bool b_redraw = false;
	bool b_update_caret	= false;

	switch (wParam) 
	{ 
	case VK_RETURN:
		on_enter();
		break;

	case 0x08:
		if (m_ncaret_pos > 0)
		{
			m_input_string.kill(m_ncaret_pos - 1);
			m_ncaret_pos --;
			b_redraw = true;
			b_update_caret = true;
		}
		break;

	default:
		m_input_string.insert((char)wParam, m_ncaret_pos);
		m_ncaret_pos ++;
		b_redraw = true;
		b_update_caret = true;
	}

	ensure_visible_horz();
	update_max_length();

	if (b_update_caret)
		update_caret();

	if (b_redraw)
		update_last_line();
}

void c_terminal::on_paint()
{
	PAINTSTRUCT ps; 
	HDC hdc = BeginPaint(m_hwnd, &ps);

	SelectObject(hdc, m_pengine->get_terminal_font());

	if (m_nchar_width == 0) calc_char_size();
	int i = 0;
	for ( i = 0; i < m_strings.get_size() - 1; i++)
		TextOut(hdc,
				- m_scroll_pos.x * m_nchar_width,
				(i - m_scroll_pos.y) * m_nchar_height,
				m_strings[i], (int)strlen(m_strings[i]));

	if (i < m_strings.get_size())
	{
		c_string last_string = m_strings[i];
		last_string += m_input_string;

		TextOut(hdc,
				- m_scroll_pos.x * m_nchar_width,
				(i - m_scroll_pos.y) * m_nchar_height,
				(LPCTSTR)last_string, last_string.get_length());
	}

	EndPaint(m_hwnd, &ps);
}

void c_terminal::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	int nscroll = 0;
	switch (LOWORD(wParam))
	{
		case SB_PAGEUP:
			nscroll = -1;
			break;

		case SB_PAGEDOWN:
			nscroll = 1;
			break;

		case SB_LINEUP: 
			nscroll = -1;
			break;

		case SB_LINEDOWN: 
			nscroll = 1;
			break;

		case SB_THUMBTRACK:
			nscroll = HIWORD(wParam) - m_scroll_pos.x;
	}

	if (nscroll <= 0 && m_scroll_pos.x == 0) return;
	if (nscroll >= 0 && m_scroll_pos.x >= m_nlongest_string - 1) return;

	m_scroll_pos.x += nscroll;
	ScrollWindow(m_hwnd, - nscroll * m_nchar_width, 0, 0, 0);
	update_scroll_bars();
}

void c_terminal::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	int nscroll = 0;
	switch (LOWORD(wParam))
	{
		case SB_PAGEUP:
			nscroll = -1;
			break;

		case SB_PAGEDOWN:
			nscroll = 1;
			break;

		case SB_LINEUP: 
			nscroll = -1;
			break;

		case SB_LINEDOWN: 
			nscroll = 1;
			break;

		case SB_THUMBTRACK:
			nscroll = HIWORD(wParam) - m_scroll_pos.y;
	}

	if (nscroll <= 0 && m_scroll_pos.y == 0) return;
	if (nscroll >= 0 && m_scroll_pos.y >= m_strings.get_size() - 1) return;

	m_scroll_pos.y += nscroll;
	ScrollWindow(m_hwnd, 0, - nscroll * m_nchar_height, 0, 0);
	update_scroll_bars();
}

void c_terminal::get_text(c_string* p_str)
{
	p_str->empty();

	int i;
	for (i = 0; i < m_strings.get_size(); i++)
	{
		if (i) (*p_str) += "\r\n";
		(*p_str) += m_strings[i];
	}
}

LRESULT c_terminal::wndProc(
		HWND hwnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam)
{
#ifdef _WIN64
	c_terminal* _this = (c_terminal*)(LONGLONG)GetWindowLong(hwnd, GWLP_USERDATA);
#else
	c_terminal* _this = (c_terminal*)GetWindowLong(hwnd, GWL_USERDATA);
#endif // _WIN64
	
	if (!_this) return DefWindowProc(hwnd, uMsg, wParam, lParam);

	switch(uMsg)
	{
 	case WM_KEYDOWN: _this->on_key_down(wParam);
		break;

	case WM_CHAR: _this->on_char(wParam);
		break;

	case WM_PAINT: _this->on_paint();
		break;

	case WM_SETFOCUS: _this->update_caret();
		break;

	case WM_KILLFOCUS: HideCaret(_this->m_hwnd);
		break;

	case WM_SIZE: _this->update_scroll_bars();
		break;

	case WM_HSCROLL: _this->OnHScroll(wParam, lParam);
		break;

	case WM_VSCROLL: _this->OnVScroll(wParam, lParam);
		break;

	case WM_CLOSE:
		ShowWindow(hwnd, SW_HIDE);

		_this->m_bcontinue = false;
//		SetEvent(_this->m_pengine->get_io_event());
		_this->m_pengine->on_window_enter();
		break;

	case WM_CONTEXTMENU:
		{
			int x = GET_X_LPARAM(lParam); 
			int y = GET_Y_LPARAM(lParam); 

			HMENU h_menu = CreatePopupMenu();
			//AppendMenu(h_menu, MF_STRING, ID_TERMINAL_SAVE, "Save to file");
			//AppendMenu(h_menu, MF_STRING, ID_TERMINAL_COPY, "Copy all terminal text");
			//AppendMenu(h_menu, MF_STRING, ID_TERMINAL_CLEAR, "Clear all");
			TrackPopupMenu(h_menu, TPM_LEFTALIGN, x, y, 0, hwnd, 0);
		}
		break;

	case WM_GETDLGCODE: return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_WANTTAB;

	case WM_COMMAND:
		{
			//switch (LOWORD(wParam))
			//{
			//	case ID_TERMINAL_COPY:
			//	{
			//		c_string str;
			//		_this->get_text(&str);
			//		if (!str.get_length()) break;

			//		// do copy

			//		if (OpenClipboard(hwnd))
			//		{
			//			HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, str.get_length() + 1);
			//			if (!clipbuffer)
			//			{
			//				_ASSERT(0);
			//				return 0;
			//			}
			//			
			//			char *buffer = (char*)GlobalLock(clipbuffer);
			//			strcpy(buffer, str.get_buffer());
			//			
			//			GlobalUnlock(clipbuffer);
			//			
			//			EmptyClipboard();
			//			SetClipboardData(CF_TEXT, clipbuffer);
			//			CloseClipboard();
			//		}
			//	}
			//	break;

			//case ID_TERMINAL_CLEAR:
			//	_this->clear();
			//	break;

			//case ID_TERMINAL_SAVE:
			//	{
			//		// get text

			//		c_string str;
			//		_this->get_text(&str);
			//		if (!str.get_length())
			//		{
			//			MessageBeep(0);
			//			break;
			//		}

			//		// prompt for a file name

			//		OPENFILENAME ofn;       // common dialog box structure
			//		char szFile[MAX_PATH];
			//		szFile[0] = 0;

			//		// Initialize OPENFILENAME
			//		memset (&ofn, 0, sizeof (ofn));
			//		ofn.lStructSize = sizeof(OPENFILENAME);
			//		ofn.hwndOwner = hwnd;
			//		ofn.lpstrFilter = "All files\0*.*\0Text files\0*.TXT\0";
			//		ofn.nFilterIndex = 1;
			//		ofn.lpstrFile = szFile;
			//		ofn.nMaxFile = sizeof(szFile);
			//		//ofn.lpstrFileTitle = szFile;
			//		//ofn.nMaxFileTitle = MAX_PATH;
			//		ofn.lpstrDefExt = "txt";
			//		ofn.lpstrInitialDir = NULL;
			//		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST;

			//		// Display the Open dialog box. 

			//		BOOL b_retval = GetSaveFileName(&ofn);
			//		if (b_retval == TRUE)
			//		{
			//			FILE* f = fopen(ofn.lpstrFile, "wt");
			//			if (!f)
			//			{
			//				MessageBox(hwnd, "Can't open file to write to.", "Error", MB_OK | MB_ICONERROR);
			//				break;
			//			}
			//			fputs(str.get_buffer(), f);
			//			fclose(f);
			//		}

			//	}
			//	break;
			//}
		}
		break;

	default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

#ifdef _DEBUG
void c_terminal::assert_valid()
{
	_ASSERT(!IsBadWritePtr(this, sizeof(c_terminal)));
	_ASSERT(IsWindow(m_hwnd));
}
#endif

