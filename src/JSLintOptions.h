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

#include <string>

class JSLintNpp;
class Profile_Handler;

////////////////////////////////////////////////////////////////////////////////

class JSLintLinterOptions : public Linter_Options
{
  public:
    JSLintLinterOptions(Profile_Handler *profile_handler);

    int GetTabWidth();
    BOOL UpdateOptions(
        HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
    );
    std::wstring GetOptionsCommentString() const;
};

////////////////////////////////////////////////////////////////////////////////

class JSHintLinterOptions : public Linter_Options
{
  public:
    JSHintLinterOptions(Profile_Handler *profile_handler);

    int GetTabWidth();
    std::wstring GetOptionsCommentString() const;
};

////////////////////////////////////////////////////////////////////////////////

enum class Linter;

class JSLintOptions
{
  public:
    JSLintOptions(Profile_Handler *);

    void ReadOptions();
    void SaveOptions();

    Linter GetSelectedLinter() const;
    void SetSelectedLinter(Linter selectedLinter);
    Linter_Options const *GetSelectedLinterOptions() const;
    Linter_Options *GetSelectedLinterOptions();

    std::wstring GetOptionsJSONString() const;

    int GetTabWidth();
    BOOL UpdateOptions(
        HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
    );
    void AppendOption(UINT id, std::wstring const &value);
    void ClearAllOptions();
    void ShowDialog(JSLintNpp const *);

  private:
    Profile_Handler *profile_handler_;
    //std::wstring options_file_;
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
