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

    Linter GetSelectedLinter() const;

    void SetSelectedLinter(Linter selectedLinter);

    Linter_Options const* GetSelectedLinterOptions() const
    {
        return GetLinterOptions(m_selectedLinter);
    }

    Linter_Options* GetSelectedLinterOptions()
    {
        return GetLinterOptions(m_selectedLinter);
    }

    Linter_Options const *GetLinterOptions(Linter) const;

    Linter_Options* GetLinterOptions(Linter linter)
    {
        return const_cast<Linter_Options *>(
            static_cast<JSLintOptions const *>(this)->GetLinterOptions(linter)
        );
    }

    std::wstring GetOptionsJSONString() const;

    int GetTabWidth();

    BOOL UpdateOptions(
        HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
    );

    void AppendOption(UINT id, std::wstring const &value);
    void ResetAllOptions();
    void ShowDialog(JSLintNpp const *);

  private:
    Profile_Handler *profile_handler_;
    Linter m_selectedLinter;
    JSLintLinterOptions m_jsLintOptions;
    JSHintLinterOptions m_jsHintOptions;

    // Get rid of this crapness
    //
    // MAYBE A DIALOGUE_INTERFACE like thing. these methods are contorted.
    static JSLintOptions m_options;    // copy of options whilst editing
    static JSLintOptions
        *m_m_options;    // pointer to current options so it can be overwritten.
    static HWND m_hDlg;
    static HWND m_hWndJSLintOptionsSubdlg;
    static HWND m_hWndJSHintOptionsSubdlg;
    static HWND m_hSubDlg;

    static INT_PTR CALLBACK PredefinedControlWndProc(
        HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam
    );

    static INT_PTR CALLBACK
    SubDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK
    DlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
};
