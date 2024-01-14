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

#include "DownloadJSLint.h"
#include "JSLintNpp.h"
#include "Linter.h"
#include "Option.h"
#include "Util.h"

#include "resource.h"

#include <windowsx.h>

#include <map>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// REMOVE!
// Used in JSLintOptions and ScriptSourceDef (as as MAX_BUILD I think)
// Abstraction needed
#ifndef PROFILE_JSLINT_GROUP_NAME
#define PROFILE_JSLINT_GROUP_NAME L"JSLint"
#define PROFILE_BUILD_KEY_NAME L"build"
#endif    // !PROFILE_JSLINT_GROUP_NAME

////////////////////////////////////////////////////////////////////////////////

#define PROFILE_SELECTED_LINTER_KEY_NAME L"selected_linter"
#define PROFILE_JSLINT_OPTIONS_GROUP_NAME L"JSLint Options"
#define PROFILE_JSHINT_OPTIONS_GROUP_NAME L"JSHint Options"
#define PROFILE_ADDITIONAL_OPTIONS_KEY_NAME L"jslintnpp_additional_options"

// This is defined in 2 places. Why?
// In fact a whole bunch of this looks common to this and ScriptSourceDef class.
#define MIN_VERSION_BUILD 110

////////////////////////////////////////////////////////////////////////////////

LinterOptions::LinterOptions(
    LPCTSTR optionsGroupName, std::wstring const &options_file
) :
    m_optionsGroupName(optionsGroupName),
    options_file_(options_file)
{
    m_options[IDC_PREDEFINED] = Option(OPTION_TYPE_ARR_STRING, L"predef", L"");
}

void LinterOptions::ReadOptions()
{
    TCHAR szValue[65536];    // memory is cheap
    std::wstring strConfigFileName = options_file_;
    if (Path::IsFileExists(strConfigFileName))
    {
        GetPrivateProfileString(
            PROFILE_JSLINT_GROUP_NAME,
            PROFILE_BUILD_KEY_NAME,
            NULL,
            szValue,
            _countof(szValue),
            strConfigFileName.c_str()
        );
        if (_ttoi(szValue) >= MIN_VERSION_BUILD)
        {
            for (auto &option : m_options)
            {
                GetPrivateProfileString(
                    m_optionsGroupName,
                    option.second.name.c_str(),
                    NULL,
                    szValue,
                    _countof(szValue),
                    strConfigFileName.c_str()
                );
                if (_tcscmp(szValue, L"") != 0)
                {
                    std::wstring strValue = TrimSpaces(szValue);
                    if (option.second.type == OPTION_TYPE_BOOL)
                    {
                        if (strValue == L"true" || strValue == L"false"
                            || strValue.empty())
                        {
                            option.second.value = strValue;
                        }
                    }
                    else if (option.second.type == OPTION_TYPE_INT)
                    {
                        int value;
                        if (_stscanf(strValue.c_str(), L"%d", &value) != EOF
                            && value > 0)
                        {
                            option.second.value = strValue;
                        }
                    }
                    else if (option.second.type == OPTION_TYPE_ARR_STRING)
                    {
                        option.second.value = strValue;
                    }
                }
            }

            GetPrivateProfileString(
                m_optionsGroupName,
                PROFILE_ADDITIONAL_OPTIONS_KEY_NAME,
                NULL,
                szValue,
                _countof(szValue),
                strConfigFileName.c_str()
            );
            m_additionalOptions = szValue;
        }
    }
}

void LinterOptions::SaveOptions()
{
    std::wstring strConfigFileName = options_file_;

    for (auto const & option : m_options)
    {
        WritePrivateProfileString(
            m_optionsGroupName,
            option.second.name.c_str(),
            option.second.value.c_str(),
            strConfigFileName.c_str()
        );
    }

    WritePrivateProfileString(
        m_optionsGroupName,
        PROFILE_ADDITIONAL_OPTIONS_KEY_NAME,
        m_additionalOptions.c_str(),
        strConfigFileName.c_str()
    );
}

bool LinterOptions::IsOptionIncluded(Option const &option) const
{
    return ! option.value.empty();
}

std::wstring LinterOptions::GetOptionsCommentString() const
{
    std::wstring strOptions;

    for (auto const &option : m_options)
    {
        if (IsOptionIncluded(option.second))
        {
            if (option.first != IDC_PREDEFINED)
            {
                if (! strOptions.empty())
                {
                    strOptions += L", ";
                }
                strOptions += option.second.name + L": " + option.second.value;
            }
        }
    }

    return strOptions;
}

std::wstring LinterOptions::GetOptionsJSONString() const
{
    std::wstring strOptions;

    for (auto const & option : m_options)
    {
        if (not IsOptionIncluded(option.second))
        {
            continue;
        }

        std::wstring value;

        if (option.second.type == OPTION_TYPE_ARR_STRING)
        {
            std::vector<std::wstring> arr;
            StringSplit(option.second.value, L",", arr);
            std::vector<std::wstring>::const_iterator itArr;
            for (itArr = arr.begin(); itArr != arr.end(); ++itArr)
            {
                if (value.empty())
                {
                    value += L"[";
                }
                else
                {
                    value += L", ";
                }

                std::wstring element = TrimSpaces(*itArr);
                FindReplace(element, L"\\", L"\\\\");
                FindReplace(element, L"\"", L"\\\"");

                value += L"\"" + element + L"\"";
            }
            if (! value.empty())
            {
                value += L"]";
            }
        }
        else
        {
            value = option.second.value;
        }

        if (! value.empty())
        {
            if (! strOptions.empty())
            {
                strOptions += L", ";
            }
            strOptions += option.second.name + L": " + value;
        }
    }

    if (! m_additionalOptions.empty())
    {
        if (! strOptions.empty())
        {
            strOptions += L", ";
        }
        strOptions += m_additionalOptions;
    }

    return L"{ " + strOptions + L" }";
}

void LinterOptions::CheckOption(UINT id)
{
    m_options[id].value = L"true";
}

void LinterOptions::UncheckOption(UINT id)
{
    m_options[id].value = L"false";
}

void LinterOptions::ClearOption(UINT id)
{
    m_options[id].value = L"";
}

void LinterOptions::SetOption(UINT id, std::wstring const &value)
{
    m_options[id].value = value;
}

void LinterOptions::AppendOption(UINT id, std::wstring const &value)
{
    Option &option = m_options[id];
    if (option.value.empty())
    {
        option.value = value;
    }
    else
    {
        option.value += L", " + value;
    }
}

void LinterOptions::ResetOption(UINT id)
{
    m_options[id].value = m_options[id].defaultValue;
}

void LinterOptions::SetAdditionalOptions(std::wstring const &additionalOptions)
{
    m_additionalOptions = additionalOptions;
}

void LinterOptions::ClearAllOptions()
{
    for (auto & option : m_options)
    {
        if (option.first != IDC_PREDEFINED)
        {
            option.second.value = option.second.defaultValue;
        }
    }
}

BOOL LinterOptions::UpdateOptions(
    HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
)
{
    ////AARRGGGHH
    if (bSaveOrValidate)
    {
        for (auto const & option : m_options)
        {
            if (option.second.type == OPTION_TYPE_BOOL)
            {
                int checkState =
                    Button_GetCheck(GetDlgItem(hSubDlg, option.first));
                if (checkState == BST_UNCHECKED)
                {
                    UncheckOption(option.first);
                }
                else if (checkState == BST_CHECKED)
                {
                    CheckOption(option.first);
                }
                else
                {
                    ClearOption(option.first);
                }
            }
        }

        // predefined
        std::wstring strPredefined =
            TrimSpaces(GetWindowText(GetDlgItem(hDlg, IDC_PREDEFINED)));
        if (strPredefined.empty())
        {
            ResetOption(IDC_PREDEFINED);
        }
        else
        {
            SetOption(IDC_PREDEFINED, strPredefined);
        }

        // additional options
        std::wstring strAdditionalOptions =
            TrimSpaces(GetWindowText(GetDlgItem(hDlg, IDC_ADDITIONAL_OPTIONS)));
        SetAdditionalOptions(strAdditionalOptions);
    }
    else
    {
        std::map<UINT, Option>::iterator it;
        for (it = m_options.begin(); it != m_options.end(); ++it)
        {
            if (it->second.type == OPTION_TYPE_BOOL)
            {
                int checkState;
                if (it->second.value == L"false")
                {
                    checkState = BST_UNCHECKED;
                }
                else if (it->second.value == L"true")
                {
                    checkState = BST_CHECKED;
                }
                else
                {
                    checkState = BST_INDETERMINATE;
                }
                Button_SetCheck(GetDlgItem(hSubDlg, it->first), checkState);
            }
            else if (it->second.type == OPTION_TYPE_INT || it->second.type == OPTION_TYPE_ARR_STRING)
            {
                if (GetDlgItem(hDlg, it->first))
                {
                    SetWindowText(
                        GetDlgItem(hDlg, it->first), it->second.value.c_str()
                    );
                }
                else
                {
                    SetWindowText(
                        GetDlgItem(hSubDlg, it->first), it->second.value.c_str()
                    );
                }
            }
        }

        SetWindowText(
            GetDlgItem(hDlg, IDC_ADDITIONAL_OPTIONS),
            m_additionalOptions.c_str()
        );
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

JSLintLinterOptions::JSLintLinterOptions(std::wstring const &options_file) :
    LinterOptions(PROFILE_JSLINT_OPTIONS_GROUP_NAME, options_file)
{
    m_options[IDC_CHECK_PASSFAIL] = Option(L"passfail");
    m_options[IDC_CHECK_WHITE] = Option(L"white");
    m_options[IDC_CHECK_BROWSER] = Option(L"browser");
    m_options[IDC_CHECK_DEVEL] = Option(L"devel");
    m_options[IDC_CHECK_WINDOWS] = Option(L"windows");
    m_options[IDC_CHECK_RHINO] = Option(L"rhino");
    m_options[IDC_CHECK_SAFE] = Option(L"safe");
    m_options[IDC_CHECK_ADSAFE] = Option(L"adsafe");
    m_options[IDC_CHECK_DEBUG] = Option(L"debug");
    m_options[IDC_CHECK_EVIL] = Option(L"evil");
    m_options[IDC_CHECK_CONTINUE] = Option(L"continue");
    m_options[IDC_CHECK_FORIN] = Option(L"forin");
    m_options[IDC_CHECK_SUB] = Option(L"sub");
    m_options[IDC_CHECK_CSS] = Option(L"css");
    m_options[IDC_CHECK_TODO] = Option(L"todo");
    m_options[IDC_CHECK_ON] = Option(L"on");
    m_options[IDC_CHECK_FRAGMENT] = Option(L"fragment");
    m_options[IDC_CHECK_ES5] = Option(L"es5");
    m_options[IDC_CHECK_VARS] = Option(L"vars");
    m_options[IDC_CHECK_UNDEF] = Option(L"undef");
    m_options[IDC_CHECK_NOMEN] = Option(L"nomen");
    m_options[IDC_CHECK_NODE] = Option(L"node");
    m_options[IDC_CHECK_PLUSPLUS] = Option(L"plusplus");
    m_options[IDC_CHECK_BITWISE] = Option(L"bitwise");
    m_options[IDC_CHECK_REGEXP] = Option(L"regexp");
    m_options[IDC_CHECK_NEWCAP] = Option(L"newcap");
    m_options[IDC_CHECK_UNPARAM] = Option(L"unparam");
    m_options[IDC_CHECK_SLOPPY] = Option(L"sloppy");
    m_options[IDC_CHECK_EQEQ] = Option(L"eqeq");
    m_options[IDC_CHECK_STUPID] = Option(L"stupid");

    m_options[IDC_IDENT] = Option(OPTION_TYPE_INT, L"indent", L"4");
    m_options[IDC_MAXLEN] = Option(OPTION_TYPE_INT, L"maxlen", L"");
    m_options[IDC_MAXERR] = Option(OPTION_TYPE_INT, L"maxerr", L"50");
}

int JSLintLinterOptions::GetTabWidth()
{
    int indent;
    if (_stscanf(m_options[IDC_IDENT].value.c_str(), L"%d", &indent) == EOF
        || indent < 1)
    {
        return 4;
    }
    return indent;
}

BOOL JSLintLinterOptions::UpdateOptions(
    HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
)
{
    if (! LinterOptions::UpdateOptions(
            hDlg, hSubDlg, bSaveOrValidate, bShowErrorMessage
        ))
    {
        return FALSE;
    }

    if (bSaveOrValidate)
    {
        // indent
        std::wstring strIndent =
            TrimSpaces(GetWindowText(GetDlgItem(hSubDlg, IDC_IDENT)));
        if (! strIndent.empty())
        {
            int indent;
            if (_stscanf(strIndent.c_str(), L"%d", &indent) == EOF
                || indent < 0)
            {
                if (bShowErrorMessage)
                {
                    MessageBox(
                        hSubDlg,
                        (L"Indentation must be an integer greater than or "
                         L"equal to zero"),
                        L"JSLint",
                        MB_OK | MB_ICONERROR
                    );
                    SetFocus(GetDlgItem(hDlg, IDC_IDENT));
                }
                return FALSE;
            }
        }
        SetOption(IDC_IDENT, strIndent);

        // maxlen
        std::wstring strMaxlen =
            TrimSpaces(GetWindowText(GetDlgItem(hSubDlg, IDC_MAXLEN)));
        if (! strMaxlen.empty())
        {
            int maxlen;
            if (_stscanf(strMaxlen.c_str(), L"%d", &maxlen) == EOF
                || maxlen < 1)
            {
                if (bShowErrorMessage)
                {
                    MessageBox(
                        hSubDlg,
                        (L"Maximum line length must be an integer greater than "
                         L"zero"),
                        L"JSLint",
                        MB_OK | MB_ICONERROR
                    );
                    SetFocus(GetDlgItem(hDlg, IDC_MAXLEN));
                }
                return FALSE;
            }
        }
        SetOption(IDC_MAXLEN, strMaxlen);

        // maxerr
        std::wstring strMaxerr =
            TrimSpaces(GetWindowText(GetDlgItem(hSubDlg, IDC_MAXERR)));
        if (! strMaxerr.empty())
        {
            int maxerr;
            if (_stscanf(strMaxerr.c_str(), L"%d", &maxerr) == EOF
                || maxerr < 1)
            {
                if (bShowErrorMessage)
                {
                    MessageBox(
                        hSubDlg,
                        (L"Maximum numer of errors must be an integer greater "
                         L"than zero"),
                        L"JSLint",
                        MB_OK | MB_ICONERROR
                    );
                    SetFocus(GetDlgItem(hDlg, IDC_MAXERR));
                }
                return FALSE;
            }
        }
        SetOption(IDC_MAXERR, strMaxerr);
    }

    return TRUE;
}

std::wstring JSLintLinterOptions::GetOptionsCommentString() const
{
    return L"/*jslint " + LinterOptions::GetOptionsCommentString() + L" */";
}

////////////////////////////////////////////////////////////////////////////////

JSHintLinterOptions::JSHintLinterOptions(std::wstring const &config_file) :
    LinterOptions(PROFILE_JSHINT_OPTIONS_GROUP_NAME, config_file)
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

int JSHintLinterOptions::GetTabWidth()
{
    return 4;
}

std::wstring JSHintLinterOptions::GetOptionsCommentString() const
{
    return L"/*jshint " + LinterOptions::GetOptionsCommentString() + L" */";
}

////////////////////////////////////////////////////////////////////////////////

JSLintOptions JSLintOptions::m_options(L"");
JSLintOptions *JSLintOptions::m_m_options;
HWND JSLintOptions::m_hDlg;
HWND JSLintOptions::m_hWndJSLintOptionsSubdlg;
HWND JSLintOptions::m_hWndJSHintOptionsSubdlg;
HWND JSLintOptions::m_hSubDlg;

JSLintOptions::JSLintOptions(std::wstring const &options_file) :
    options_file_(options_file),
    m_selectedLinter(Linter::LINTER_JSLINT),
    m_jsLintOptions(options_file),
    m_jsHintOptions(options_file)
{
    m_m_options = this;
}

// FIXME
//**************************************************************************************
//
// DESTRUCTOR SHOULD SAVE CONFIG
//
//  SHOULD READOPTIONS be part of the constructor(s)
//**************************************************************************************

void JSLintOptions::ReadOptions()
{
    std::wstring strConfigFileName = options_file_;
    if (Path::IsFileExists(strConfigFileName))
    {
        TCHAR szValue[65536];    // memory is cheap
        GetPrivateProfileString(
            PROFILE_JSLINT_GROUP_NAME,
            PROFILE_SELECTED_LINTER_KEY_NAME,
            NULL,
            szValue,
            _countof(szValue),
            strConfigFileName.c_str()
        );
        if (_tcscmp(szValue, L"JSHint") == 0)
        {
            m_selectedLinter = Linter::LINTER_JSHINT;
        }
    }
    m_jsLintOptions.ReadOptions();
    m_jsHintOptions.ReadOptions();
}

void JSLintOptions::SaveOptions()
{
    std::wstring strConfigFileName = options_file_;

    WritePrivateProfileString(
        PROFILE_JSLINT_GROUP_NAME,
        PROFILE_SELECTED_LINTER_KEY_NAME,
        m_selectedLinter == Linter::LINTER_JSLINT ? L"JSLint" : L"JSHint",
        strConfigFileName.c_str()
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

LinterOptions *JSLintOptions::GetSelectedLinterOptions()
{
    if (m_selectedLinter == Linter::LINTER_JSLINT)
    {
        return &m_jsLintOptions;
    }
    return &m_jsHintOptions;
}

LinterOptions const *JSLintOptions::GetSelectedLinterOptions() const
{
    if (m_selectedLinter == Linter::LINTER_JSLINT)
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

void JSLintOptions::ClearAllOptions()
{
    GetSelectedLinterOptions()->ClearAllOptions();
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
                    m_options.ClearAllOptions();
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
    plugin->pluginDialogBox(IDD_OPTIONS, DlgProc, plugin);
}
