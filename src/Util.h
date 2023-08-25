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

#pragma once

////////////////////////////////////////////////////////////////////////////////

std::wstring TrimSpaces(const std::wstring& str);
void StringSplit(std::wstring str, std::wstring delim, std::vector<std::wstring>& results);
std::wstring StringJoin(std::vector<std::wstring>& results, std::wstring delim);
void FindReplace(std::wstring& str, const std::wstring& strOld, const std::wstring& strNew);

BOOL CenterWindow(HWND hWnd, HWND hParentWnd, BOOL bRepaint = FALSE);
std::wstring GetWindowText(HWND hWnd);
void DoEvents();

////////////////////////////////////////////////////////////////////////////////

class Win32Handle
{
	Win32Handle(const Win32Handle& rhs);
	Win32Handle& operator=(const Win32Handle& rhs);

public:
	Win32Handle() : m_handle(NULL) {}
	Win32Handle(HANDLE handle) : m_handle(handle) {}

	~Win32Handle() { 
		if (m_handle != NULL) {
			CloseHandle(m_handle); 
		}
	}

	Win32Handle& operator=(HANDLE handle) { 
		if (m_handle != NULL) {
			CloseHandle(m_handle);
		}
		m_handle = handle; 
		return *this; 
	}

	operator HANDLE() { return m_handle; }
	HANDLE* operator &() { return &m_handle; }

private:
	HANDLE m_handle;
};

////////////////////////////////////////////////////////////////////////////////

class IOException : public std::exception
{
};

////////////////////////////////////////////////////////////////////////////////

class Path
{
public:
	static std::wstring GetPathRoot(const std::wstring& strPath);
	static std::wstring GetDirectoryName(const std::wstring& strPath);
	static std::wstring GetFileName(const std::wstring& strPath);
	static std::wstring GetFileNameWithoutExtension(const std::wstring& strPath);
	static std::wstring GetPathNameWithoutExtension(const std::wstring& strPath);
	static std::wstring GetExtension(const std::wstring& strPath);

	static std::wstring GetFullPath(const std::wstring& strPath);
	static std::wstring GetFullPath(const std::wstring& strPath, const std::wstring& strBaseDir);

	static bool IsRelative(const std::wstring& strPath);
	static bool IsDir(const std::wstring& strPath);
	static bool IsFileExists(const std::wstring& strPath);

	static std::wstring GetTempFileName();
	static std::wstring GetModuleFileName(HMODULE hModule);
};

////////////////////////////////////////////////////////////////////////////////

class TempFile
{
public:
	~TempFile() {
		// delete temp file
		if (!m_strFileName.empty())
			DeleteFile(m_strFileName.c_str());
	}

	// create temp file
	void Create() {
		m_strFileName = Path::GetTempFileName();
		if (m_strFileName.empty()) {
			throw IOException();
		}
	}

	// test if temp file is created
	operator bool() const { return !m_strFileName.empty(); }

	// return temp file name as std std::string
	const std::wstring& GetFileName() const { return m_strFileName; }

	// return temp file name as Win32 std::string
	operator LPCTSTR() const { return m_strFileName.c_str(); }

private:
	std::wstring m_strFileName;
};

////////////////////////////////////////////////////////////////////////////////

class TextConversion
{
public:
	static std::string UTF8_To_A(const std::string& str) {
		int wsize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* wbuffer = new wchar_t[wsize];
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wbuffer, wsize);
		int size = WideCharToMultiByte(CP_ACP, 0, wbuffer, wsize, NULL, 0, NULL, NULL);
		char* buffer = new char[size];
		WideCharToMultiByte(CP_ACP, 0, wbuffer, wsize, buffer, size, NULL, NULL);
		delete [] wbuffer;
		std::string result(buffer);
		delete [] buffer;
		return result;
	}

	static std::wstring UTF8_To_W(const std::string& str) {
		int wsize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* wbuffer = new wchar_t[wsize];
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wbuffer, wsize);
		std::wstring result(wbuffer);
		delete [] wbuffer;
		return result;
	}

	static std::wstring UTF8_To_T(const std::string& str) {
	#if defined(UNICODE) || defined(_UNICODE)
		return UTF8_To_W(str);
	#else
		return UTF8_To_A(str);
	#endif
	}

	static std::string A_To_UTF8(const std::string& str) {
		int wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		wchar_t* wbuffer = new wchar_t[wsize];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wbuffer, wsize);
		int size = WideCharToMultiByte(CP_UTF8, 0, wbuffer, wsize, NULL, 0, NULL, NULL);
		char* buffer = new char[size];
		WideCharToMultiByte(CP_UTF8, 0, wbuffer, wsize, buffer, size, NULL, NULL);
		delete [] wbuffer;
		std::string result(buffer);
		delete [] buffer;
		return result;
	}

	static std::string W_To_UTF8(const std::wstring& wstr) {
		int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		char* buffer = new char[size];
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, buffer, size, NULL, NULL);
		std::string result(buffer);
		delete [] buffer;
		return result;
	}

	static std::string T_To_UTF8(const std::wstring& tstr) {
	#if defined(UNICODE) || defined(_UNICODE)
		return W_To_UTF8(tstr);
	#else
		return A_To_UTF8(tstr);
	#endif
	}

	static std::wstring A_To_T(const std::string& str) {
	#if defined(UNICODE) || defined(_UNICODE)
		int wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		wchar_t* wbuffer = new wchar_t[wsize];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wbuffer, wsize);
		std::wstring result(wbuffer);
		delete [] wbuffer;
		return result;
	#else
		return str;
	#endif
	}
};
