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

#include "Linter_Versions.h"

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

class Downloader;
enum class Linter;
class Version_Info;

class Download_Progress_Bar : public Modal_Dialogue_Interface
{
  public:
    Download_Progress_Bar(Plugin const *, Linter, Linter_Versions const &);

    ~Download_Progress_Bar();

    /** Show progress (sort of) */
    void update(std::size_t transferred) const noexcept;

    /** Flag operation is completed.
     *
     * Result will be returned via get_result
     */
    void completed(int result) noexcept;

    /** Get the downloaded data */
    std::vector<uint8_t> const &data() const noexcept;

    /** Get the version number of the downloaded code */
    std::wstring version() const;

  private:
    std::optional<LONG_PTR> on_dialogue_message(
        UINT message, WPARAM wParam, LPARAM lParam
    ) override;

    Linter linter_;
    Linter_Versions const &versions_;
    std::unique_ptr<Downloader> downloader_;
};
