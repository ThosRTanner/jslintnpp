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

class JSLintNpp;

#include <map>
#include <string>

////////////////////////////////////////////////////////////////////////////////

enum OptionType
{
    OPTION_TYPE_UNKNOWN,
    OPTION_TYPE_BOOL,
    OPTION_TYPE_INT,
    OPTION_TYPE_ARR_STRING
};

////////////////////////////////////////////////////////////////////////////////

struct Option
{
    Option() : type(OPTION_TYPE_UNKNOWN)
    {
    }

    Option(std::wstring const &name) :
        type(OPTION_TYPE_BOOL),
        name(name),
        value(L""),
        defaultValue(L"")
    {
    }

    Option(
        OptionType type, std::wstring const &name, std::wstring const &value
    ) :
        type(type),
        name(name),
        value(value),
        defaultValue(value)
    {
    }

    OptionType type;
    std::wstring name;
    std::wstring value;
    std::wstring defaultValue;
};

////////////////////////////////////////////////////////////////////////////////

class LinterOptions
{
  public:
    LinterOptions(LPCTSTR optionsGroupName, std::wstring const &options_file);

    void ReadOptions();
    void SaveOptions();

    virtual std::wstring GetOptionsCommentString() const;
    std::wstring GetOptionsJSONString() const;

    // std::wstring GetOptionName(UINT id) const;
    UINT GetOptionID(std::wstring const &name) const;

    void CheckOption(UINT id);
    void UncheckOption(UINT id);
    void ClearOption(UINT id);

    void SetOption(UINT id, std::wstring const &value);
    void AppendOption(UINT id, std::wstring const &value);
    void ResetOption(UINT id);

    void SetAdditionalOptions(std::wstring const &additionalOptions);

    void ClearAllOptions();

    bool IsOptionIncluded(Option const &option) const;

    virtual int GetTabWidth() = 0;

    virtual BOOL UpdateOptions(
        HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
    );

  protected:
    LPCTSTR m_optionsGroupName;
    std::map<UINT, Option> m_options;
    std::wstring m_additionalOptions;

  private:
    std::wstring options_file_;
};

////////////////////////////////////////////////////////////////////////////////

class JSLintLinterOptions : public LinterOptions
{
  public:
    JSLintLinterOptions(std::wstring const &options_file);

    int GetTabWidth();
    BOOL UpdateOptions(
        HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
    );
    std::wstring GetOptionsCommentString() const;
};

////////////////////////////////////////////////////////////////////////////////

class JSHintLinterOptions : public LinterOptions
{
  public:
    JSHintLinterOptions(std::wstring const &config_file);

    int GetTabWidth();
    std::wstring GetOptionsCommentString() const;
};

////////////////////////////////////////////////////////////////////////////////

enum Linter
{
    LINTER_JSLINT,
    LINTER_JSHINT
};

////////////////////////////////////////////////////////////////////////////////

class JSLintOptions
{
  public:
    JSLintOptions(std::wstring const &);

    void ReadOptions();
    void SaveOptions();

    Linter GetSelectedLinter() const;
    void SetSelectedLinter(Linter selectedLinter);
    LinterOptions const *GetSelectedLinterOptions() const;
    LinterOptions *GetSelectedLinterOptions();

    std::wstring GetOptionsJSONString() const;

    int GetTabWidth();
    BOOL UpdateOptions(
        HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
    );
    void AppendOption(UINT id, std::wstring const &value);
    void ClearAllOptions();
    void ShowDialog(JSLintNpp const *);

  private:
    std::wstring options_file_;
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
