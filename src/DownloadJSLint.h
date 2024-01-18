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

#include "Linter_Versions.h"

#include <string>

class JSLintNpp;
enum class Linter;
class Version_Info;

class DownloadJSLint
{
  public:
    DownloadJSLint(JSLintNpp const *plugin);

    enum DownloadResult
    {
        DOWNLOAD_OK = 1,
        DOWNLOAD_NO_NEW_VERSION,
        DOWNLOAD_FAILED
    };
    DownloadResult DownloadLatest(Linter linter, std::wstring &latestVersion);

    Linter_Versions const &GetVersions(Linter linter) const;

    bool HasVersion(Linter linter, std::wstring const &version) const;

    Version_Info const &GetVersion(Linter linter, std::wstring const &version)
        const;

  private:
    JSLintNpp const *plugin_;
    std::wstring config_dir_;
    std::wstring m_versionsFolder;
    Linter_Versions m_jsLintVersions;
    Linter_Versions m_jsHintVersions;

    Linter_Versions &GetVersions(Linter linter)
    {
        return const_cast<Linter_Versions &>(
            static_cast<DownloadJSLint const *>(this)->GetVersions(linter)
        );
    }

    void LoadVersions(std::wstring const &fileSpec, Linter_Versions &versions);
};
