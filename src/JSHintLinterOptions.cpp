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

#include "JSHintLinterOptions.h"

#include "resource.h"

#include <string>

JSHintLinterOptions::JSHintLinterOptions(Profile_Handler *profile_handler) :
    Linter_Options(L"JSHint Options", profile_handler)
{
    m_options[IDC_CHECK_DEBUG] = Option(L"debug");
    m_options[IDC_CHECK_FORIN] = Option(L"forin");
    m_options[IDC_CHECK_EQNULL] = Option(L"eqnull");
    m_options[IDC_CHECK_NOARG] = Option(L"noarg");
    m_options[IDC_CHECK_NOEMPTY] = Option(L"noempty");
    m_options[IDC_CHECK_EQEQEQ] = Option(L"eqeqeq");
    m_options[IDC_CHECK_BOSS] = Option(L"boss");
    m_options[IDC_CHECK_LOOPFUNC] = Option(L"loopfunc");
    m_options[IDC_CHECK_EVIL] = Option(L"evil");
    m_options[IDC_CHECK_LAXBREAK] = Option(L"laxbreak");
    m_options[IDC_CHECK_BITWISE] = Option(L"bitwise");
    m_options[IDC_CHECK_STRICT] = Option(L"strict");
    m_options[IDC_CHECK_UNDEF] = Option(L"undef");
    m_options[IDC_CHECK_CURLY] = Option(L"curly");
    m_options[IDC_CHECK_NONEW] = Option(L"nonew");
    m_options[IDC_CHECK_BROWSER] = Option(L"browser");
    m_options[IDC_CHECK_DEVEL] = Option(L"devel");
    m_options[IDC_CHECK_JQUERY] = Option(L"jquery");
    m_options[IDC_CHECK_ES5] = Option(L"es5");
    m_options[IDC_CHECK_NODE] = Option(L"node");
}

std::wstring JSHintLinterOptions::GetOptionsCommentString() const
{
    return L"/*jshint " + Linter_Options::GetOptionsCommentString() + L" */";
}

int JSHintLinterOptions::GetTabWidth() const
{
    return 4;
}

