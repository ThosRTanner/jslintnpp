#pragma once

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

#include <string>

enum class Linter;
class Profile_Handler;

enum ScriptSource
{
    SCRIPT_SOURCE_BUILTIN,
    SCRIPT_SOURCE_DOWNLOADED
};

class Settings_Dialogue;

class ScriptSourceDef
{
  public:
    ScriptSourceDef(Linter linter, Profile_Handler *profile_handler);

    ~ScriptSourceDef();

    LPCSTR GetNamespace() const noexcept;
    int GetScriptResourceID() const noexcept;
    std::wstring get_undef_errmsg() const;

  public:
    Linter m_linter;

  private:
    Profile_Handler *profile_handler_;

  public:
    ScriptSource m_scriptSource;
    std::wstring m_scriptVersion;

  private:
    bool m_bSpecUndefVarErrMsg;
    std::wstring m_undefVarErrMsg;

    LPCTSTR GetDefaultUndefVarErrMsg() const noexcept;

    std::wstring prefix() const;

    std::wstring get_settings_value(
        std::wstring const &key, std::wstring const &def_value = L""
    ) const;

    void set_settings_value(std::wstring const &key, std::wstring const &value)
        const;

    friend class Settings_Dialogue;
};
