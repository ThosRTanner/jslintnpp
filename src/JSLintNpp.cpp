//This file is part of JSLint Plugin for Notepad++
//Copyright (C) 2010 Martin Vladic <martin.vladic@gmail.com>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "StdHeaders.h"

#include "JSLintNpp.h"

#include "AboutDlg.h"
#include "DownloadJSLint.h"
#include "JSLint.h"
#include "JSLintOptions.h"
#include "OutputDlg.h"
#include "Settings.h"
#include "Util.h"



#include "Plugin/Callback_Context.h"

#include "menuCmdID.h"

#include <memory>

typedef Callback_Context_Base<JSLintNpp> Callbacks;

#define CALLBACK_ENTRY(N)                                                     \
    {                                                                         \
        JSLintNpp::N,                                                       \
            std::make_shared<Callback_Context<JSLintNpp, JSLintNpp::N>>() \
    }

template <>
Callbacks::Contexts Callbacks::contexts = {
#pragma warning(push)
#pragma warning(disable : 26426)
    CALLBACK_ENTRY(FUNC_INDEX_JSLINT_CURRENT_FILE),
    CALLBACK_ENTRY(FUNC_INDEX_JSLINT_ALL_FILES),
    CALLBACK_ENTRY(FUNC_INDEX_SEP_2),
    CALLBACK_ENTRY(FUNC_INDEX_GOTO_PREV_LINT),
    CALLBACK_ENTRY(FUNC_INDEX_GOTO_NEXT_LINT),
    CALLBACK_ENTRY(FUNC_INDEX_SHOW_LINTS),
    CALLBACK_ENTRY(FUNC_INDEX_SEP_6),
    CALLBACK_ENTRY(FUNC_INDEX_JSLINT_OPTIONS),
    CALLBACK_ENTRY(FUNC_INDEX_SETTINGS),
    CALLBACK_ENTRY(FUNC_INDEX_SEP_9),
    CALLBACK_ENTRY(FUNC_INDEX_ABOUT),
#pragma warning(pop)
};

JSLintNpp::JSLintNpp(NppData const& data) :
    Plugin(data),
    config_dir_(get_config_dir()),
    config_file_name_(get_config_file_name()),
    options_(std::make_unique<JSLintOptions>(config_file_name_)),
    settings_(std::make_unique<Settings>(this)),
    downloader_(std::make_unique<DownloadJSLint>(this))
{
    //It's not clear to me why some of this isn't done in the constructors
    //although worth noting that some dodgy stuff goes on with settings and options.
    settings_->ReadOptions();
    options_->ReadOptions();
    downloader_->LoadVersions();
}

JSLintNpp::~JSLintNpp()
{
    settings_->SaveOptions();
    options_->SaveOptions();
}

#define MAKE_CALLBACK(entry, text, method, shortcut) \
    make_callback(entry, text, Callbacks::contexts, this, &JSLintNpp::method, false, shortcut)

#define MAKE_SEPARATOR(entry) \
    make_callback(entry, TEXT("---"), Callbacks::contexts, this, nullptr)

std::vector<FuncItem>& JSLintNpp::on_get_menu_entries()
{
    static ShortcutKey f5 = { true, false, true, VK_F5 };
    static ShortcutKey f6 = { true, false, true, VK_F6 };
    static ShortcutKey f7 = { true, false, true, VK_F7 };
    static ShortcutKey f8 = { true, false, true, VK_F8 };

    static std::vector<FuncItem> res = {
        MAKE_CALLBACK(FUNC_INDEX_JSLINT_CURRENT_FILE, TEXT("JSLint Current File"), jsLintCurrentFile, &f5),
        MAKE_CALLBACK(FUNC_INDEX_JSLINT_ALL_FILES, TEXT("JSLint All Files"), jsLintAllFiles, &f6),
        MAKE_SEPARATOR(FUNC_INDEX_SEP_2),
        MAKE_CALLBACK(FUNC_INDEX_GOTO_PREV_LINT, TEXT("Go To Previous Lint"), gotoPrevLint, &f7),
        MAKE_CALLBACK(FUNC_INDEX_GOTO_NEXT_LINT, TEXT("Go To Next Lint"), gotoNextLint, &f8),
        MAKE_CALLBACK(FUNC_INDEX_SHOW_LINTS, TEXT("Show Lints"), showLints, nullptr),
        MAKE_SEPARATOR(FUNC_INDEX_SEP_6),
        MAKE_CALLBACK(FUNC_INDEX_JSLINT_OPTIONS, TEXT("JSLint Options"), showJSLintOptionsDlg, nullptr),
        MAKE_CALLBACK(FUNC_INDEX_SETTINGS, TEXT("Settings"), showSettingsDlg, nullptr),
        MAKE_SEPARATOR(FUNC_INDEX_SEP_9),
        MAKE_CALLBACK(FUNC_INDEX_ABOUT, TEXT("About"), showAboutDlg, nullptr)
    };
    return res;
}

//
// Plugin command functions
//

void JSLintNpp::jsLintCurrentFile()
{
    createOutputWindow();

    DoEvents();

    int type;
    send_to_notepad(NPPM_GETCURRENTLANGTYPE, 0, &type);
    if (options_->GetSelectedLinter() == LINTER_JSLINT) {
        if (type != L_JS && type != L_JAVASCRIPT && type != L_HTML && type != L_CSS) {
            ::MessageBox(
                get_notepad_window(),
                TEXT("JSLint can operate only on JavaScript, HTML or CSS files."),
                TEXT("JSLint"),
                MB_OK | MB_ICONINFORMATION
            );
            return;
        }
    }
    else {
        if (type != L_JS && type != L_JAVASCRIPT) {
            ::MessageBox(
                get_notepad_window(),
                TEXT("JSHint can operate only on JavaScript files."),
                TEXT("JSLint"),
                MB_OK | MB_ICONINFORMATION
            );
            return;
        }
    }

    // set hourglass cursor
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    output_dialogue_->ClearAllLints();

    doJSLint();

    showLints();
    gotoNextLint();

    // restore normal cursor:
    POINT pt;
    GetCursorPos(&pt);
    SetCursorPos(pt.x, pt.y);
}

void JSLintNpp::jsLintAllFiles()
{
    // set hourglass cursor
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    int numJSFiles = 0;

    int numOpenFiles = send_to_notepad(NPPM_GETNBOPENFILES, 0, PRIMARY_VIEW);
    if (numOpenFiles > 0) {
        createOutputWindow();

        DoEvents();

        output_dialogue_->ClearAllLints();

        for (int i = 0; i < numOpenFiles; ++i) {
            send_to_notepad(NPPM_ACTIVATEDOC, 0, i);

            int type;
            send_to_notepad(NPPM_GETCURRENTLANGTYPE, 0, &type);
            if (type == L_JS || (options_->GetSelectedLinter() == LINTER_JSLINT && (type == L_HTML || type == L_CSS))) {
                ++numJSFiles;
                doJSLint();
            }
        }

        showLints();
        gotoNextLint();
    }

    // restore normal cursor:
    POINT pt;
    GetCursorPos(&pt);
    SetCursorPos(pt.x, pt.y);

    if (numJSFiles == 0) {
        if (options_->GetSelectedLinter() == LINTER_JSLINT) {
            ::MessageBox(
                get_notepad_window(),
                TEXT("There is no JavaScript, HTML or CSS file opened in Notepad++!"),
                TEXT("JSLint"),
                MB_OK | MB_ICONINFORMATION
            );
        }
        else {
            ::MessageBox(
                get_notepad_window(),
                TEXT("There is no JavaScript file opened in Notepad++!"),
                TEXT("JSLint"),
                MB_OK | MB_ICONINFORMATION
            );
        }
        return;
    }
}

inline void JSLintNpp::gotoNextLint()
{
    if (output_dialogue_)
    {
        output_dialogue_->SelectNextLint();
    }
}

void JSLintNpp::gotoPrevLint()
{
    if (output_dialogue_)
    {
        output_dialogue_->SelectPrevLint();
    }
}

void JSLintNpp::showLints()
{
    createOutputWindow();
    output_dialogue_->display();
}

void JSLintNpp::showJSLintOptionsDlg()
{
    options_->ShowDialog(this);
}

void JSLintNpp::showSettingsDlg()
{
    settings_->ShowDialog();
}

void JSLintNpp::showAboutDlg()
{
    pluginDialogBox(IDD_ABOUT, AboutDlgProc, this);
}

void JSLintNpp::createOutputWindow()
{
    if (!output_dialogue_)
    {
        output_dialogue_ = std::make_unique<OutputDlg>(FUNC_INDEX_SHOW_LINTS, this);
    }
}

void JSLintNpp::doJSLint()
{
    // get current file path and document index
    TCHAR filePath[MAX_PATH];
    send_to_notepad(NPPM_GETFULLCURRENTPATH, 0, filePath);

    // get all the text from the scintilla window
    Sci_TextRange tr;

    tr.chrg.cpMin = 0;
    tr.chrg.cpMax = -1;

    int length = (int)send_to_editor(SCI_GETLENGTH);
    tr.lpstrText = new char[length + 1];
    tr.lpstrText[0] = 0;

    send_to_editor(SCI_GETTEXTRANGE, 0, &tr);
    std::string strScript = tr.lpstrText;
    delete tr.lpstrText;

    // get code page of the text
    int nSciCodePage = send_to_editor(SCI_GETCODEPAGE);
    if (nSciCodePage != SC_CP_UTF8) {
        strScript = TextConversion::A_To_UTF8(strScript); // convert to UTF-8
    }

    try {
        JSLint jsLint(this);

        std::string strOptions = TextConversion::T_To_UTF8(
            options_->GetOptionsJSONString());
        std::list<JSLintReportItem> lints;

        int nppTabWidth = (int)send_to_editor(SCI_GETTABWIDTH);
        int jsLintTabWidth = options_->GetTabWidth();

        jsLint.CheckScript(strOptions, strScript, nppTabWidth, jsLintTabWidth, lints);

        output_dialogue_->AddLints(filePath, lints);

        DoEvents();
    }
    catch (std::exception& e) {
        ::MessageBox(
            get_notepad_window(),
            TextConversion::A_To_T(e.what()).c_str(),
            TEXT("JSLint"),
            MB_OK | MB_ICONERROR
        );
    }
}

template <>
INT_PTR JSLintNpp::pluginDialogBox(UINT idDlg, DLGPROC lpDlgProc, void const *self) const
{
    HWND hWndFocus = ::GetFocus();
    INT_PTR nRet = ::DialogBoxParam(module(), MAKEINTRESOURCE(idDlg),
        get_notepad_window(), lpDlgProc, reinterpret_cast<LPARAM>(self));
    ::SetFocus(hWndFocus);
    return nRet;
}

std::wstring JSLintNpp::get_config_dir() const
{
    TCHAR szConfigDir[MAX_PATH];
    szConfigDir[0] = 0;
    send_to_notepad(NPPM_GETPLUGINSCONFIGDIR, sizeof(szConfigDir), szConfigDir);
    return szConfigDir;
}

std::wstring JSLintNpp::get_config_file_name() const
{
    return Path::GetFullPath(TEXT("JSLint.ini"), config_dir_);
}
