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
#include "Option.h"
#include "Options_Dialogue.h"
#include "Profile_Handler.h"
#include "Util.h"

#include "resource.h"

#include <windowsx.h>

#include <string>

////////////////////////////////////////////////////////////////////////////////
// REMOVE!
// Used in JSLintOptions and ScriptSourceDef (as as MAX_BUILD I think)
// Abstraction needed
#ifndef PROFILE_JSLINT_GROUP_NAME
#define PROFILE_JSLINT_GROUP_NAME L"JSLint"
//#define PROFILE_BUILD_KEY_NAME L"build"
#endif    // !PROFILE_JSLINT_GROUP_NAME

////////////////////////////////////////////////////////////////////////////////

#define PROFILE_SELECTED_LINTER_KEY_NAME L"selected_linter"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

JSLintOptions JSLintOptions::m_options(nullptr); //REMOVE THIS!!!
JSLintOptions *JSLintOptions::m_m_options;
HWND JSLintOptions::m_hDlg;
HWND JSLintOptions::m_hWndJSLintOptionsSubdlg;
HWND JSLintOptions::m_hWndJSHintOptionsSubdlg;
HWND JSLintOptions::m_hSubDlg;

JSLintOptions::JSLintOptions(Profile_Handler *profile_handler) :
    profile_handler_(profile_handler),
    m_selectedLinter(Linter::LINTER_JSLINT),
    m_jsLintOptions(profile_handler),
    m_jsHintOptions(profile_handler)
{
    m_m_options = this;
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
    if (profile_handler_ == nullptr) //FIXME
    {
        return;
    }

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

BOOL JSLintOptions::UpdateOptions(
    HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
)
{
    if (! GetSelectedLinterOptions()->UpdateOptions(
            hDlg, hSubDlg, bSaveOrValidate, bShowErrorMessage
        ))
    {
        return FALSE;
    }

    SetWindowText(
        GetDlgItem(hDlg, IDC_PREVIEW),
        GetSelectedLinterOptions()->GetOptionsCommentString().c_str()
    );

    return TRUE;
}

void JSLintOptions::AppendOption(UINT id, std::wstring const &value)
{
    GetSelectedLinterOptions()->AppendOption(id, value);
}

void JSLintOptions::ResetAllOptions()
{
    GetSelectedLinterOptions()->ResetAllOptions();
}

INT_PTR CALLBACK JSLintOptions::PredefinedControlWndProc(
    HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam
)
{
    if (uMessage == WM_PASTE)
    {
        if (IsClipboardFormatAvailable(CF_TEXT))
        {
            if (OpenClipboard(NULL))
            {
                HGLOBAL hGlobal = GetClipboardData(CF_TEXT);
                if (hGlobal)
                {
                    LPSTR lpData = (LPSTR)GlobalLock(hGlobal);
                    if (lpData != NULL)
                    {
                        std::wstring str(TextConversion::A_To_T(lpData));

                        std::vector<std::wstring> results;
                        StringSplit(str, L" \t\r\n", results);
                        str = StringJoin(results, L", ");

                        SendMessage(
                            hWnd, EM_REPLACESEL, TRUE, (LPARAM)str.c_str()
                        );
                    }
                    GlobalUnlock(hGlobal);
                }
                CloseClipboard();
            }
        }
        return 0;
    }

    WNDPROC oldWndProc = (WNDPROC)GetProp(hWnd, L"OldWndProc");
    return (*oldWndProc)(hWnd, uMessage, wParam, lParam);
}

INT_PTR CALLBACK JSLintOptions::SubDlgProc(
    HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam
)
{
    if (uMessage == WM_INITDIALOG)
    {
        return TRUE;
    }
    else if (uMessage == WM_COMMAND)
    {
        if (HIWORD(wParam) == BN_CLICKED)
        {
            UINT id = LOWORD(wParam);
            if (LOWORD(wParam) >= IDC_CHECK_FIRST_OPTION
                && LOWORD(wParam) <= IDC_CHECK_LAST_OPTION)
            {
                int checkState = Button_GetCheck(GetDlgItem(hDlg, id));
                if (checkState == BST_UNCHECKED)
                {
                    Button_SetCheck(GetDlgItem(hDlg, id), BST_INDETERMINATE);
                }
                else if (checkState == BST_CHECKED)
                {
                    Button_SetCheck(GetDlgItem(hDlg, id), BST_UNCHECKED);
                }
                else
                {
                    Button_SetCheck(GetDlgItem(hDlg, id), BST_CHECKED);
                }
                m_options.UpdateOptions(m_hDlg, m_hSubDlg, true, false);
            }
        }
    }

    return 0;
}

INT_PTR CALLBACK
JSLintOptions::DlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    m_hDlg = hDlg;
    if (uMessage == WM_INITDIALOG)
    {
        auto plugin = reinterpret_cast<Plugin const *>(lParam);

        // FIXME
        CenterWindow(hDlg, plugin->get_notepad_window());

        m_options = *m_m_options;

        // initialize selected linter combo box
        HWND hWndSelectedLinter = GetDlgItem(hDlg, IDC_SELECTED_LINTER);
        ComboBox_AddString(hWndSelectedLinter, L"JSLint");
        ComboBox_AddString(hWndSelectedLinter, L"JSHint");

        // create JSLint and JSHint options subdialog
        HWND hWndOptionsPlaceholder = GetDlgItem(hDlg, IDC_OPTIONS_PLACEHOLDER);
        RECT rect;
        GetWindowRect(hWndOptionsPlaceholder, &rect);
        POINT ptTopLeft;
        ptTopLeft.x = rect.left;
        ptTopLeft.y = rect.top;
        ScreenToClient(hDlg, &ptTopLeft);

        m_hWndJSLintOptionsSubdlg = CreateDialog(
            plugin->module(),
            MAKEINTRESOURCE(IDD_JSLINT_OPTIONS),
            hDlg,
            SubDlgProc
        );
        SetWindowPos(
            m_hWndJSLintOptionsSubdlg,
            hWndOptionsPlaceholder,
            ptTopLeft.x,
            ptTopLeft.y,
            rect.right - rect.left,
            rect.bottom - rect.top,
            0
        );

        m_hWndJSHintOptionsSubdlg = CreateDialog(
            plugin->module(),
            MAKEINTRESOURCE(IDD_JSHINT_OPTIONS),
            hDlg,
            SubDlgProc
        );
        SetWindowPos(
            m_hWndJSHintOptionsSubdlg,
            hWndOptionsPlaceholder,
            ptTopLeft.x,
            ptTopLeft.y,
            rect.right - rect.left,
            rect.bottom - rect.top,
            0
        );

        if (m_options.GetSelectedLinter() == Linter::LINTER_JSLINT)
        {
            m_hSubDlg = m_hWndJSLintOptionsSubdlg;
            ComboBox_SelectString(hWndSelectedLinter, 0, L"JSLint");
        }
        else
        {
            m_hSubDlg = m_hWndJSHintOptionsSubdlg;
            ComboBox_SelectString(hWndSelectedLinter, 0, L"JSHint");
        }
        m_options.UpdateOptions(m_hDlg, m_hSubDlg, false, false);
        ShowWindow(m_hSubDlg, SW_SHOW);

        // subclass IDC_PREDEFINED
        HWND hWndPredefined = GetDlgItem(hDlg, IDC_PREDEFINED);
        WNDPROC oldWndProc = (WNDPROC)SetWindowLongPtr(
            hWndPredefined, GWLP_WNDPROC, (LONG_PTR)PredefinedControlWndProc
        );
        SetProp(hWndPredefined, L"OldWndProc", (HANDLE)oldWndProc);
    }
    else if (uMessage == WM_COMMAND)
    {
        if (HIWORD(wParam) == BN_CLICKED)
        {
            switch (LOWORD(wParam))
            {
                case IDC_CLEAR_ALL:
                    m_options.UpdateOptions(m_hDlg, m_hSubDlg, true, false);
                    m_options.ResetAllOptions();
                    m_options.UpdateOptions(m_hDlg, m_hSubDlg, false, false);
                    break;
                case IDOK:
                    if (m_options.UpdateOptions(m_hDlg, m_hSubDlg, true, true))
                    {
                        *m_m_options = m_options;
                        EndDialog(hDlg, 1);
                    }
                    return 1;
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    return 1;
                default:
                    break;
            }
        }
        else if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            HWND hWndSelectedLinter = GetDlgItem(hDlg, IDC_SELECTED_LINTER);
            int curSel = ComboBox_GetCurSel(hWndSelectedLinter);
            if (curSel != CB_ERR)
            {
                if (m_options.UpdateOptions(m_hDlg, m_hSubDlg, true, true))
                {
                    TCHAR buffer[32];
                    ComboBox_GetLBText(hWndSelectedLinter, curSel, buffer);

                    ShowWindow(m_hSubDlg, SW_HIDE);

                    if (_tcsicmp(buffer, L"JSLint") == 0)
                    {
                        m_options.SetSelectedLinter(Linter::LINTER_JSLINT);
                        m_hSubDlg = m_hWndJSLintOptionsSubdlg;
                    }
                    else
                    {
                        m_options.SetSelectedLinter(Linter::LINTER_JSHINT);
                        m_hSubDlg = m_hWndJSHintOptionsSubdlg;
                    }

                    m_options.UpdateOptions(m_hDlg, m_hSubDlg, false, false);
                    ShowWindow(m_hSubDlg, SW_SHOW);
                }
                else
                {
                    if (m_options.GetSelectedLinter() == Linter::LINTER_JSLINT)
                    {
                        ComboBox_SelectString(hWndSelectedLinter, 0, L"JSLint");
                    }
                    else
                    {
                        ComboBox_SelectString(hWndSelectedLinter, 0, L"JSHint");
                    }
                }
            }
        }
        else if (HIWORD(wParam) == EN_KILLFOCUS)
        {
            m_options.UpdateOptions(m_hDlg, m_hSubDlg, true, false);
        }
    }
    else if (uMessage == WM_SYSCOMMAND)
    {
        if (wParam == SC_CLOSE)
        {
            // cancel
            EndDialog(hDlg, 0);
            return 1;
        }
    }
    return 0;
}

void JSLintOptions::ShowDialog(JSLintNpp const *plugin)
{
    Options_Dialogue dialogue(plugin);
    plugin->pluginDialogBox(IDD_OPTIONS, DlgProc, plugin);
}
