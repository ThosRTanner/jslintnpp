#pragma once

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

#include "Plugin/Modal_Dialogue_Interface.h"

#include <functional>

//class __single_inheritance DownloadJSLint;    // Thank you microsoft.
enum class Linter;

class Download_Progress_Bar : public Modal_Dialogue_Interface
{
  public:
    typedef std::function<void (Download_Progress_Bar *)> Download_Func;

    Download_Progress_Bar(
        Plugin const *, Linter, Download_Func
    );

    ~Download_Progress_Bar();

    /** Show progress (sort of) */
    void update(DWORD transferred);

    /** Flag operation is completed.
     *
     * Result will be returned via get_result
     */
    void completed(int result);

  private:
    std::optional<LONG_PTR> on_dialogue_message(
        UINT message, WPARAM wParam, LPARAM lParam
    ) override;

    Linter linter_;
    Download_Func start_download_;
};
