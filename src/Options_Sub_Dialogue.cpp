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

#include "Options_Dialogue.h"

#include "resource.h"

#include <WinUser.h>
#include <windowsx.h>

Options_Sub_Dialogue::Options_Sub_Dialogue(
    int dialogue_id, Plugin *plugin, Options_Dialogue *parent,
    Linter_Options *options
) :
    Non_Modal_Dialogue_Interface(dialogue_id, plugin, parent->window()),
    parent_(parent),
    options_(options)
{
}

Options_Sub_Dialogue::~Options_Sub_Dialogue()
{
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
                //m_options.UpdateOptions(m_hDlg, m_hSubDlg, true, false);
                //Now we need to update the options dialogue with the contents of this window.

            }
        }
    }

    return std::nullopt;
}
