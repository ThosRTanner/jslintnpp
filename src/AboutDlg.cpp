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

#include "AboutDlg.h"

#include "Version.h"

#include "Plugin/Plugin.h"

#include "resource.h"

#include <CommCtrl.h>
#include <WinUser.h>
#include <basetsd.h>
#include <shellapi.h>
#include <tchar.h>

#include <optional>

AboutDlg::AboutDlg(Plugin const *plugin) : Modal_Dialogue_Interface(plugin)
{
    create_modal_dialogue(IDD_ABOUT);
}

AboutDlg::~AboutDlg() = default;

std::optional<LONG_PTR> AboutDlg::on_dialogue_message(
    UINT message, WPARAM wParam, LPARAM lParam
)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            HWND hWndVersionStatic = GetDlgItem(IDC_VERSION_STATIC);

            TCHAR szVersionFormat[50];
            ::GetWindowText(
                hWndVersionStatic, &szVersionFormat[0], _countof(szVersionFormat)
            );

            TCHAR szVersion[100];
            _stprintf(
                &szVersion[0],
                &szVersionFormat[0],
                L"Unicode",
                MY_PRODUCT_VERSION
            );

            ::SetWindowText(hWndVersionStatic, &szVersion[0]);

            centre_dialogue();
        }
        break;

        case WM_NOTIFY:
            switch (reinterpret_cast<LPNMHDR>(lParam)->code)
            {
                case NM_CLICK:
                case NM_RETURN:
                {
                    auto const pNMLink = reinterpret_cast<NMLINK const *>(lParam);
                    LITEM const item = pNMLink->item;
                    ShellExecute(
                        nullptr, L"open", &item.szUrl[0], nullptr, nullptr, SW_SHOW
                    );
                    break;
                }

                default:
                    break;
            }
            break;

        default:
            break;
    }

    return std::nullopt;
}
