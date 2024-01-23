// This file is part of JSLint Plugin for Notepad++
// Copyright (C) 2010 Martin Vladic <martin.vladic@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#pragma once

////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>

std::wstring TrimSpaces(std::wstring const &str);

void StringSplit(
    std::wstring str, std::wstring delim, std::vector<std::wstring> &results
);

std::wstring StringJoin(std::vector<std::wstring> &results, std::wstring delim);

void FindReplace(
    std::wstring &str, std::wstring const &strOld, std::wstring const &strNew
);

BOOL CenterWindow(HWND hWnd, HWND hParentWnd, BOOL bRepaint = FALSE);

std::wstring GetWindowText(HWND hWnd);

void DoEvents();

////////////////////////////////////////////////////////////////////////////////

class Win32Handle
{
    Win32Handle(Win32Handle const &rhs);
    Win32Handle &operator=(Win32Handle const &rhs);

  public:
    Win32Handle() : m_handle(NULL)
    {
    }
    Win32Handle(HANDLE handle) : m_handle(handle)
    {
    }

    ~Win32Handle()
    {
        if (m_handle != NULL)
        {
            CloseHandle(m_handle);
        }
    }

    Win32Handle &operator=(HANDLE handle)
    {
        if (m_handle != NULL)
        {
            CloseHandle(m_handle);
        }
        m_handle = handle;
        return *this;
    }

    operator HANDLE()
    {
        return m_handle;
    }
    HANDLE *operator&()
    {
        return &m_handle;
    }

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
    //static std::wstring GetPathRoot(std::wstring const &strPath);
    static std::wstring GetDirectoryName(std::wstring const &strPath);
    static std::wstring GetFileName(std::wstring const &strPath);
    static std::wstring GetFileNameWithoutExtension(std::wstring const &strPath
    );
    static std::wstring GetPathNameWithoutExtension(std::wstring const &strPath
    );
    static std::wstring GetExtension(std::wstring const &strPath);

    static std::wstring GetFullPath(std::wstring const &strPath);
    static std::wstring GetFullPath(
        std::wstring const &strPath, std::wstring const &strBaseDir
    );

    static bool IsRelative(std::wstring const &strPath);
    static bool IsDir(std::wstring const &strPath);
    static bool IsFileExists(std::wstring const &strPath);

    static std::wstring GetTempFileName();
    static std::wstring GetModuleFileName(HMODULE hModule);
};

////////////////////////////////////////////////////////////////////////////////

class TempFile
{
  public:
    ~TempFile()
    {
        // delete temp file
        if (! m_strFileName.empty())
        {
            DeleteFile(m_strFileName.c_str());
        }
    }

    // create temp file
    void Create()
    {
        m_strFileName = Path::GetTempFileName();
        if (m_strFileName.empty())
        {
            throw IOException();
        }
    }

    // test if temp file is created
    operator bool() const
    {
        return ! m_strFileName.empty();
    }

    // return temp file name as std std::string
    std::wstring const &GetFileName() const
    {
        return m_strFileName;
    }

    // return temp file name as Win32 std::string
    operator LPCTSTR() const
    {
        return m_strFileName.c_str();
    }

  private:
    std::wstring m_strFileName;
};

////////////////////////////////////////////////////////////////////////////////

class TextConversion
{
  public:
    static std::string UTF8_To_A(std::string const &str);

    static std::wstring UTF8_To_W(std::string const &str);

    static std::wstring UTF8_To_T(std::string const &str)
    {
#if defined(UNICODE) || defined(_UNICODE)
        return UTF8_To_W(str);
#else
        return UTF8_To_A(str);
#endif
    }

    static std::string A_To_UTF8(std::string const &str);

    static std::string W_To_UTF8(std::wstring const &wstr);

    static std::string T_To_UTF8(std::wstring const &tstr)
    {
#if defined(UNICODE) || defined(_UNICODE)
        return W_To_UTF8(tstr);
#else
        return A_To_UTF8(tstr);
#endif
    }

    static std::wstring A_To_T(std::string const &str);
};
