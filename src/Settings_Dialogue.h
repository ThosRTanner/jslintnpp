#pragma once

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

#include "Plugin/Modal_Dialogue_Interface.h"

#include <memory>

class DownloadJSLint;
class JSLintNpp;
enum class Linter;
class Settings;

class Settings_Dialogue : public Modal_Dialogue_Interface
{
  public:
    Settings_Dialogue(JSLintNpp const *, Settings *, DownloadJSLint *);

    ~Settings_Dialogue();

    Settings* get_settings() const noexcept
    {
        return settings_.get();
    }

  private:
    JSLintNpp const *plugin_;
    std::unique_ptr<Settings> settings_;
    DownloadJSLint *downloader_;

    std::optional<LONG_PTR> on_dialogue_message(
        UINT message, WPARAM wParam, LPARAM lParam
    ) override;

    void LoadVersions(int versionsComboBoxID, Linter linter);
    void display_options() const noexcept;
    BOOL update_options();

    void UpdateControls() noexcept;

    UINT IsDlgButtonChecked(int button) const noexcept;

    BOOL CheckDlgButton(int button, UINT check) const noexcept;

    BOOL CheckRadioButton(int first_button, int last_button, int check_button)
        const noexcept;
};
