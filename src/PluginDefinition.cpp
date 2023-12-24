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

#include "PluginDefinition.h"

#include "AboutDlg.h"
#include "JSLint.h"
#include "JSLintOptions.h"
#include "Settings.h"
#include "DownloadJSLint.h"
#include "OutputDlg.h"


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
    config_file_name_(get_config_file_name()),
    options_(std::make_unique<JSLintOptions>(config_file_name_))
{

    //FIXME. Seriously? Singletons?
    Settings::GetInstance().ReadOptions();
    options_->ReadOptions();
    DownloadJSLint::GetInstance().LoadVersions();
}

JSLintNpp::~JSLintNpp()
{
    //FIXME. Seriously? Singletons?
    Settings::GetInstance().SaveOptions();
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

    //g_outputDlg.ClearAllLints();

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

        //g_outputDlg.ClearAllLints();

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

void JSLintNpp::gotoNextLint()
{
    //g_outputDlg.SelectNextLint();
}

void JSLintNpp::gotoPrevLint()
{
    //g_outputDlg.SelectPrevLint();
}

void JSLintNpp::showLints()
{
    createOutputWindow();
    //g_outputDlg.display();
}

void JSLintNpp::showJSLintOptionsDlg()
{
    options_->ShowDialog(this);
}

void JSLintNpp::showSettingsDlg()
{
    Settings::GetInstance().ShowDialog();
}

void JSLintNpp::showAboutDlg()
{
    pluginDialogBox(IDD_ABOUT, AboutDlgProc);
}

/*
std::wstring JSLintNpp::GetConfigFileName() const 
{
    static std::wstring strConfigFileName;

    if (strConfigFileName.empty()) {
        TCHAR szConfigDir[MAX_PATH];
        szConfigDir[0] = 0;
        send_to_notepad(NPPM_GETPLUGINSCONFIGDIR, sizeof(szConfigDir), szConfigDir);
        strConfigFileName = Path::GetFullPath(TEXT("JSLint.ini"), szConfigDir);
    }

    return strConfigFileName;
}
*/

void JSLintNpp::createOutputWindow()
{
    /*
    if (!g_outputDlg.isCreated())
    {
        g_outputDlg.setParent(g_nppData._nppHandle);

        tTbData	data = { 0 };

        g_outputDlg.create(&data);

        // define the default docking behaviour
        data.uMask = DWS_DF_CONT_BOTTOM | DWS_ICONTAB;
        data.pszModuleName = g_outputDlg.getPluginFileName();
        data.hIconTab = g_outputDlg.GetTabIcon();
        data.dlgID = FUNC_INDEX_SHOW_LINTS;
        ::SendMessage(g_nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&data);
    }
    */
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
        JSLint jsLint;

        std::string strOptions = TextConversion::T_To_UTF8(
            options_->GetOptionsJSONString());
        std::list<JSLintReportItem> lints;

        int nppTabWidth = (int)send_to_editor(SCI_GETTABWIDTH);
        int jsLintTabWidth = options_->GetTabWidth();

        jsLint.CheckScript(strOptions, strScript, nppTabWidth, jsLintTabWidth, lints);

        //g_outputDlg.AddLints(filePath, lints);

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

INT_PTR JSLintNpp::pluginDialogBox(UINT idDlg, DLGPROC lpDlgProc) const
{
    HWND hWndFocus = ::GetFocus();
    INT_PTR nRet = ::DialogBoxParam(module(), MAKEINTRESOURCE(idDlg),
        get_notepad_window(), lpDlgProc, reinterpret_cast<LPARAM>(this));
    ::SetFocus(hWndFocus);
    return nRet;
}

inline std::wstring JSLintNpp::get_config_file_name() const
{
    TCHAR szConfigDir[MAX_PATH];
    szConfigDir[0] = 0;
    send_to_notepad(NPPM_GETPLUGINSCONFIGDIR, sizeof(szConfigDir), szConfigDir);
    return Path::GetFullPath(TEXT("JSLint.ini"), szConfigDir);
}
