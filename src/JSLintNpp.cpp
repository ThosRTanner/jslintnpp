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

#include "JSLintNpp.h"

#include "AboutDlg.h"
#include "DownloadJSLint.h"
#include "JSLint.h"
#include "JSLintOptions.h"
#include "Linter.h"
#include "OutputDlg.h"
#include "Profile_Handler.h"
#include "Settings.h"
#include "Util.h"
#include "Version_Info.h"

#include "Plugin/Callback_Context.h"

#include <memory>

DEFINE_PLUGIN_MENU_CALLBACKS(JSLintNpp);

JSLintNpp::JSLintNpp(NppData const &data) :
    Plugin(data, get_plugin_name()),
    config_dir_(get_config_dir()),
    config_file_name_(get_config_file_name()),
    profile_handler_(std::make_unique<Profile_Handler>(config_file_name_)),
    options_(std::make_unique<JSLintOptions>(profile_handler_.get())),
    downloader_(std::make_unique<DownloadJSLint>(this)),
    settings_(std::make_unique<Settings>(
        this, profile_handler_.get(), downloader_.get()
    ))
{
    // It's not clear to me why some of this isn't done in the constructors
    // although worth noting that some dodgy stuff goes on with settings and
    // options.
    options_->ReadOptions();
}

JSLintNpp::~JSLintNpp()
{
    options_->SaveOptions();
}

wchar_t const *JSLintNpp::get_plugin_name() noexcept
{
    return L"JSLint";
}

#define MAKE_CALLBACK(entry, text, method, shortcut) \
    PLUGIN_MENU_MAKE_CALLBACK(JSLintNpp, entry, text, method, false, shortcut)

#define MAKE_SEPARATOR(entry) PLUGIN_MENU_MAKE_SEPARATOR(JSLintNpp, entry)

std::vector<FuncItem> &JSLintNpp::on_get_menu_entries()
{
    static ShortcutKey f5 = {true, false, true, VK_F5};
    static ShortcutKey f6 = {true, false, true, VK_F6};
    static ShortcutKey f7 = {true, false, true, VK_F7};
    static ShortcutKey f8 = {true, false, true, VK_F8};

    static std::vector<FuncItem> res = {
        MAKE_CALLBACK(
            FUNC_INDEX_JSLINT_CURRENT_FILE,
            L"JSLint Current File",
            jsLintCurrentFile,
            &f5
        ),
        MAKE_CALLBACK(
            FUNC_INDEX_JSLINT_ALL_FILES,
            L"JSLint All Files",
            jsLintAllFiles,
            &f6
        ),
        MAKE_SEPARATOR(FUNC_INDEX_SEP_2),
        MAKE_CALLBACK(
            FUNC_INDEX_GOTO_PREV_LINT, L"Go To Previous Lint", gotoPrevLint, &f7
        ),
        MAKE_CALLBACK(
            FUNC_INDEX_GOTO_NEXT_LINT, L"Go To Next Lint", gotoNextLint, &f8
        ),
        MAKE_CALLBACK(FUNC_INDEX_SHOW_LINTS, L"Show Lints", showLints, nullptr),
        MAKE_SEPARATOR(FUNC_INDEX_SEP_6),
        MAKE_CALLBACK(
            FUNC_INDEX_JSLINT_OPTIONS,
            L"JSLint Options",
            showJSLintOptionsDlg,
            nullptr
        ),
        MAKE_CALLBACK(
            FUNC_INDEX_SETTINGS, L"Settings", showSettingsDlg, nullptr
        ),
        MAKE_SEPARATOR(FUNC_INDEX_SEP_9),
        MAKE_CALLBACK(FUNC_INDEX_ABOUT, L"About", showAboutDlg, nullptr)
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
    if (options_->GetSelectedLinter() == Linter::LINTER_JSLINT)
    {
        if (type != L_JS && type != L_JAVASCRIPT && type != L_HTML
            && type != L_CSS)
        {
            message_box(
                L"JSLint can operate only on JavaScript, HTML or CSS files.",
                MB_OK | MB_ICONINFORMATION
            );
            return;
        }
    }
    else
    {
        if (type != L_JS && type != L_JAVASCRIPT)
        {
            message_box(
                L"JSHint can operate only on JavaScript files.",
                MB_OK | MB_ICONINFORMATION
            );
            return;
        }
    }

    // set hourglass cursor
    SetCursor(LoadCursor(nullptr, IDC_WAIT));

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
    SetCursor(LoadCursor(nullptr, IDC_WAIT));

    int numJSFiles = 0;

    auto numOpenFiles = send_to_notepad(NPPM_GETNBOPENFILES, 0, PRIMARY_VIEW);
    if (numOpenFiles > 0)
    {
        createOutputWindow();

        DoEvents();

        output_dialogue_->ClearAllLints();

        for (int i = 0; i < numOpenFiles; ++i)
        {
            send_to_notepad(NPPM_ACTIVATEDOC, 0, i);

            int type;
            send_to_notepad(NPPM_GETCURRENTLANGTYPE, 0, &type);
            if (type == L_JS || type == L_JAVASCRIPT
                || (options_->GetSelectedLinter() == Linter::LINTER_JSLINT
                    && (type == L_HTML || type == L_CSS)))
            {
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

    if (numJSFiles == 0)
    {
        if (options_->GetSelectedLinter() == Linter::LINTER_JSLINT)
        {
            message_box(
                (L"There is no JavaScript, HTML or CSS file opened in "
                 L"Notepad++!"),
                MB_OK | MB_ICONINFORMATION
            );
        }
        else
        {
            message_box(
                L"There is no JavaScript file opened in Notepad++!",
                MB_OK | MB_ICONINFORMATION
            );
        }
        return;
    }
}

void JSLintNpp::gotoNextLint()
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
    AboutDlg dlg(this);
}

void JSLintNpp::createOutputWindow()
{
    if (! output_dialogue_)
    {
        output_dialogue_ = std::make_unique<OutputDlg>(
            FUNC_INDEX_SHOW_LINTS, this, options_.get()
        );
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
    auto nSciCodePage = send_to_editor(SCI_GETCODEPAGE);
    if (nSciCodePage != SC_CP_UTF8)
    {
        strScript = TextConversion::A_To_UTF8(strScript);    // convert to UTF-8
    }

    try
    {
        JSLint jsLint(this);

        std::string strOptions =
            TextConversion::T_To_UTF8(options_->GetOptionsJSONString());
        std::list<JSLintReportItem> lints;

        int nppTabWidth = (int)send_to_editor(SCI_GETTABWIDTH);
        int jsLintTabWidth = options_->GetTabWidth();

        jsLint.CheckScript(
            strOptions, strScript, nppTabWidth, jsLintTabWidth, lints
        );

        output_dialogue_->AddLints(filePath, lints);

        DoEvents();
    }
    catch (std::exception &e)
    {
        message_box(
            TextConversion::A_To_T(e.what()).c_str(), MB_OK | MB_ICONERROR
        );
    }
}

template <>
INT_PTR JSLintNpp::pluginDialogBox(
    UINT idDlg, DLGPROC lpDlgProc, void const *self
) const
{
    HWND hWndFocus = ::GetFocus();
    INT_PTR nRet = ::DialogBoxParam(
        module(),
        MAKEINTRESOURCE(idDlg),
        get_notepad_window(),
        lpDlgProc,
        reinterpret_cast<LPARAM>(self)
    );
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
    return Path::GetFullPath(L"JSLint.ini", config_dir_);
}
