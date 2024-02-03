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

#include "StdHeaders.h"

#include "Util.h"

#include <Shlwapi.h>

////////////////////////////////////////////////////////////////////////////////

std::wstring TrimSpaces(std::wstring const &str)
{
    size_t const first = str.find_first_not_of(L" \t\r\n");
    size_t const last = str.find_last_not_of(L" \t\r\n");

    if (first == std::string::npos || last == std::string::npos)
    {
        return std::wstring();
    }

    return str.substr(first, last - first + 1);
}

void StringSplit(
    std::wstring str, std::wstring delim, std::vector<std::wstring> &results
)
{
    std::size_t cutAt;
    while (cutAt = str.find_first_of(delim), cutAt != str.npos)
    {
        if (cutAt != 0)
        {
            results.push_back(str.substr(0, cutAt));
        }
        str = str.substr(cutAt + 1);
    }
    if (str.length() > 0)
    {
        results.push_back(str);
    }
}

std::wstring StringJoin(std::vector<std::wstring> &results, std::wstring delim)
{
    std::wstring result;

    for (std::vector<std::wstring>::const_iterator it = results.begin();
         it != results.end();
         ++it)
    {
        if (! result.empty())
        {
            result += delim;
        }
        result += *it;
    }

    return result;
}

void FindReplace(
    std::wstring &str, std::wstring const &strOld, std::wstring const &strNew
)
{
    size_t pos = 0;
    while ((pos = str.find(strOld, pos)) != std::wstring::npos)
    {
        str.replace(pos, strOld.length(), strNew);
        pos += strNew.length();
    }
}

void DoEvents() noexcept
{
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
    {
        if (::GetMessage(&msg, nullptr, 0, 0))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

std::wstring Path::GetFileName(std::wstring const &strPath)
{
    return ::PathFindFileName(strPath.c_str());
}

std::wstring Path::GetFullPath(
    std::wstring const &strPath, std::wstring const &strBaseDir
)
{
    if (! Path::IsRelative(strPath))
    {
        return strPath;
    }
    TCHAR strFullPath[MAX_PATH];
    return ::PathCombine(&strFullPath[0], strBaseDir.c_str(), strPath.c_str());
}

bool Path::IsRelative(std::wstring const &strPath) noexcept
{
    return ::PathIsRelative(strPath.c_str()) ? true : false;
}

bool Path::FileExists(std::wstring const &strPath) noexcept
{
    return ::PathFileExists(strPath.c_str()) ? true : false;
}

std::wstring TextConversion::UTF8_To_W(std::string const &str)
{
    int const wsize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    wchar_t *wbuffer = new wchar_t[wsize];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wbuffer, wsize);
    std::wstring result(wbuffer);
    delete[] wbuffer;
    return result;
}

std::string TextConversion::A_To_UTF8(std::string const &str)
{
    int const wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    wchar_t *wbuffer = new wchar_t[wsize];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wbuffer, wsize);
    int const size =
        WideCharToMultiByte(CP_UTF8, 0, wbuffer, wsize, nullptr, 0, nullptr, nullptr);
    char *buffer = new char[size];
    WideCharToMultiByte(CP_UTF8, 0, wbuffer, wsize, buffer, size, nullptr, nullptr);
    delete[] wbuffer;
    std::string result(buffer);
    delete[] buffer;
    return result;
}

std::string TextConversion::W_To_UTF8(std::wstring const &wstr)
{
    int const size =
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    char *buffer = new char[size];
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, buffer, size, nullptr, nullptr);
    std::string result(buffer);
    delete[] buffer;
    return result;
}

std::wstring TextConversion::A_To_T(std::string const &str)
{
    int const wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    wchar_t *wbuffer = new wchar_t[wsize];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wbuffer, wsize);
    std::wstring result(wbuffer);
    delete[] wbuffer;
    return result;
}
