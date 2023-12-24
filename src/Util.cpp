//This file is part of JSLint Plugin for Notepad++
//Copyright (C) 2010 Martin Vladic <martin.vladic@gmail.com>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "StdHeaders.h"
#include "Util.h"

#include <Shlwapi.h>
#include <tchar.h>

////////////////////////////////////////////////////////////////////////////////

std::wstring TrimSpaces(const std::wstring& str)
{
    size_t first = str.find_first_not_of(TEXT(" \t\r\n"));
    size_t last = str.find_last_not_of(TEXT(" \t\r\n"));
 
    if (first == std::string::npos || last == std::string::npos)
		return std::wstring();

	return str.substr(first, last - first + 1 );
}

void StringSplit(std::wstring str, std::wstring delim, std::vector<std::wstring>& results)
{
	int cutAt;
	while ((cutAt = str.find_first_of(delim)) != str.npos) {
		if (cutAt > 0)
			results.push_back(str.substr(0,cutAt));
		str = str.substr(cutAt+1);
	}
	if (str.length() > 0)
		results.push_back(str);
}

std::wstring StringJoin(std::vector<std::wstring>& results, std::wstring delim)
{
	std::wstring result;

	for (std::vector<std::wstring>::const_iterator it = results.begin(); it != results.end(); ++it) {
		if (!result.empty())
			result += delim;
		result += *it;
	}

	return result;
}

void FindReplace(std::wstring& str, const std::wstring& strOld, const std::wstring& strNew)
{
  size_t pos = 0;
  while ((pos = str.find(strOld, pos)) != std::wstring::npos)
  {
     str.replace(pos, strOld.length(), strNew);
     pos += strNew.length();
  }
}

BOOL CenterWindow(HWND hWnd, HWND hParentWnd, BOOL bRepaint)
{
    RECT rectParent;
    RECT rect;
    INT  height, width;
    INT  x, y;

    ::GetWindowRect(hParentWnd, &rectParent);
    ::GetWindowRect(hWnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    x = ((rectParent.right - rectParent.left) - width) / 2;
    x += rectParent.left;
    y = ((rectParent.bottom - rectParent.top) - height) / 2;
    y += rectParent.top;
    return ::MoveWindow(hWnd, x, y, width, height, bRepaint);
}

std::wstring GetWindowText(HWND hWnd)
{
	int nLength = GetWindowTextLength(hWnd);
	TCHAR *szBuffer = new TCHAR[nLength + 1];
	GetWindowText(hWnd, szBuffer, nLength + 1);
	std::wstring result = szBuffer;
	delete [] szBuffer;
    return result;
}

void DoEvents()
{
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
        if (::GetMessage(&msg, NULL, 0, 0)) {
            ::TranslateMessage(&msg);
            :: DispatchMessage(&msg);
		} else {
            break;
		}
    }
}

////////////////////////////////////////////////////////////////////////////////

std::wstring Path::GetDirectoryName(const std::wstring& strPath)
{
	TCHAR szPathCopy[MAX_PATH];
	_tcsncpy_s(szPathCopy, _countof(szPathCopy), strPath.c_str(), _TRUNCATE);

	::PathRemoveFileSpec(szPathCopy);

	return szPathCopy;
}

std::wstring Path::GetFileName(const std::wstring& strPath)
{
	return ::PathFindFileName(strPath.c_str());
}

std::wstring Path::GetFileNameWithoutExtension(const std::wstring& strPath)
{
	TCHAR szPathCopy[MAX_PATH];
	_tcsncpy_s(szPathCopy, _countof(szPathCopy), GetFileName(strPath).c_str(), _TRUNCATE);

	::PathRemoveExtension(szPathCopy);

	return szPathCopy;
}

std::wstring Path::GetPathNameWithoutExtension(const std::wstring& strPath)
{
	TCHAR szPathCopy[MAX_PATH];
	_tcsncpy_s(szPathCopy, _countof(szPathCopy), strPath.c_str(), _TRUNCATE);

	::PathRemoveExtension(szPathCopy);

	return szPathCopy;
}

std::wstring Path::GetExtension(const std::wstring& strPath)
{
	return ::PathFindExtension(strPath.c_str());
}

std::wstring Path::GetFullPath(const std::wstring& strPath)
{
	TCHAR strCurrentDirectory[MAX_PATH];
	if (GetCurrentDirectory(MAX_PATH, strCurrentDirectory) == 0)
		return TEXT("");
	return GetFullPath(strPath, strCurrentDirectory);
}

std::wstring Path::GetFullPath(const std::wstring& strPath, const std::wstring& strBaseDir)
{
	if (!Path::IsRelative(strPath))
		return strPath;
	TCHAR strFullPath[MAX_PATH];
	return ::PathCombine(strFullPath, strBaseDir.c_str(), strPath.c_str());
}

bool Path::IsRelative(const std::wstring& strPath)
{
	return ::PathIsRelative(strPath.c_str()) ? true : false;
}

bool Path::IsDir(const std::wstring& strPath)
{
	return ::PathIsDirectory(strPath.c_str()) ? true : false;
}

bool Path::IsFileExists(const std::wstring& strPath)
{
	return ::PathFileExists(strPath.c_str()) ? true : false;
}

std::wstring Path::GetTempFileName()
{
    TCHAR szTempPath[MAX_PATH];	
	if (::GetTempPath(MAX_PATH, szTempPath) == 0) {
		return TEXT("");
	}

	TCHAR szTempFileName[MAX_PATH];  
	if (::GetTempFileName(szTempPath, nullptr, 0, szTempFileName) == 0) {
		return TEXT("");
	}

	return szTempFileName;
}

std::wstring Path::GetModuleFileName(HMODULE hModule)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	::GetModuleFileName(hModule, szPath, MAX_PATH);
	return szPath;
}
