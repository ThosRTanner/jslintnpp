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

#include "Linter_Options.h"

#include <string>

class JSLintNpp;
class Profile_Handler;

class JSLintLinterOptions : public Linter_Options
{
  public:
    JSLintLinterOptions(Profile_Handler *profile_handler);

    std::wstring GetOptionsCommentString() const override;

    int GetTabWidth() const override;

    std::optional<std::wstring> check_valid(int, std::wstring const &) const override;

    BOOL UpdateOptions(
        HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
    ) override;
};