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
#include "Util.h"

#include "resource.h"

#include <WinUser.h>
#include <windowsx.h>

#include <string>

namespace
{

/** Returns true if the selected option is displayed in the parent dialogue
 * rather than the linter specific dialogue
 */
constexpr bool is_in_parent(UINT item)
{
    return item == IDC_PREDEFINED || item == IDC_ADDITIONAL_OPTIONS;
}

}    // namespace

Options_Sub_Dialogue::Options_Sub_Dialogue(
    int dialogue_id, Plugin const *plugin, Options_Dialogue const *parent,
    Linter_Options *options
) :
    Non_Modal_Dialogue_Interface(dialogue_id, plugin, parent->window()),
    parent_(parent),
    options_(options)
{
    // Overlay the parent window.
    HWND hWndOptionsPlaceholder =
        ::GetDlgItem(parent_->window(), IDC_OPTIONS_PLACEHOLDER);
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
    for (auto const &[key, option] : options_->get_options())
    {
        if (is_in_parent(key))
        {
            SetWindowText(
                ::GetDlgItem(parent_->window(), key), option.value.c_str()
            );
            continue;
        }

        switch (option.type)
        {
            case OPTION_TYPE_BOOL:
            {
                int checkState;
                if (option.value == L"false")
                {
                    checkState = BST_UNCHECKED;
                }
                else if (option.value == L"true")
                {
                    checkState = BST_CHECKED;
                }
                else
                {
                    checkState = BST_INDETERMINATE;
                }
                Button_SetCheck(GetDlgItem(key), checkState);
                break;
            }

            case OPTION_TYPE_INT:
            case OPTION_TYPE_ARR_STRING:
                SetWindowText(GetDlgItem(key), option.value.c_str());
                break;
        }
    }

    update_comment_string();

    ::ShowWindow(window(), SW_SHOW);
}

void Options_Sub_Dialogue::hide()
{
    ::ShowWindow(window(), SW_HIDE);
}

bool Options_Sub_Dialogue::update(bool validate)
{
    for (auto const &option : options_->get_options())
    {
        switch (option.second.type)
        {
            case OPTION_TYPE_BOOL:
            {
                int const checkState =
                    Button_GetCheck(GetDlgItem(option.first));
                if (checkState == BST_UNCHECKED)
                {
                    options_->UncheckOption(option.first);
                }
                else if (checkState == BST_CHECKED)
                {
                    options_->CheckOption(option.first);
                }
                else
                {
                    options_->ClearOption(option.first);
                }
                break;
            }

            case OPTION_TYPE_INT:
            {
                std::wstring const value{TrimSpaces(get_window_text(option.first
                ))};
                if (validate)
                {
                    auto message = options_->check_valid(option.first, value);
                    if (message.has_value())
                    {
                        message_box(
                            message.value().c_str(), MB_OK | MB_ICONERROR
                        );
                        SetFocus(option.first);
                        return false;
                    }
                }
                options_->SetOption(option.first, value);
                break;
            }

            case OPTION_TYPE_ARR_STRING:
            {
                std::wstring const value{TrimSpaces(get_window_text(
                    option.first,
                    is_in_parent(option.first) ? parent_->window() : window()
                ))};
                options_->SetOption(option.first, value);
                break;
            }

            default:
                break;
        }
    }

    return true;
}

std::optional<LONG_PTR> Options_Sub_Dialogue::on_dialogue_message(
    UINT message, WPARAM wParam, LPARAM lParam
)
{
    if (message == WM_COMMAND)
    {
        switch (HIWORD(wParam))
        {
            case BN_CLICKED:
            {
                UINT const id = LOWORD(wParam);
                if (IDC_CHECK_FIRST_OPTION <= id && id <= IDC_CHECK_LAST_OPTION)
                {
                    auto const item = GetDlgItem(id);
                    switch (Button_GetCheck(item))
                    {
                        case BST_UNCHECKED:
                            Button_SetCheck(item, BST_INDETERMINATE);
                            options_->ClearOption(id);
                            break;

                        case BST_CHECKED:
                            Button_SetCheck(item, BST_UNCHECKED);
                            options_->UncheckOption(id);
                            break;

                        default:
                            Button_SetCheck(item, BST_CHECKED);
                            options_->CheckOption(id);
                    }

                    update_comment_string();

                    return TRUE;
                }
                break;
            }

            case EN_KILLFOCUS:
            {
                UINT const id = LOWORD(wParam);
                switch (id)
                {
                    case IDC_IDENT:
                    case IDC_MAXLEN:
                    case IDC_MAXERR:
                    {
                        auto const value{TrimSpaces(get_window_text(id))};
                        auto errmsg = options_->check_valid(id, value);
                        if (errmsg.has_value())
                        {
                            message_box(
                                errmsg.value().c_str(), MB_OK | MB_ICONERROR
                            );
                            SetFocus(id);
                            return false;
                        }
                        options_->SetOption(id, value);
                        break;
                    }

                    default:
                        break;
                }
                update_comment_string();
                return TRUE;
            }

            default:
                break;
        }
    }

    return std::nullopt;
}

void Options_Sub_Dialogue::update_comment_string() const
{
    SetWindowText(
        ::GetDlgItem(parent_->window(), IDC_PREVIEW),
        options_->GetOptionsCommentString().c_str()
    );
}
