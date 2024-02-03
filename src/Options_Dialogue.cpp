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

#include "Options_Dialogue.h"

#include "JSLintNpp.h"
#include "JSLintOptions.h"
#include "Linter.h"
#include "Options_Sub_Dialogue.h"
#include "Util.h"

#include "resource.h"

#include <WinBase.h>
#include <WinUser.h>
#include <basetsd.h>
#include <windowsx.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

using namespace std::placeholders;

Options_Dialogue::Options_Dialogue(JSLintNpp const *plugin) :
    Modal_Dialogue_Interface(plugin),
    options_(std::make_unique<JSLintOptions>(*(plugin->options()))),
    current_linter_(options_->GetSelectedLinter())
{
    create_modal_dialogue(IDD_OPTIONS);
}

Options_Dialogue::~Options_Dialogue()
{
}

std::optional<INT_PTR> Options_Dialogue::on_dialogue_message(
    UINT message, WPARAM wParam, LPARAM lParam
)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            centre_dialogue();

            // initialize selected linter combo box
            HWND const hWndSelectedLinter = GetDlgItem(IDC_SELECTED_LINTER);
            ComboBox_AddString(hWndSelectedLinter, L"JSLint");
            ComboBox_AddString(hWndSelectedLinter, L"JSHint");

            std::map<Linter, int> const linters{
                {Linter::LINTER_JSLINT, IDD_JSLINT_OPTIONS},
                {Linter::LINTER_JSHINT, IDD_JSHINT_OPTIONS}
            };

            for (auto const &linter : linters)
            {
                sub_dialogues_[linter.first] =
                    std::make_unique<Options_Sub_Dialogue>(
                        linter.second,
                        plugin(),
                        this,
                        options_->GetLinterOptions(linter.first)
                    );
            }

            if (current_linter_ == Linter::LINTER_JSLINT)
            {
                ComboBox_SelectString(hWndSelectedLinter, 0, L"JSLint");
            }
            else
            {
                ComboBox_SelectString(hWndSelectedLinter, 0, L"JSHint");
            }

            sub_dialogues_[current_linter_]->show();

            auto f = std::bind(
                &Options_Dialogue::Item_Callback_Function, this, _1, _2, _3, _4
            );
            add_item_callback(IDC_PREDEFINED, f);

            break;
        }

        case WM_COMMAND:
            return on_command(wParam);

        default:
            break;
    }
    return std::nullopt;
}

std::optional<INT_PTR> Options_Dialogue::on_command(WPARAM wParam)
{
    switch (HIWORD(wParam))
    {
        case BN_CLICKED:
        {
            switch (LOWORD(wParam))
            {
                case IDC_CLEAR_ALL:
                    // This appears to be just so the 'predefined' state doesn't
                    // get cleared.
                    sub_dialogues_[current_linter_]->update(false);
                    options_->ResetAllOptions();
                    sub_dialogues_[current_linter_]->show();
                    break;

                case IDOK:
                    if (sub_dialogues_[current_linter_]->update(true))
                    {
                        EndDialog(Clicked_OK);
                    }
                    return TRUE;

                case IDCANCEL:
                    EndDialog(Clicked_Cancel);
                    return TRUE;

                default:
                    break;
            }
            break;
        }

        case CBN_SELCHANGE:
        {
            HWND const hWndSelectedLinter = GetDlgItem(IDC_SELECTED_LINTER);
            int const curSel = ComboBox_GetCurSel(hWndSelectedLinter);
            if (curSel == CB_ERR)
            {
                break;
            }
            if (sub_dialogues_[current_linter_]->update(true))
            {
                sub_dialogues_[current_linter_]->hide();

                current_linter_ =
                    curSel == 0 ? Linter::LINTER_JSHINT : Linter::LINTER_JSLINT;

                options_->SetSelectedLinter(current_linter_);

                sub_dialogues_[current_linter_]->show();
            }
            else
            {
                if (current_linter_ == Linter::LINTER_JSLINT)
                {
                    ComboBox_SelectString(hWndSelectedLinter, 0, L"JSLint");
                }
                else
                {
                    ComboBox_SelectString(hWndSelectedLinter, 0, L"JSHint");
                }
            }
            break;
        }

        case EN_KILLFOCUS:
        {
            // Why do we do this? Update the command line comment?
            // Seems little point in this
            sub_dialogues_[current_linter_]->update(false);
            break;
        }

        default:
            break;
    }
    return std::nullopt;
}

std::optional<INT_PTR> Options_Dialogue::Item_Callback_Function(
    HWND handle, UINT message, WPARAM wParam, LPARAM lParam
)
{
    if (message == WM_PASTE)
    {
        if (IsClipboardFormatAvailable(CF_TEXT))
        {
            if (OpenClipboard(nullptr))
            {
                HGLOBAL hGlobal = GetClipboardData(CF_TEXT);
                if (hGlobal)
                {
                    auto const lpData = static_cast<CHAR const *>(GlobalLock(hGlobal));
                    if (lpData != nullptr)
                    {
                        std::wstring str(TextConversion::A_To_T(lpData));

                        std::vector<std::wstring> results;
                        StringSplit(str, L" \t\r\n", results);
                        str = StringJoin(results, L", ");

                        SendMessage(
                            handle, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(str.c_str())
                        );
                    }
                    GlobalUnlock(hGlobal);
                }
                CloseClipboard();
            }
        }
        return 0;
    }
    return std::nullopt;
}
