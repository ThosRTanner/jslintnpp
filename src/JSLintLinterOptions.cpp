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

#include "JSLintLinterOptions.h"

#include "resource.h"

#include <tchar.h>

#include <cstdio>
#include <optional>
#include <string>

#define PROFILE_JSLINT_OPTIONS_GROUP_NAME L"JSLint Options"

JSLintLinterOptions::JSLintLinterOptions(Profile_Handler *profile_handler) :
    Linter_Options(PROFILE_JSLINT_OPTIONS_GROUP_NAME, profile_handler)
{
    m_options[IDC_CHECK_PASSFAIL] = Option(L"passfail");
    m_options[IDC_CHECK_WHITE] = Option(L"white");
    m_options[IDC_CHECK_BROWSER] = Option(L"browser");
    m_options[IDC_CHECK_DEVEL] = Option(L"devel");
    m_options[IDC_CHECK_WINDOWS] = Option(L"windows");
    m_options[IDC_CHECK_RHINO] = Option(L"rhino");
    m_options[IDC_CHECK_SAFE] = Option(L"safe");
    m_options[IDC_CHECK_ADSAFE] = Option(L"adsafe");
    m_options[IDC_CHECK_DEBUG] = Option(L"debug");
    m_options[IDC_CHECK_EVIL] = Option(L"evil");
    m_options[IDC_CHECK_CONTINUE] = Option(L"continue");
    m_options[IDC_CHECK_FORIN] = Option(L"forin");
    m_options[IDC_CHECK_SUB] = Option(L"sub");
    m_options[IDC_CHECK_CSS] = Option(L"css");
    m_options[IDC_CHECK_TODO] = Option(L"todo");
    m_options[IDC_CHECK_ON] = Option(L"on");
    m_options[IDC_CHECK_FRAGMENT] = Option(L"fragment");
    m_options[IDC_CHECK_ES5] = Option(L"es5");
    m_options[IDC_CHECK_VARS] = Option(L"vars");
    m_options[IDC_CHECK_UNDEF] = Option(L"undef");
    m_options[IDC_CHECK_NOMEN] = Option(L"nomen");
    m_options[IDC_CHECK_NODE] = Option(L"node");
    m_options[IDC_CHECK_PLUSPLUS] = Option(L"plusplus");
    m_options[IDC_CHECK_BITWISE] = Option(L"bitwise");
    m_options[IDC_CHECK_REGEXP] = Option(L"regexp");
    m_options[IDC_CHECK_NEWCAP] = Option(L"newcap");
    m_options[IDC_CHECK_UNPARAM] = Option(L"unparam");
    m_options[IDC_CHECK_SLOPPY] = Option(L"sloppy");
    m_options[IDC_CHECK_EQEQ] = Option(L"eqeq");
    m_options[IDC_CHECK_STUPID] = Option(L"stupid");

    m_options[IDC_IDENT] = Option(OPTION_TYPE_INT, L"indent", L"4");
    m_options[IDC_MAXLEN] = Option(OPTION_TYPE_INT, L"maxlen", L"");
    m_options[IDC_MAXERR] = Option(OPTION_TYPE_INT, L"maxerr", L"50");
}

std::wstring JSLintLinterOptions::GetOptionsCommentString() const
{
    return L"/*jslint " + Linter_Options::GetOptionsCommentString() + L" */";
}

int JSLintLinterOptions::GetTabWidth() const
{
    int indent;
    if (_stscanf(m_options.at(IDC_IDENT).value.c_str(), L"%d", &indent) == EOF
        || indent < 1)
    {
        return 4;
    }
    return indent;
}

std::optional<std::wstring> JSLintLinterOptions::check_valid(
    int id, std::wstring const &value
) const
{
    if (value.empty())
    {
        return std::nullopt;
    }

    switch (id)
    {
        case IDC_IDENT:
        {
            int indent;
            if (_stscanf(value.c_str(), L"%d", &indent) == EOF || indent < 0)
            {
                return (
                    L"Indentation must be an integer greater than or "
                    L"equal to zero"
                );
            }
            break;
        }

        case IDC_MAXLEN:
        {
            int maxlen;
            if (_stscanf(value.c_str(), L"%d", &maxlen) == EOF || maxlen < 1)
            {
                return (
                    L"Maximum line length must be an integer greater than "
                    L"zero"
                );
            }
            break;
        }

        case IDC_MAXERR:
        {
            int maxerr;
            if (_stscanf(value.c_str(), L"%d", &maxerr) == EOF || maxerr < 1)
            {
                return (
                    L"Maximum numer of errors must be an integer greater "
                    L"than zero"
                );
            }
        }
    }
    return std::nullopt;
}
