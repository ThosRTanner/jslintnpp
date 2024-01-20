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

#include "resource.h"

#include <windowsx.h>

Options_Dialogue::Options_Dialogue(JSLintNpp *plugin) :
    Modal_Dialogue_Interface(plugin),
    options_(std::make_unique<JSLintOptions>(*plugin->options()))
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
            HWND hWndSelectedLinter = GetDlgItem(IDC_SELECTED_LINTER);
            ComboBox_AddString(hWndSelectedLinter, L"JSLint");
            ComboBox_AddString(hWndSelectedLinter, L"JSHint");

            // create JSLint and JSHint options subdialog
            HWND hWndOptionsPlaceholder = GetDlgItem(IDC_OPTIONS_PLACEHOLDER);
            RECT rect;
            GetWindowRect(hWndOptionsPlaceholder, &rect);
            POINT ptTopLeft;
            ptTopLeft.x = rect.left;
            ptTopLeft.y = rect.top;
            ScreenToClient(window(), &ptTopLeft);

            //THis appears to be a non modal dialogue window stuck on top of the modal one.
            /*
            auto m_hWndJSLintOptionsSubdlg = CreateDialog(
                plugin()->module(),
                MAKEINTRESOURCE(IDD_JSLINT_OPTIONS),
                window(),
                SubDlgProc
            );
            SetWindowPos(
                m_hWndJSLintOptionsSubdlg,
                hWndOptionsPlaceholder,
                ptTopLeft.x,
                ptTopLeft.y,
                rect.right - rect.left,
                rect.bottom - rect.top,
                0
            );

            auto m_hWndJSHintOptionsSubdlg = CreateDialog(
                plugin()->module(),
                MAKEINTRESOURCE(IDD_JSHINT_OPTIONS),
                window(),
                SubDlgProc
            );
            SetWindowPos(
                m_hWndJSHintOptionsSubdlg,
                hWndOptionsPlaceholder,
                ptTopLeft.x,
                ptTopLeft.y,
                rect.right - rect.left,
                rect.bottom - rect.top,
                0
            );

            if (options_->GetSelectedLinter() == Linter::LINTER_JSLINT)
            {
                auto m_hSubDlg = m_hWndJSLintOptionsSubdlg;
                ComboBox_SelectString(hWndSelectedLinter, 0, L"JSLint");
            }
            else
            {
                auto m_hSubDlg = m_hWndJSHintOptionsSubdlg;
                ComboBox_SelectString(hWndSelectedLinter, 0, L"JSHint");
            }
            //options_->UpdateOptions(m_hDlg, m_hSubDlg, false, false);
            //ShowWindow(m_hSubDlg, SW_SHOW);

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
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                switch (LOWORD(wParam))
                {
                    case IDC_CLEAR_ALL:
                        //options_->UpdateOptions(m_hDlg, m_hSubDlg, true, false);
                        options_->ClearAllOptions();
                       // options_->UpdateOptions(
                       //     m_hDlg, m_hSubDlg, false, false
                        //);
                        break;

                    case IDOK:
                        //if (options_->UpdateOptions(
                         //       m_hDlg, m_hSubDlg, true, true
                        //    ))
                        {
                            //*m_m_options = m_options;
                            EndDialog(1);
                        }
                        return 1;

                    case IDCANCEL:
                        EndDialog(0LL);
                        return 1;

                    default:
                        break;
                }
            }
            else if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                HWND hWndSelectedLinter = GetDlgItem(IDC_SELECTED_LINTER);
                int curSel = ComboBox_GetCurSel(hWndSelectedLinter);
                if (curSel != CB_ERR)
                {
                    /*
                    if (options_->UpdateOptions(m_hDlg, m_hSubDlg, true, true))
                    {
                        TCHAR buffer[32];
                        ComboBox_GetLBText(hWndSelectedLinter, curSel, buffer);

                        ShowWindow(m_hSubDlg, SW_HIDE);

                        if (_tcsicmp(buffer, L"JSLint") == 0)
                        {
                            options_->SetSelectedLinter(Linter::LINTER_JSLINT);
                            m_hSubDlg = m_hWndJSLintOptionsSubdlg;
                        }
                        else
                        {
                            options_->SetSelectedLinter(Linter::LINTER_JSHINT);
                            m_hSubDlg = m_hWndJSHintOptionsSubdlg;
                        }

                        options_->UpdateOptions(
                            m_hDlg, m_hSubDlg, false, false
                        );
                        ShowWindow(m_hSubDlg, SW_SHOW);
                    }
                    else
                    {
                        if (options_->GetSelectedLinter()
                            == Linter::LINTER_JSLINT)
                        {
                            ComboBox_SelectString(
                                hWndSelectedLinter, 0, L"JSLint"
                            );
                        }
                        else
                        {
                            ComboBox_SelectString(
                                hWndSelectedLinter, 0, L"JSHint"
                            );
                        }
                    }
                    */
                }
            }
            else if (HIWORD(wParam) == EN_KILLFOCUS)
            {
                //options_->UpdateOptions(m_hDlg, m_hSubDlg, true, false);
            }
        }
        break;
    }
    return std::nullopt;
}
