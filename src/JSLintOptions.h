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

#include "Linter_Options.h"

#include "JSLintLinterOptions.h"
#include "JSHintLinterOptions.h"
#include "Linter.h"

#include <string>

class JSLintNpp;
class Profile_Handler;
enum class Linter;

class JSLintOptions
{
  public:
    JSLintOptions(Profile_Handler *);

    ~JSLintOptions();

    void ReadOptions();

    void SaveOptions();

    Linter GetSelectedLinter() const noexcept;

    void SetSelectedLinter(Linter selectedLinter) noexcept;
 
    Linter_Options const* GetSelectedLinterOptions() const noexcept
    {
        return GetLinterOptions(m_selectedLinter);
    }

    Linter_Options* GetSelectedLinterOptions() noexcept
    {
        return GetLinterOptions(m_selectedLinter);
    }

    Linter_Options const *GetLinterOptions(Linter) const noexcept;

    Linter_Options* GetLinterOptions(Linter linter) noexcept
    {
        return const_cast<Linter_Options *>(
            static_cast<JSLintOptions const *>(this)->GetLinterOptions(linter)
        );
    }

    std::wstring GetOptionsJSONString() const;

    int GetTabWidth();

    void AppendOption(UINT id, std::wstring const &value);
    void ResetAllOptions();
    void ShowDialog(JSLintNpp const *);

  private:
    Profile_Handler *profile_handler_;
    Linter m_selectedLinter;
    JSLintLinterOptions m_jsLintOptions;
    JSHintLinterOptions m_jsHintOptions;
};
