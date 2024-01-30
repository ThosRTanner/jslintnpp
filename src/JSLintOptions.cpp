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

#include "JSLintOptions.h"

#include "JSLintNpp.h"
#include "Linter.h"
#include "Options_Dialogue.h"
#include "Profile_Handler.h"
//#include "Util.h"

#include <string>

////////////////////////////////////////////////////////////////////////////////
// REMOVE!
// Used in JSLintOptions and ScriptSourceDef (as as MAX_BUILD I think)
// Abstraction needed
#ifndef PROFILE_JSLINT_GROUP_NAME
#define PROFILE_JSLINT_GROUP_NAME L"JSLint"
#endif    // !PROFILE_JSLINT_GROUP_NAME

#define PROFILE_SELECTED_LINTER_KEY_NAME L"selected_linter"

JSLintOptions::JSLintOptions(Profile_Handler *profile_handler) :
    profile_handler_(profile_handler),
    m_selectedLinter(Linter::LINTER_JSLINT),
    m_jsLintOptions(profile_handler),
    m_jsHintOptions(profile_handler)
{
}

JSLintOptions::~JSLintOptions()
{
}

// FIXME
//**************************************************************************************
//
// DESTRUCTOR SHOULD SAVE CONFIG
//
//  READOPTIONS SHOULD be part of the constructor(s)
//**************************************************************************************

void JSLintOptions::ReadOptions()
{
    m_selectedLinter =
        profile_handler_->get_str_value(
            PROFILE_JSLINT_GROUP_NAME, PROFILE_SELECTED_LINTER_KEY_NAME
        ) == L"JSHint"
        ? Linter::LINTER_JSHINT
        : Linter::LINTER_JSLINT;

    m_jsLintOptions.ReadOptions();
    m_jsHintOptions.ReadOptions();
}

void JSLintOptions::SaveOptions()
{
    profile_handler_->set_str_value(
        PROFILE_JSLINT_GROUP_NAME,
        PROFILE_SELECTED_LINTER_KEY_NAME,
        m_selectedLinter == Linter::LINTER_JSLINT ? L"JSLint" : L"JSHint"
    );
    m_jsLintOptions.SaveOptions();
    m_jsHintOptions.SaveOptions();
}

Linter JSLintOptions::GetSelectedLinter() const
{
    return m_selectedLinter;
}

void JSLintOptions::SetSelectedLinter(Linter selectedLinter)
{
    m_selectedLinter = selectedLinter;
}

Linter_Options const *JSLintOptions::GetLinterOptions(Linter linter) const
{
    if (linter == Linter::LINTER_JSLINT)
    {
        return &m_jsLintOptions;
    }
    return &m_jsHintOptions;
}

std::wstring JSLintOptions::GetOptionsJSONString() const
{
    return GetSelectedLinterOptions()->GetOptionsJSONString();
}

int JSLintOptions::GetTabWidth()
{
    return GetSelectedLinterOptions()->GetTabWidth();
}

void JSLintOptions::AppendOption(UINT id, std::wstring const &value)
{
    GetSelectedLinterOptions()->AppendOption(id, value);
}

void JSLintOptions::ResetAllOptions()
{
    GetSelectedLinterOptions()->ResetAllOptions();
}

void JSLintOptions::ShowDialog(JSLintNpp const *plugin)
{
    Options_Dialogue dialogue(plugin);
    if (dialogue.get_result() == Options_Dialogue::Clicked_OK)
    {
        *this = *dialogue.get_options();
    }
}
