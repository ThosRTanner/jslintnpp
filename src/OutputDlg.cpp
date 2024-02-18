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

#include "OutputDlg.h"

#include "JSLint.h"
#include "JSLintNpp.h"
#include "JSLintOptions.h"
#include "Util.h"

#include "resource.h"

#include "Notepad_plus_msgs.h"
#include "Scintilla.h"

#include <CommCtrl.h>
#include <WinBase.h>
#include <WinUser.h>
#include <basetsd.h>
#include <tchar.h>

#include <sstream>
#include <string>

////////////////////////////////////////////////////////////////////////////////

#define ID_COPY_LINTS 1500
#define ID_SHOW_LINT 1501
#define ID_ADD_PREDEFINED 1502
#define ID_SELECT_ALL 1503

#define IDM_TOOLBAR 2000

#define IDM_TB_JSLINT_CURRENT_FILE (IDM_TOOLBAR + 1)
#define IDM_TB_JSLINT_ALL_FILES (IDM_TOOLBAR + 2)
#define IDM_TB_PREV_LINT (IDM_TOOLBAR + 3)
#define IDM_TB_NEXT_LINT (IDM_TOOLBAR + 4)
#define IDM_TB_JSLINT_OPTIONS (IDM_TOOLBAR + 5)

////////////////////////////////////////////////////////////////////////////////

OutputDlg::TabDefinition OutputDlg::m_tabs[] = {
    {L"Errors", IDC_ERROR_LIST,  true },
    {L"Unused", IDC_UNUSED_LIST, false}
};

////////////////////////////////////////////////////////////////////////////////

OutputDlg::OutputDlg(
    int menu_entry, JSLintNpp const *plugin, JSLintOptions *options
) :
    Docking_Dialogue_Interface(IDD_OUTPUT, plugin),
    plugin_(plugin),
    options_(options)
{
    for (int i = 0; i < NUM_LIST_VIEWS; ++i)
    {
        m_hWndListViews[i] = 0;
    }

    InitializeTab();
    for (int i = 0; i < NUM_LIST_VIEWS; ++i)
    {
        InitializeListView(i);
    }
    OnTabSelChanged();

    register_dialogue(menu_entry, Position::Dock_Bottom, GetTabIcon());
}

OutputDlg::~OutputDlg()
{
}

std::optional<LONG_PTR> OutputDlg::on_dialogue_message(
    UINT message, WPARAM wParam, LPARAM lParam
)
{
    switch (message)
    {
        case WM_COMMAND:
        {
            if (LOWORD(wParam) == ID_COPY_LINTS)
            {
                CopyToClipboard();
                return TRUE;
            }
            else if (LOWORD(wParam) == ID_SHOW_LINT)
            {
                int iTab = TabCtrl_GetCurSel(m_hWndTab);
                int const iLint = ListView_GetNextItem(
                    m_hWndListViews[iTab], -1, LVIS_FOCUSED | LVIS_SELECTED
                );
                if (iLint != -1)
                {
                    ShowLint(iLint);
                }
                return TRUE;
            }
            else if (LOWORD(wParam) == ID_ADD_PREDEFINED)
            {
                int iTab = TabCtrl_GetCurSel(m_hWndTab);
                int const iLint = ListView_GetNextItem(
                    m_hWndListViews[iTab], -1, LVIS_FOCUSED | LVIS_SELECTED
                );
                if (iLint != -1)
                {
                    FileLint const &fileLint = m_fileLints[iTab][iLint];
                    std::wstring var = fileLint.lint.GetUndefVar(plugin_);
                    if (! var.empty())
                    {
                        options_->AppendOption(IDC_PREDEFINED, var);
                    }
                }
                return TRUE;
            }
            else if (LOWORD(wParam) == ID_SELECT_ALL)
            {
                ListView_SetItemState(
                    m_hWndListViews[TabCtrl_GetCurSel(m_hWndTab)],
                    -1,
                    LVIS_SELECTED,
                    LVIS_SELECTED
                );
                return TRUE;
            }
        }
        break;

        case WM_NOTIFY:
        {
            LPNMHDR pNMHDR = reinterpret_cast<LPNMHDR>(lParam);
            if (pNMHDR->idFrom
                    == m_tabs[TabCtrl_GetCurSel(m_hWndTab)].m_listViewID
                && pNMHDR->code == LVN_KEYDOWN)
            {
                auto pnkd = reinterpret_cast<NMLVKEYDOWN const *>(lParam);
                if (pnkd->wVKey == 'A' && (::GetKeyState(VK_CONTROL) >> 15 & 1))
                {
                    ListView_SetItemState(
                        m_hWndListViews[TabCtrl_GetCurSel(m_hWndTab)],
                        -1,
                        LVIS_SELECTED,
                        LVIS_SELECTED
                    );
                }
                else if (pnkd->wVKey == 'C' && (::GetKeyState(VK_CONTROL) >> 15 & 1))
                {
                    CopyToClipboard();
                }
            }
            else if (pNMHDR->idFrom == m_tabs[TabCtrl_GetCurSel(m_hWndTab)].m_listViewID && pNMHDR->code == NM_DBLCLK)
            {
                auto lpnmitem = reinterpret_cast<NMITEMACTIVATE const *>(lParam);
                int const iFocused = lpnmitem->iItem;
                if (iFocused != -1)
                {
                    ShowLint(iFocused);
                }
            }
            else if (pNMHDR->code == TTN_GETDISPINFO)
            {
                LPTOOLTIPTEXT lpttt = reinterpret_cast<LPTOOLTIPTEXT>(pNMHDR);
                lpttt->hinst = plugin_->module();

                // Specify the resource identifier of the descriptive
                // text for the given button.
                auto const resId = lpttt->hdr.idFrom;

                TCHAR tip[MAX_PATH];
                GetNameStrFromCmd(resId, &tip[0], sizeof(tip));
                lpttt->lpszText = &tip[0];
                return TRUE;
            }
            else if (pNMHDR->idFrom == IDC_TAB && pNMHDR->code == TCN_SELCHANGE)
            {
                OnTabSelChanged();
                return TRUE;
            }
            return std::nullopt;
        }
        break;

        case WM_CONTEXTMENU:
        {
            // build context menu
            HMENU menu = ::CreatePopupMenu();

            int const numSelected = ListView_GetSelectedCount(
                m_hWndListViews[TabCtrl_GetCurSel(m_hWndTab)]
            );

            int iTab = TabCtrl_GetCurSel(m_hWndTab);

            int iFocused = -1;
            if (numSelected > 0)
            {
                iFocused = ListView_GetNextItem(
                    m_hWndListViews[iTab], -1, LVIS_FOCUSED | LVIS_SELECTED
                );
            }

            bool reasonIsUndefVar = false;
            if (iFocused != -1)
            {
                FileLint const &fileLint = m_fileLints[iTab][iFocused];
                reasonIsUndefVar = fileLint.lint.IsReasonUndefVar(plugin_);
            }

            if (iFocused != -1)
            {
                AppendMenu(menu, MF_ENABLED, ID_SHOW_LINT, L"Show");
            }

            if (reasonIsUndefVar)
            {
                AppendMenu(
                    menu,
                    MF_ENABLED,
                    ID_ADD_PREDEFINED,
                    L"Add to the Predefined List"
                );
            }

            if (GetMenuItemCount(menu) > 0)
            {
                AppendMenu(menu, MF_SEPARATOR, 0, nullptr);
            }

            if (numSelected > 0)
            {
                AppendMenu(menu, MF_ENABLED, ID_COPY_LINTS, L"Copy");
            }

            AppendMenu(menu, MF_ENABLED, ID_SELECT_ALL, L"Select All");

            // determine context menu position
            POINT point;
            point.x = LOWORD(lParam);
            point.y = HIWORD(lParam);
            if (point.x == 65535 || point.y == 65535)
            {
                point.x = 0;
                point.y = 0;
                ClientToScreen(
                    m_hWndListViews[TabCtrl_GetCurSel(m_hWndTab)], &point
                );
            }

            // show context menu
            TrackPopupMenu(menu, 0, point.x, point.y, 0, window(), nullptr);
        }
        break;

        case WM_SIZE:
        case WM_MOVE:
            // FIXME Resize currently does nothing.
            Resize();
            break;

        case WM_PAINT:
            // FIXME Should we just drop through?
            /*::RedrawWindow(m_toolbar.getHSelf(), nullptr, nullptr, TRUE);*/
            break;

        default:
            break;
    }

    return std::nullopt;
}

void OutputDlg::InitializeTab() noexcept
{
    m_hWndTab = GetDlgItem(IDC_TAB);

    TCITEM tie;

    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.iImage = -1;

    for (int i = 0; i < NUM_LIST_VIEWS; ++i)
    {
        tie.pszText = const_cast<LPTSTR>(m_tabs[i].m_strTabName);
        TabCtrl_InsertItem(m_hWndTab, i, &tie);
    }
}

void OutputDlg::InitializeListView(int i) noexcept
{
    m_hWndListViews[i] = GetDlgItem(m_tabs[i].m_listViewID);

    ListView_SetExtendedListViewStyle(m_hWndListViews[i], LVS_EX_FULLROWSELECT);

    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    int iCol = 0;

    lvc.iSubItem = iCol;
    lvc.pszText = const_cast<wchar_t *>(L"");
    lvc.cx = 28;
    lvc.fmt = LVCFMT_RIGHT;
    ListView_InsertColumn(m_hWndListViews[i], iCol++, &lvc);

    if (m_tabs[i].m_errorList)
    {
        lvc.iSubItem = iCol;
        lvc.pszText = const_cast<wchar_t *>(L"Reason");
        lvc.cx = 500;
        lvc.fmt = LVCFMT_LEFT;
        ListView_InsertColumn(m_hWndListViews[i], iCol++, &lvc);
    }
    else
    {
        lvc.iSubItem = iCol;
        lvc.pszText = const_cast<wchar_t *>(L"Variable");
        lvc.cx = 250;
        lvc.fmt = LVCFMT_LEFT;
        ListView_InsertColumn(m_hWndListViews[i], iCol++, &lvc);

        lvc.iSubItem = iCol;
        lvc.pszText = const_cast<wchar_t *>(L"Function");
        lvc.cx = 250;
        lvc.fmt = LVCFMT_LEFT;
        ListView_InsertColumn(m_hWndListViews[i], iCol++, &lvc);
    }

    lvc.iSubItem = iCol;
    lvc.pszText = const_cast<wchar_t *>(L"File");
    lvc.cx = 200;
    lvc.fmt = LVCFMT_LEFT;
    ListView_InsertColumn(m_hWndListViews[i], iCol++, &lvc);

    lvc.iSubItem = iCol;
    lvc.pszText = const_cast<wchar_t *>(L"Line");
    lvc.cx = 50;
    lvc.fmt = LVCFMT_RIGHT;
    ListView_InsertColumn(m_hWndListViews[i], iCol++, &lvc);

    lvc.iSubItem = iCol;
    lvc.pszText = const_cast<wchar_t *>(L"Column");
    lvc.cx = 50;
    lvc.fmt = LVCFMT_RIGHT;
    ListView_InsertColumn(m_hWndListViews[i], iCol++, &lvc);
}

void OutputDlg::Resize()
{
    RECT const rc = getClientRect();

    ::MoveWindow(
        m_hWndTab, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE
    );

    TabCtrl_AdjustRect(m_hWndTab, FALSE, &rc);
    // InflateRect(&rc, -4, -4);
    for (int i = 0; i < NUM_LIST_VIEWS; ++i)
    {
        ::SetWindowPos(
            m_hWndListViews[i],
            m_hWndTab,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            0
        );
    }
}

void OutputDlg::OnTabSelChanged()
{
    int const iSel = TabCtrl_GetCurSel(m_hWndTab);
    for (int i = 0; i < NUM_LIST_VIEWS; ++i)
    {
        ShowWindow(m_hWndListViews[i], iSel == i ? SW_SHOW : SW_HIDE);
    }
}

HICON OutputDlg::GetTabIcon()
{
    // Possibly one should free this up, but I don't see the dialogue memory
    // being freed up anywhere.
    return static_cast<HICON>(::LoadImage(
        plugin_->module(),
        MAKEINTRESOURCE(IDI_JSLINT_TAB),
        IMAGE_ICON,
        0,
        0,
        LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT
    ));
}

void OutputDlg::GetNameStrFromCmd(UINT resID, LPTSTR tip, UINT count)
{
    // NOTE: On change, keep sure to change order of IDM_EX_... in toolBarIcons
    // also
    static wchar_t const *szToolTip[] = {
        L"JSLint Current File",
        L"JSLint All Files",
        L"Go To Previous Lint",
        L"Go To Next Lint",
        L"JSLint Options",
    };

    _tcscpy(tip, szToolTip[resID - IDM_TB_JSLINT_CURRENT_FILE]);
}

void OutputDlg::ClearAllLints()
{
    for (int i = 0; i < NUM_LIST_VIEWS; ++i)
    {
        m_fileLints[i].clear();
        ListView_DeleteAllItems(m_hWndListViews[i]);
    }
}

void OutputDlg::AddLints(
    std::wstring const &strFilePath, std::list<JSLintReportItem> const &lints
)
{
    std::basic_stringstream<TCHAR> stream;

    LVITEM lvI;
    lvI.mask = LVIF_TEXT | LVIF_STATE;

    for (auto const & lint : lints)
    {
        HWND hWndListView = m_hWndListViews[lint.GetType()];

        lvI.iSubItem = 0;
        lvI.iItem = ListView_GetItemCount(hWndListView);
        lvI.state = 0;
        lvI.stateMask = 0;

        stream.str(L"");
        stream << lvI.iItem + 1;
        std::wstring strNum = stream.str();

        lvI.pszText = const_cast<LPTSTR>(strNum.c_str());

        ListView_InsertItem(hWndListView, &lvI);

        int iCol = 1;

        if (m_tabs[lint.GetType()].m_errorList)
        {
            std::wstring strReason = lint.GetReason();
            ListView_SetItemText(
                hWndListView, lvI.iItem, iCol++, (LPTSTR)strReason.c_str()
            );
        }
        else
        {
            std::wstring strVariable = lint.GetReason();
            ListView_SetItemText(
                hWndListView, lvI.iItem, iCol++, (LPTSTR)strVariable.c_str()
            );
            std::wstring strFunction = lint.GetEvidence();
            ListView_SetItemText(
                hWndListView, lvI.iItem, iCol++, (LPTSTR)strFunction.c_str()
            );
        }

        std::wstring strFile = Path::GetFileName(strFilePath);
        ListView_SetItemText(
            hWndListView, lvI.iItem, iCol++, (LPTSTR)strFile.c_str()
        );

        stream.str(L"");
        stream << lint.GetLine() + 1;
        std::wstring strLine = stream.str();
        ListView_SetItemText(
            hWndListView, lvI.iItem, iCol++, (LPTSTR)strLine.c_str()
        );

        stream.str(L"");
        stream << lint.GetCharacter() + 1;
        std::wstring strColumn = stream.str();
        ListView_SetItemText(
            hWndListView, lvI.iItem, iCol++, (LPTSTR)strColumn.c_str()
        );

        m_fileLints[lint.GetType()].push_back(FileLint(strFilePath, lint));
    }

    for (int i = 0; i < NUM_LIST_VIEWS; ++i)
    {
        std::wstring strTabName;
        int const count = ListView_GetItemCount(m_hWndListViews[i]);
        if (count > 0)
        {
            stream.str(L"");
            stream << m_tabs[i].m_strTabName << L" (" << count << L")";
            strTabName = stream.str();
        }
        else
        {
            strTabName = m_tabs[i].m_strTabName;
        }
        TCITEM tie;
        tie.mask = TCIF_TEXT;
        tie.pszText = const_cast<LPTSTR>(strTabName.c_str());
        TabCtrl_SetItem(m_hWndTab, i, &tie);
    }

    InvalidateRect();
}

void OutputDlg::SelectNextLint()
{
    int iTab = TabCtrl_GetCurSel(m_hWndTab);
    HWND hWndListView = m_hWndListViews[iTab];

    int const count = ListView_GetItemCount(hWndListView);
    if (count == 0)
    {
        // no lints, set focus to editor
        ::SetFocus(plugin_->get_scintilla_window());
        return;
    }

    int i = ListView_GetNextItem(hWndListView, -1, LVNI_FOCUSED | LVNI_SELECTED)
        + 1;
    if (i == count)
    {
        i = 0;
    }

    ListView_SetItemState(hWndListView, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);

    ListView_SetItemState(
        hWndListView,
        i,
        LVIS_SELECTED | LVIS_FOCUSED,
        LVIS_SELECTED | LVIS_FOCUSED
    );
    ListView_EnsureVisible(hWndListView, i, FALSE);
    ShowLint(i);
}

void OutputDlg::SelectPrevLint()
{
    int iTab = TabCtrl_GetCurSel(m_hWndTab);
    HWND hWndListView = m_hWndListViews[iTab];

    int const count = ListView_GetItemCount(hWndListView);
    if (count == 0)
    {
        // no lints, set focus to editor
        ::SetFocus(plugin_->get_scintilla_window());
        return;
    }

    int i = ListView_GetNextItem(hWndListView, -1, LVNI_FOCUSED | LVNI_SELECTED)
        - 1;
    if (i == -1)
    {
        i = count - 1;
    }

    ListView_SetItemState(hWndListView, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);

    ListView_SetItemState(
        hWndListView,
        i,
        LVIS_SELECTED | LVIS_FOCUSED,
        LVIS_SELECTED | LVIS_FOCUSED
    );
    ListView_EnsureVisible(hWndListView, i, FALSE);
    ShowLint(i);
}

void OutputDlg::ShowLint(int i)
{
    int iTab = TabCtrl_GetCurSel(m_hWndTab);
    FileLint const &fileLint = m_fileLints[iTab][i];

    int const line = fileLint.lint.GetLine();
    int const column = fileLint.lint.GetCharacter();

    if (! fileLint.strFilePath.empty() && line >= 0 && column >= 0)
    {
        LRESULT const lRes = plugin_->send_to_notepad(
            NPPM_SWITCHTOFILE, 0, fileLint.strFilePath.c_str()
        );
        if (lRes)
        {
            HWND const hWndScintilla = plugin_->get_scintilla_window();
            if (hWndScintilla != nullptr)
            {
                plugin_->send_to_editor(SCI_GOTOLINE, line);
                // since there is no SCI_GOTOCOLUMN, we move to the right until
                // ...
                while (true)
                {
                    plugin_->send_to_editor(SCI_CHARRIGHT);

                    LRESULT const curPos = plugin_->send_to_editor(SCI_GETCURRENTPOS);

                    LRESULT const curLine =
                        plugin_->send_to_editor(SCI_LINEFROMPOSITION, curPos);

                    if (curLine > line)
                    {
                        // ... current line is greater than desired line or ...
                        plugin_->send_to_editor(SCI_CHARLEFT);
                        break;
                    }

                    LRESULT const curCol =
                        plugin_->send_to_editor(SCI_GETCOLUMN, curPos);
                    if (curCol > column)
                    {
                        // ... current column is greater than desired column or
                        // ...
                        plugin_->send_to_editor(SCI_CHARLEFT);
                        break;
                    }

                    if (curCol == column)
                    {
                        // ... we reached desired column.
                        break;
                    }
                }
            }
        }
    }

    InvalidateRect();
}

void OutputDlg::CopyToClipboard()
{
    std::basic_stringstream<TCHAR> stream;

    int iTab = TabCtrl_GetCurSel(m_hWndTab);

    bool bFirst = true;
    int i = ListView_GetNextItem(m_hWndListViews[iTab], -1, LVNI_SELECTED);
    while (i != -1)
    {
        FileLint const &fileLint = m_fileLints[iTab][i];

        if (bFirst)
        {
            bFirst = false;
        }
        else
        {
            stream << L"\r\n";
        }

        stream << L"Line " << fileLint.lint.GetLine() + 1 << L", column "
               << fileLint.lint.GetCharacter() + 1 << L": "
               << fileLint.lint.GetReason().c_str() << L"\r\n\t"
               << fileLint.lint.GetEvidence().c_str() << L"\r\n";

        i = ListView_GetNextItem(
            m_hWndListViews[TabCtrl_GetCurSel(m_hWndTab)], i, LVNI_SELECTED
        );
    }

    std::wstring str = stream.str();
    if (str.empty())
    {
        return;
    }

    if (OpenClipboard(window()))
    {
        if (EmptyClipboard())
        {
            size_t const size = (str.size() + 1) * sizeof(TCHAR);
            HGLOBAL hResult = GlobalAlloc(GMEM_MOVEABLE, size);
            LPTSTR lpsz = static_cast<LPTSTR>(GlobalLock(hResult));
            memcpy(lpsz, str.c_str(), size);
            GlobalUnlock(hResult);

            if (SetClipboardData(CF_UNICODETEXT, hResult) == nullptr)
            {
                GlobalFree(hResult);
                message_box(
                    L"Unable to set Clipboard data", MB_OK | MB_ICONERROR
                );
            }
        }
        else
        {
            message_box(L"Cannot empty the Clipboard", MB_OK | MB_ICONERROR);
        }
        CloseClipboard();
    }
    else
    {
        message_box(L"Cannot open the Clipboard", MB_OK | MB_ICONERROR);
    }
}
