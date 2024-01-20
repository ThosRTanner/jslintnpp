// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "StdHeaders.h"

#include "Options_Sub_Dialogue.h"

#include "Linter_Options.h"
#include "Options_Dialogue.h"

#include "resource.h"

#include <windowsx.h>
#include <WinUser.h>

Options_Sub_Dialogue::Options_Sub_Dialogue(
    int dialogue_id, Plugin const *plugin, Options_Dialogue const *parent,
    Linter_Options *options
) :
    Non_Modal_Dialogue_Interface(dialogue_id, plugin, parent->window()),
    parent_(parent),
    options_(options)
{
    //Overlay the parent window.
    HWND hWndOptionsPlaceholder = ::GetDlgItem(parent_->window(), IDC_OPTIONS_PLACEHOLDER);
    RECT rect;
    GetWindowRect(hWndOptionsPlaceholder, &rect);
    POINT ptTopLeft;
    ptTopLeft.x = rect.left;
    ptTopLeft.y = rect.top;
    ScreenToClient(parent_->window(), &ptTopLeft);
    SetWindowPos(
        window(),
        hWndOptionsPlaceholder,
        ptTopLeft.x,
        ptTopLeft.y,
        rect.right - rect.left,
        rect.bottom - rect.top,
        0
    );
}

Options_Sub_Dialogue::~Options_Sub_Dialogue()
{
}

void Options_Sub_Dialogue::show()
{
    // FIXME Populate contents in a less crap way than this.
    options_->UpdateOptions(parent_->window(), window(), false, false);
    ::ShowWindow(window(), SW_SHOW);
}

void Options_Sub_Dialogue::hide()
{
    ::ShowWindow(window(), SW_HIDE);
}

std::optional<LONG_PTR> Options_Sub_Dialogue::on_dialogue_message(
    UINT message, WPARAM wParam, LPARAM lParam
) noexcept
{
    if (message == WM_COMMAND)
    {
        if (HIWORD(wParam) == BN_CLICKED)
        {
            UINT id = LOWORD(wParam);
            if (IDC_CHECK_FIRST_OPTION <= id && id <= IDC_CHECK_LAST_OPTION)
            {
                auto const item = GetDlgItem(id);
                switch (Button_GetCheck(item))
                {
                    case BST_UNCHECKED:
                        Button_SetCheck(item, BST_INDETERMINATE);
                        break;

                    case BST_CHECKED:
                        Button_SetCheck(item, BST_UNCHECKED);
                        break;

                    default:
                        Button_SetCheck(item, BST_CHECKED);
                }

                // THis is deeply horrible and needs extracting into here.
                options_->UpdateOptions(
                    parent_->window(), window(), true, false
                );
            }
        }
    }

    return std::nullopt;
}
