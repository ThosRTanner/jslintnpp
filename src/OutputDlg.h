//This file is part of JSLint Plugin for Notepad++
//Copyright (C) 2010 Martin Vladic <martin.vladic@gmail.com>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef OUTPUT_DLG_H
#define OUTPUT_DLG_H

//#include "DockingFeature/DockingDlgInterface.h"
#include "Plugin/Docking_Dialogue_Interface.h"

#include "JSLint.h"

#include "resource.h"

#include <CommCtrl.h>

#include <list>
#include <vector>

class JSLintNpp;

class OutputDlg : public Docking_Dialogue_Interface
{
public :
	OutputDlg(int menu, JSLintNpp const *);
	~OutputDlg();

	/*
    virtual void display(bool toShow = true) const {
        DockingDlgInterface::display(toShow);
        if (toShow)
            ::SetFocus(::GetDlgItem(_hSelf, IDC_OUTPUT));
    };

	void setParent(HWND parent2set){
		_hParent = parent2set;
	};
	*/

	void on_display() noexcept
	{
		::SetFocus(GetDlgItem(IDC_OUTPUT));
	}

	HICON GetTabIcon();

	void ClearAllLints();
	void AddLints(const std::wstring& strFilePath, const std::list<JSLintReportItem>& lints);
	void SelectNextLint();
	void SelectPrevLint();

protected :

	void OnToolbarCmd(UINT message);
	void OnToolbarDropDown(LPNMTOOLBAR lpnmtb);

private:
	JSLintNpp const * plugin_;
	HICON m_hTabIcon;
    HWND m_hWndTab;

    static const int NUM_LIST_VIEWS = 2;
	HWND m_hWndListViews[NUM_LIST_VIEWS];

    struct TabDefinition {
        LPCTSTR m_strTabName;
        UINT m_listViewID;
        bool m_errorList;
    };
    static TabDefinition m_tabs[NUM_LIST_VIEWS];

	struct FileLint {
		FileLint(const std::wstring& strFilePath, const JSLintReportItem& lint)
			: strFilePath(strFilePath), lint(lint) {}
		std::wstring strFilePath;
		JSLintReportItem lint;
	};
	std::vector<FileLint> m_fileLints[NUM_LIST_VIEWS];

	std::optional<LONG_PTR> on_dialogue_message(UINT message, WPARAM wParam, LPARAM lParam) override;

	void InitializeTab();
    void InitializeListView(int i);
	void Resize();
    void OnTabSelChanged();
	void GetNameStrFromCmd(UINT resID, LPTSTR tip, UINT count);
	void ShowLint(int i);
	//FIXME vs is saying it can't find it. but it's defined at line 551 of the cpp.
	void CopyToClipboard();
};

#endif //OUTPUT_DLG_H
