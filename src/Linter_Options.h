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

#include "Option.h"

#include <map>
#include <string>

class Profile_Handler;

class Linter_Options
{
  public:
    Linter_Options(LPCTSTR optionsGroupName, Profile_Handler *profile_handler);

    void ReadOptions();
    void SaveOptions();

    virtual std::wstring GetOptionsCommentString() const;
    std::wstring GetOptionsJSONString() const;

    void CheckOption(UINT id);
    void UncheckOption(UINT id);
    void ClearOption(UINT id);

    void SetOption(UINT id, std::wstring const &value);
    void AppendOption(UINT id, std::wstring const &value);
    void ResetOption(UINT id);

    void ResetAllOptions();

    virtual int GetTabWidth() = 0;

    virtual BOOL UpdateOptions(
        HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
    );

  protected:
    LPCTSTR m_optionsGroupName;
    std::map<UINT, Option> m_options;

  private:
    Profile_Handler *profile_handler_;
};
