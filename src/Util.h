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

void DoEvents() noexcept;

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

class Path
{
  public:
    static std::wstring GetFileName(std::wstring const &strPath);

    static std::wstring GetFullPath(
        std::wstring const &strPath, std::wstring const &strBaseDir
    );

    static bool IsRelative(std::wstring const &strPath) noexcept;
    static bool FileExists(std::wstring const &strPath) noexcept;
};

////////////////////////////////////////////////////////////////////////////////

class TextConversion
{
  public:
    static std::wstring UTF8_To_W(std::string const &str);

    static std::wstring UTF8_To_T(std::string const &str)
    {
        return UTF8_To_W(str);
    }

    static std::string A_To_UTF8(std::string const &str);

    static std::string W_To_UTF8(std::wstring const &wstr);

    static std::string T_To_UTF8(std::wstring const &tstr)
    {
        return W_To_UTF8(tstr);
    }
};
