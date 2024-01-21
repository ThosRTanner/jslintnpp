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

#include "resource.h"

#include <windowsx.h>
#include "tchar.h"
#include "WinUser.h"

#include <map>

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

std::optional<LONG_PTR> Options_Dialogue::on_dialogue_message(
    UINT message, WPARAM wParam, LPARAM lParam
) noexcept
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

            // subclass IDC_PREDEFINED
            /*
            HWND hWndPredefined = GetDlgItem(IDC_PREDEFINED);
            WNDPROC oldWndProc = (WNDPROC)SetWindowLongPtr(
                hWndPredefined, GWLP_WNDPROC, (LONG_PTR)PredefinedControlWndProc
            );
            SetProp(hWndPredefined, L"OldWndProc", (HANDLE)oldWndProc);
            */
            break;
        }

        case WM_COMMAND:
            return on_command(wParam);
    }
    return std::nullopt;
}

std::optional<LONG_PTR> Options_Dialogue::on_command(WPARAM wParam) noexcept
{
    switch (HIWORD(wParam))
    {
        case BN_CLICKED:
        {
            switch (LOWORD(wParam))
            {
                case IDC_CLEAR_ALL:
                    // options_->UpdateOptions(m_hDlg, m_hSubDlg, true,
                    // false);
                    options_->ResetAllOptions();
                    // options_->UpdateOptions(
                    //     m_hDlg, m_hSubDlg, false, false
                    //);
                    break;

                case IDOK:
                    // if (options_->UpdateOptions(
                    //        m_hDlg, m_hSubDlg, true, true
                    //    ))
                    {
                        //*m_m_options = m_options;
                        EndDialog(Clicked_OK);
                    }
                    return 1;

                case IDCANCEL:
                    EndDialog(Clicked_Cancel);
                    return 1;

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
            if (true /*options_->UpdateOptions(m_hDlg, m_hSubDlg, true, true)*/)
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
            // options_->UpdateOptions(m_hDlg, m_hSubDlg, true, false);
            break;
        }

        default:
            break;
    }
    return std::nullopt;
}
