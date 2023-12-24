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


#pragma once

//#include "PluginInterface.h"

#include "Plugin/Plugin.h"

#include <memory>
#include <string>

class JSLintOptions;

class JSLintNpp : public Plugin
{
public:
	JSLintNpp(NppData const&);

	~JSLintNpp();

	JSLintNpp(JSLintNpp const&) = delete;
	JSLintNpp(JSLintNpp&&) = delete;
	JSLintNpp& operator=(JSLintNpp const&) = delete;
	JSLintNpp& operator=(JSLintNpp&&) = delete;

	static TCHAR const* get_plugin_name() noexcept
	{
		return TEXT("JSLint");
	}

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

	std::wstring GetConfigFileName() const
	{
		return config_file_name_;
	}

private:
	std::vector<FuncItem>& on_get_menu_entries() override;

#if 0
	void on_notification(SCNotification const*) noexcept override;

	LRESULT on_message(UINT, WPARAM, LPARAM) noexcept override;
#endif

	void doJSLint();

	void createOutputWindow();

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

public:
	INT_PTR pluginDialogBox(UINT idDlg, DLGPROC lpDlgProc) const;
private:

	std::wstring get_config_file_name() const;

	std::wstring config_file_name_;
	std::unique_ptr<JSLintOptions> options_;
};
