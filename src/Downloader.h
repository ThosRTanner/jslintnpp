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

#include "Internet_Handle.h"
#include "Linter_Versions.h"
#include "Url_Components.h"

//#include <stdint.h>

//#include <memory>
#include <string>
#include <vector>
#include <cstdint>

// Forward includes from windows headers
typedef struct _WINHTTP_URL_COMPONENTS URL_COMPONENTS;

class Download_Progress_Bar;
enum class Linter;

class Downloader
{
  public:
    // FIXME I now have two of these
    enum DownloadResult
    {
        DOWNLOAD_OK = 1,
        DOWNLOAD_NO_NEW_VERSION,
        DOWNLOAD_FAILED
    };

    Downloader(Download_Progress_Bar *, wchar_t const *url, Linter, Linter_Versions const &);

    ~Downloader();

    std::vector<uint8_t> const &data() const noexcept
    {
        return out_buffer_;
    }

    std::wstring version() const
    {
        return version_;
    }

  private:
    Download_Progress_Bar *progress_bar_;
    Linter linter_;
    Linter_Versions const &versions_;
    Url_Components url_components_;
    Internet_Handle session_;
    Internet_Handle connect_;
    Internet_Handle request_;
    std::vector<uint8_t> out_buffer_;
    uint64_t total_size_{0};
    std::wstring version_;

    HINTERNET open_connection();
    HINTERNET open_request();

    void cleanup_context(DownloadResult result);

    void DownloadOK()
    {
        cleanup_context(DOWNLOAD_OK);
    }

    void DownloadNoNewVersion()
    {
        cleanup_context(DOWNLOAD_NO_NEW_VERSION);
    }

    void DownloadFailed()
    {
        cleanup_context(DOWNLOAD_FAILED);
    }

    static void CALLBACK winhttp_callback(
        HINTERNET handle, DWORD_PTR context, DWORD status, LPVOID information,
        DWORD length
    );

    void winhttp_callback_handler(
        DWORD status, LPVOID information, DWORD length
    );

    bool already_have_version();
};
