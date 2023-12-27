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

#include "Plugin/Plugin.h"

#include <memory>
#include <string>

class DownloadJSLint;
class JSLintOptions;
class OutputDlg;
class Settings;

class JSLintNpp : public Plugin
{
  public:
    JSLintNpp(NppData const &);

    ~JSLintNpp();

    JSLintNpp(JSLintNpp const &) = delete;
    JSLintNpp(JSLintNpp &&) = delete;
    JSLintNpp &operator=(JSLintNpp const &) = delete;
    JSLintNpp &operator=(JSLintNpp &&) = delete;

    static wchar_t const *get_plugin_name() noexcept;

    enum Menu_Entries
    {
        FUNC_INDEX_JSLINT_CURRENT_FILE,
        FUNC_INDEX_JSLINT_ALL_FILES,
        FUNC_INDEX_SEP_2,
        FUNC_INDEX_GOTO_PREV_LINT,
        FUNC_INDEX_GOTO_NEXT_LINT,
        FUNC_INDEX_SHOW_LINTS,
        FUNC_INDEX_SEP_6,
        FUNC_INDEX_JSLINT_OPTIONS,
        FUNC_INDEX_SETTINGS,
        FUNC_INDEX_SEP_9,
        FUNC_INDEX_ABOUT
    };

    auto GetConfigFileName() const
    {
        return config_file_name_;
    }

    auto GetConfigDir() const
    {
        return config_dir_;
    }

    auto get_downloader() const
    {
        return downloader_.get();
    }

    auto get_options() const
    {
        return options_.get();
    }

    auto get_settings() const
    {
        return settings_.get();
    }

  private:
    std::vector<FuncItem> &on_get_menu_entries() override;

    //
    // Plugin command functions
    //
    void jsLintCurrentFile();
    void jsLintAllFiles();
    void gotoNextLint();
    void gotoPrevLint();
    void showLints();
    void showJSLintOptionsDlg();
    void showSettingsDlg();
    void showAboutDlg();

#if 0
	void on_notification(SCNotification const*) noexcept override;

	LRESULT on_message(UINT, WPARAM, LPARAM) noexcept override;
#endif

    void doJSLint();

    void createOutputWindow();

  public:
    template <class T>
    INT_PTR pluginDialogBox(UINT idDlg, DLGPROC lpDlgProc, T *self) const
    {
        return pluginDialogBox(
            idDlg, lpDlgProc, static_cast<void const *>(self)
        );
    }

    template <>
    INT_PTR pluginDialogBox(UINT idDlg, DLGPROC lpDlgProc, void const *) const;

  private:
    std::wstring get_config_file_name() const;

    std::wstring get_config_dir() const;

  private:
    std::wstring config_dir_;
    std::wstring config_file_name_;
    std::unique_ptr<JSLintOptions> options_;
    std::unique_ptr<Settings> settings_;
    std::unique_ptr<DownloadJSLint> downloader_;
    std::unique_ptr<OutputDlg> output_dialogue_;
};
