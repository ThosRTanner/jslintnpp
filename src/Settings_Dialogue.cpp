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

#include "Settings_Dialogue.h"

#include "DownloadJSLint.h"
#include "JSLintNpp.h"
#include "Linter.h"
#include "Settings.h"
#include "Util.h"            //Do NOT remove
#include "Version_Info.h"    //Do NOT remove

#include "resource.h"

#include <windowsx.h>

#include <string>

Settings_Dialogue::Settings_Dialogue(
    JSLintNpp const *plugin, Settings *settings, DownloadJSLint *downloader
) :
    Modal_Dialogue_Interface(plugin),
    plugin_(plugin),
    settings_(std::make_unique<Settings>(*settings)),
    downloader_(downloader)
{
    create_modal_dialogue(IDD_SETTINGS);
}

Settings_Dialogue::~Settings_Dialogue() = default;

std::optional<LONG_PTR> Settings_Dialogue::on_dialogue_message(
    UINT message, WPARAM wParam, LPARAM lParam
)
{
    switch (message)
    {
        case WM_INITDIALOG:
            LoadVersions(IDC_JSLINT_SCRIPT_VERSION, Linter::LINTER_JSLINT);
            LoadVersions(IDC_JSHINT_SCRIPT_VERSION, Linter::LINTER_JSHINT);
            display_options();
            UpdateControls();
            centre_dialogue();
            break;

        case WM_COMMAND:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                std::wstring latestVersion;

                switch (LOWORD(wParam))
                {
                    case IDC_RADIO1:
                    case IDC_RADIO2:
                    case IDC_JSLINT_SPEC_UNDEF_VAR_ERR_MSG:
                    case IDC_RADIO3:
                    case IDC_RADIO4:
                    case IDC_JSHINT_SPEC_UNDEF_VAR_ERR_MSG:
                        UpdateControls();
                        break;

                    case IDC_JSLINT_DOWNLOAD_LATEST:
                        switch (downloader_->DownloadLatest(
                            Linter::LINTER_JSLINT, latestVersion
                        ))
                        {
                            case DownloadJSLint::DOWNLOAD_OK:
                                LoadVersions(
                                    IDC_JSLINT_SCRIPT_VERSION,
                                    Linter::LINTER_JSLINT
                                );
                                ComboBox_SelectString(
                                    GetDlgItem(IDC_JSLINT_SCRIPT_VERSION),
                                    -1,
                                    latestVersion.c_str()
                                );
                                break;

                            case DownloadJSLint::DOWNLOAD_NO_NEW_VERSION:
                                message_box(
                                    L"You already have the latest version!",
                                    MB_OK | MB_ICONEXCLAMATION
                                );
                                break;

                            case DownloadJSLint::DOWNLOAD_FAILED:
                                message_box(
                                    L"Download error!", MB_OK | MB_ICONERROR
                                );
                                break;
                        }
                        break;

                    case IDC_JSHINT_DOWNLOAD_LATEST:
                        switch (downloader_->DownloadLatest(
                            Linter::LINTER_JSHINT, latestVersion
                        ))
                        {
                            case DownloadJSLint::DOWNLOAD_OK:
                                LoadVersions(
                                    IDC_JSHINT_SCRIPT_VERSION,
                                    Linter::LINTER_JSHINT
                                );
                                ComboBox_SelectString(
                                    GetDlgItem(IDC_JSHINT_SCRIPT_VERSION),
                                    -1,
                                    latestVersion.c_str()
                                );
                                break;

                            case DownloadJSLint::DOWNLOAD_NO_NEW_VERSION:
                                message_box(
                                    L"You already have the latest version!",
                                    MB_OK | MB_ICONEXCLAMATION
                                );
                                break;

                            case DownloadJSLint::DOWNLOAD_FAILED:
                                message_box(
                                    L"Download error!", MB_OK | MB_ICONERROR
                                );
                                break;
                        }
                        break;

                    case IDOK:
                        if (update_options())
                        {
                            EndDialog(settings_.get());
                        }
                        return 1;

                    default:
                        break;
                }
            }
        }
        break;
    }
    return std::nullopt;
}

void Settings_Dialogue::LoadVersions(int versionsComboBoxID, Linter linter)
{
    ComboBox_ResetContent(GetDlgItem(versionsComboBoxID));

    for (auto const &version : plugin_->downloader()->GetVersions(linter))
    {
        ComboBox_AddString(
            GetDlgItem(versionsComboBoxID), version.first.c_str()
        );
    }
}

void Settings_Dialogue::display_options() const noexcept
{
    if (settings_->m_jsLintScript.m_scriptSource == SCRIPT_SOURCE_BUILTIN)
    {
        CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
    }
    else
    {
        CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
    }

    ComboBox_SelectString(
        GetDlgItem(IDC_JSLINT_SCRIPT_VERSION),
        -1,
        settings_->m_jsLintScript.m_scriptVersion.c_str()
    );

    if (settings_->m_jsLintScript.m_bSpecUndefVarErrMsg)
    {
        CheckDlgButton(IDC_JSLINT_SPEC_UNDEF_VAR_ERR_MSG, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(IDC_JSLINT_SPEC_UNDEF_VAR_ERR_MSG, BST_UNCHECKED);
    }

    SetWindowText(
        GetDlgItem(IDC_JSLINT_UNDEF_VAR_ERR_MSG),
        settings_->m_jsLintScript.m_undefVarErrMsg.c_str()
    );

    // Now we do exactly the same thing for the jshint. yick.
    if (settings_->m_jsHintScript.m_scriptSource == SCRIPT_SOURCE_BUILTIN)
    {
        CheckRadioButton(IDC_RADIO3, IDC_RADIO4, IDC_RADIO3);
    }
    else
    {
        CheckRadioButton(IDC_RADIO3, IDC_RADIO4, IDC_RADIO4);
    }

    ComboBox_SelectString(
        GetDlgItem(IDC_JSHINT_SCRIPT_VERSION),
        -1,
        settings_->m_jsHintScript.m_scriptVersion.c_str()
    );

    if (settings_->m_jsHintScript.m_bSpecUndefVarErrMsg)
    {
        CheckDlgButton(IDC_JSHINT_SPEC_UNDEF_VAR_ERR_MSG, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(IDC_JSHINT_SPEC_UNDEF_VAR_ERR_MSG, BST_UNCHECKED);
    }

    SetWindowText(
        GetDlgItem(IDC_JSHINT_UNDEF_VAR_ERR_MSG),
        settings_->m_jsHintScript.m_undefVarErrMsg.c_str()
    );
}

BOOL Settings_Dialogue::update_options() noexcept
{
    if (IsDlgButtonChecked(IDC_RADIO1))
    {
        settings_->m_jsLintScript.m_scriptSource = SCRIPT_SOURCE_BUILTIN;
    }
    else
    {
        settings_->m_jsLintScript.m_scriptSource = SCRIPT_SOURCE_DOWNLOADED;

        int nCurSel = ComboBox_GetCurSel(GetDlgItem(IDC_JSLINT_SCRIPT_VERSION));
        if (nCurSel == CB_ERR)
        {
            MessageBox(
                window(),
                L"Please select JSLint script version!",
                L"JSLint",
                MB_OK | MB_ICONERROR
            );
            SetFocus(IDC_JSLINT_SCRIPT_VERSION);
            return FALSE;
        }
        settings_->m_jsLintScript.m_scriptVersion =
            get_window_text(IDC_JSLINT_SCRIPT_VERSION);

        if (IsDlgButtonChecked(IDC_JSLINT_SPEC_UNDEF_VAR_ERR_MSG))
        {
            settings_->m_jsLintScript.m_bSpecUndefVarErrMsg = true;

            std::wstring undefVarErrMsg =
                get_window_text(IDC_JSLINT_UNDEF_VAR_ERR_MSG);
            if (undefVarErrMsg.empty())
            {
                message_box(
                    L"Please enter 'undefined variable' error message text!",
                    MB_OK | MB_ICONERROR
                );
                SetFocus(IDC_JSLINT_UNDEF_VAR_ERR_MSG);
                return FALSE;
            }
            settings_->m_jsLintScript.m_undefVarErrMsg = undefVarErrMsg;
        }
        else
        {
            settings_->m_jsLintScript.m_bSpecUndefVarErrMsg = false;
        }
    }

    // Sigh, repeat for 2nd set.
    if (IsDlgButtonChecked(IDC_RADIO3))
    {
        settings_->m_jsHintScript.m_scriptSource = SCRIPT_SOURCE_BUILTIN;
    }
    else
    {
        settings_->m_jsHintScript.m_scriptSource = SCRIPT_SOURCE_DOWNLOADED;

        int nCurSel = ComboBox_GetCurSel(GetDlgItem(IDC_JSHINT_SCRIPT_VERSION));
        if (nCurSel == CB_ERR)
        {
            message_box(
                L"Please select JSHint script version!", MB_OK | MB_ICONERROR
            );
            SetFocus(IDC_JSHINT_SCRIPT_VERSION);
            return FALSE;
        }
        settings_->m_jsHintScript.m_scriptVersion =
            get_window_text(IDC_JSHINT_SCRIPT_VERSION);

        if (IsDlgButtonChecked(IDC_JSHINT_SPEC_UNDEF_VAR_ERR_MSG))
        {
            settings_->m_jsHintScript.m_bSpecUndefVarErrMsg = true;

            std::wstring undefVarErrMsg =
                get_window_text(IDC_JSHINT_UNDEF_VAR_ERR_MSG);
            if (undefVarErrMsg.empty())
            {
                message_box(
                    L"Please enter 'undefined variable' error message text!",
                    MB_OK | MB_ICONERROR
                );
                SetFocus(IDC_JSHINT_UNDEF_VAR_ERR_MSG);
                return FALSE;
            }
            settings_->m_jsHintScript.m_undefVarErrMsg = undefVarErrMsg;
        }
        else
        {
            settings_->m_jsHintScript.m_bSpecUndefVarErrMsg = false;
        }
    }

    return TRUE;
}

void Settings_Dialogue::UpdateControls()
{
    BOOL bDownload;

    bDownload = IsDlgButtonChecked(IDC_RADIO2);
    EnableWindow(GetDlgItem(IDC_JSLINT_SCRIPT_VERSION_LABEL), bDownload);
    EnableWindow(GetDlgItem(IDC_JSLINT_SCRIPT_VERSION), bDownload);
    EnableWindow(GetDlgItem(IDC_JSLINT_DOWNLOAD_LATEST), bDownload);
    EnableWindow(GetDlgItem(IDC_JSLINT_SPEC_UNDEF_VAR_ERR_MSG), bDownload);
    EnableWindow(
        GetDlgItem(IDC_JSLINT_UNDEF_VAR_ERR_MSG),
        bDownload && IsDlgButtonChecked(IDC_JSLINT_SPEC_UNDEF_VAR_ERR_MSG)
    );

    bDownload = IsDlgButtonChecked(IDC_RADIO4);
    EnableWindow(GetDlgItem(IDC_JSHINT_SCRIPT_VERSION_LABEL), bDownload);
    EnableWindow(GetDlgItem(IDC_JSHINT_SCRIPT_VERSION), bDownload);
    EnableWindow(GetDlgItem(IDC_JSHINT_DOWNLOAD_LATEST), bDownload);
    EnableWindow(GetDlgItem(IDC_JSHINT_SPEC_UNDEF_VAR_ERR_MSG), bDownload);
    EnableWindow(
        GetDlgItem(IDC_JSHINT_UNDEF_VAR_ERR_MSG),
        bDownload && IsDlgButtonChecked(IDC_JSHINT_SPEC_UNDEF_VAR_ERR_MSG)
    );
}

UINT Settings_Dialogue::IsDlgButtonChecked(int button) const noexcept
{
    return ::IsDlgButtonChecked(window(), button);
}

BOOL Settings_Dialogue::CheckDlgButton(int button, UINT check) const noexcept
{
    return ::CheckDlgButton(window(), button, check);
}

BOOL Settings_Dialogue::CheckRadioButton(
    int first_button, int last_button, int check_button
) const noexcept
{
    return ::CheckRadioButton(
        window(), first_button, last_button, check_button
    );
}
