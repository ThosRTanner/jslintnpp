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

#include "JSLintOptions.h"

#include "ScriptSourceDef.h"

#include <string>

class JSLintNpp;

////////////////////////////////////////////////////////////////////////////////
//REMOVE!
//Used in JSLintOptions and ScriptSourceDef (as as MAX_BUILD I think)
//Abstraction needed
#ifndef PROFILE_JSLINT_GROUP_NAME
#define PROFILE_JSLINT_GROUP_NAME L"JSLint"
#define PROFILE_BUILD_KEY_NAME L"build"
#endif    // !PROFILE_JSLINT_GROUP_NAME


class Settings_Dialogue;

class Settings
{
  public:
    Settings(JSLintNpp const *);

    void ReadOptions();
    void SaveOptions();

    void ShowDialog();

    ScriptSourceDef const &GetScriptSource(Linter linter) const;

  private:
    JSLintNpp const *plugin_;
    std::wstring config_file_;

    ScriptSourceDef m_jsLintScript;
    ScriptSourceDef m_jsHintScript;

    void ReadOptions(
        std::wstring const &prefix, ScriptSourceDef &scriptSourceDef
    );
    void SaveOptions(
        std::wstring const &prefix, ScriptSourceDef const &scriptSourceDef
    );

    friend class Settings_Dialogue;
};
