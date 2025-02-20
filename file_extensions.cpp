#include "pch.h"

#include "file_extensions.h"
#include "list.h"
#include <shlobj.h>
#include <sys/types.h>
#include <sys/stat.h>

void __stdcall file_dlg(int nargs, c_variable** pargs, c_variable& result, bool b_open)
{
	char buff[MAX_PATH];
	OPENFILENAME ofd;
	char filter[1024];	// hope enough

	buff[0] = 0;
	memset(&ofd, 0, sizeof(ofd));
	ofd.lStructSize = sizeof(ofd);
	ofd.lpstrFile = buff;
	ofd.nMaxFile = sizeof(buff);
	ofd.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST;
			c_string str;

	switch (nargs)
	{
	case 3:	// initial directory
		pargs[2]->as_string(str);
		if (strlen(str))
			ofd.lpstrInitialDir = str;

	case 2:	// filename
		pargs[1]->as_string(str);
		if (strlen(str)) strcpy(buff, str);

	case 1:	// filter
		pargs[0]->as_string(str);
		if (strlen(str))
		{
			const char* p_in = str;
			char* p_out = filter;
			int i = 0;

			while (*p_in && i < sizeof(filter) - 2)
			{
				if (*p_in == '|') *p_out = 0;
				else *p_out = *p_in;

				p_in ++; p_out ++; i++;
			}
			*p_out++ = 0;
			*p_out++ = 0;

			ofd.lpstrFilter = filter;
		}

	case 0: break;

	default: _ASSERT(0);
	}

	if (b_open) GetOpenFileName(&ofd);
	else GetSaveFileName(&ofd);

	result = buff;
}

void dir_size(const char* p_name, bool b_subdirs, int& n_size)
{
	WIN32_FIND_DATA fd;

	if (!strlen(p_name)) return;
	c_string str_name;
	str_name = p_name;

	char c_last = str_name.get_last();
	if (c_last != '/' && c_last != '\\') str_name += '\\';
	str_name += "*.*";

	HANDLE h = FindFirstFile(str_name, &fd);

	if (h) n_size += fd.nFileSizeLow;

	while(FindNextFile(h, &fd))
	{
		if (strcmp(fd.cFileName, ".") == 0 ||
			strcmp(fd.cFileName, "..") == 0) continue;

		if ( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && b_subdirs)
		{
			str_name = p_name;
			if (c_last != '/' && c_last != '\\') str_name += '\\';
			str_name += fd.cFileName;

			dir_size(str_name, b_subdirs, n_size);
		}
		else
			n_size += fd.nFileSizeLow;
	}

	FindClose(h);
}

//-----------------------------------------------------------------------------
// copy_file ------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall copy_file(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 2);
	if (nargs != 2)
	{
		result = FALSE;
		return;
	}
	c_string str0, str1;
	pargs[0]->as_string(str0);
	pargs[1]->as_string(str1);
	result = (int)CopyFile(str0, str1, TRUE);
}

bool _check_copy_file(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'CopyFile' function gets 2 arguments: source and destination.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// move_file ------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall move_file(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 2);
	if (nargs != 2)
	{
		result = FALSE;
		return;
	}
	c_string str0, str1;

	pargs[0]->as_string(str0);
	pargs[1]->as_string(str1);

	result = (int)MoveFile(str0, str1);
}

bool _check_move_file(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2)
	{
		*p_msg = "'MoveFile' function gets 2 arguments: source and destination.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// file_exists ----------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall file_exists(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{	
	if (nargs != 1)
	{
		result = 0;
		return;
	}
	struct _stat s;

	c_string str;
	pargs[0]->as_string(str);

	if (_stat(str, &s) == -1)
		result = (int)0;
	else
		result = (int)1;
}

bool _check_file_exists(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'FileExists' function gets 1 argument.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// delete_file ----------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall delete_file(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = 0; 
		return;
	}
	struct _stat s;

	c_string str;
	pargs[0]->as_string(str);

	if (_stat(str, &s) == -1)
	{
		result = (int)0;
		return;
	}

	if (s.st_mode & _S_IFDIR)
		result = (int)RemoveDirectory(str);
	else
		result = (int)DeleteFile(str);
}

bool _check_delete_file(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'DeleteFile' function gets 1 argument.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// open_file_dlg --------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall open_file_dlg(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	file_dlg(nargs, pargs, result, true);
}

bool _check_open_file_dlg(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n > 3)
	{
		*p_msg = "'OpenFileDlg' function gets up to 3 arguments.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// save_file_dlg --------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall save_file_dlg(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	file_dlg(nargs, pargs, result, false);
}

bool _check_save_file_dlg(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n > 3)
	{
		*p_msg = "'SaveFileDlg' function gets up to 3 arguments.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// mk_dir ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall mk_dir(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = 0;
		return;
	}
	c_string str;
	pargs[0]->as_string(str);

	result = (int)CreateDirectory(str, 0);
}

bool _check_mk_dir(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'MkDir' function gets 1 argument.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// file_size ------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall file_size(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	if (pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	c_string str;
	pargs[0]->as_string(str);

	struct _stat s;
	if (_stat(str, &s) != 0)
	{
		result = INT_MIN;
		return;
	}

	if (s.st_mode & _S_IFDIR)
	{
		result = INT_MIN;
		return;
	}

	result = s.st_size;
}

bool _check_file_size(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'FileSize' function gets 1 argument.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// dir_size -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall dir_size(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	if (pargs[0]->vt != VT_BSTR)
	{
		result = INT_MIN;
		return;
	}
	int n_size = 0;

	c_string str;
	pargs[0]->as_string(str);

	dir_size(str,
		(nargs == 1) || ( pargs[1]->as_integer() != 0),
		n_size);

	result = (int)n_size;
}

bool _check_dir_size(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1 && n != 2)
	{
		*p_msg = "'MkDir' function gets 1 arguments.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// split_path -----------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall split_path(int nargs, c_variable** pargs, c_engine* p_engine)
{
	_ASSERT(nargs == 5);

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	c_string str;
	pargs[0]->as_string(str);

	_splitpath(str, drive, dir, fname, ext);

	*pargs[1] = drive;
	*pargs[2] = dir;
	*pargs[3] = fname;
	*pargs[4] = ext;
}

bool _check_split_path(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 5)
	{
		*p_msg = "'SplitPath' function gets 5 arguments.";
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// list_dir -------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall list_dir(int nargs, c_variable** pargs, c_engine* p_engine)
{
	_ASSERT(nargs == 2 || nargs == 3);
	_ASSERT(pargs[0]->vt == VT_VARARRAY);

	c_str_array fnames;

// delete old contents of array

	c_variable* p_var = pargs[0];
	if (p_var->parray)
	{
		SafeArrayDestroy(p_var->parray);
		p_var->parray = 0;
	}

// prepare name and pattern

	c_string str_name;
	pargs[1]->as_string(str_name);

	c_string pattern;
	if (nargs == 3) pargs[2]->as_string(pattern);
	else pattern = "*.*";

	char c_last = str_name.get_last();
	if (c_last != '/' && c_last != '\\') str_name += '\\';
	str_name += pattern;

// find them all

	WIN32_FIND_DATA fd;

	HANDLE h = FindFirstFile(str_name, &fd);

	if (h != INVALID_HANDLE_VALUE)fnames.add(fd.cFileName);

	while(FindNextFile(h, &fd))
		fnames.add(fd.cFileName);

	FindClose(h);

// alloc new SafeArray

	SAFEARRAY* p_array = SafeArrayCreateVector(VT_VARIANT, 0, fnames.get_size());
	c_variable* p_data = 0;
	HRESULT hr = SafeArrayAccessData(p_array, (void**)&p_data);
	if (FAILED(hr) || !p_data)
	{
		_ASSERT(0); //hmm...
		return;
	}

	int i;
	for (i = 0; i < fnames.get_size(); i++)
		p_data[i] = fnames[i];

	SafeArrayUnaccessData(p_array);

	p_var->parray = p_array;
}

bool _check_list_dir(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 2 && n != 3)
	{
		*p_msg = "'ListDir' function gets 2 or 3 arguments.";
		return false;
	}

	if (p_types[0] != VT_VARARRAY)
	{
		*p_msg = "first argument of 'ListDir' function must be of Array type.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// select_dir_dlg -------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall select_dir_dlg(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	if (nargs != 0)
	{
		result = "";
		return;
	}

	char dir[MAX_PATH];
	BROWSEINFO bi;

	dir[0] = 0;

    bi.hwndOwner		= 0;
    bi.pidlRoot			= 0;
    bi.pszDisplayName	= dir;
    bi.lpszTitle		= "Select directory";
    bi.ulFlags			= BIF_EDITBOX;
    bi.lpfn				= 0;
    bi.lParam			= 0;
    bi.iImage			= 0;

	LPITEMIDLIST p = SHBrowseForFolder(&bi);
	if (p) SHGetPathFromIDList(p, dir);
	result = dir;
}

bool _check_select_dir_dlg(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 0)
	{
		*p_msg = "'SelectDirDlg' function does not get arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// get_cur_dir ----------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall get_cur_dir(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 0);
	if (nargs != 0)
	{
		result = "";
		return;
	}
	char buffer[MAX_PATH];
	buffer[0] = 0;

	GetCurrentDirectory(sizeof(buffer), buffer);

	result = buffer;
}

bool _check_get_cur_dir(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 0)
	{
		*p_msg = "'GetCurrentDirectory' function does not get arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// set_cur_dir ----------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall set_cur_dir(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = FALSE;
		return;
	}
	c_string s;
	pargs[0]->as_string(s);

	BOOL b = SetCurrentDirectory(s.get_buffer());

	result = b;
}

bool _check_set_cur_dir(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'SetCurrentDirectory' function takes one argument.";
		return false;
	}

	return true;
}
