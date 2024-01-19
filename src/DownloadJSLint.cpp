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

#include "DownloadJSLint.h"

#include "Download_Progress_Bar.h"
#include "JSLintNpp.h"
#include "Linter.h"
#include "Util.h"
#include "Version_Info.h"

#include <stdio.h>

DownloadJSLint::DownloadJSLint(JSLintNpp const *plugin) :
    plugin_(plugin),
    config_dir_(plugin_->config_dir()),
    m_versionsFolder(Path::GetFullPath(L"JSLint", config_dir_))
{
    if (! Path::IsFileExists(m_versionsFolder))
    {
        CreateDirectory(m_versionsFolder.c_str(), NULL);
    }

    LoadVersions(L"jslint.*.js", m_jsLintVersions);
    LoadVersions(L"jshint.*.js", m_jsHintVersions);
}

void DownloadJSLint::LoadVersions(
    std::wstring const &fileSpec, Linter_Versions &versions
)
{
    WIN32_FIND_DATA findFileData;
    HANDLE findFileHandle = FindFirstFile(
        Path::GetFullPath(fileSpec.c_str(), m_versionsFolder).c_str(),
        &findFileData
    );
    if (findFileHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            versions.insert(std::make_pair<std::wstring, Version_Info>(
                std::wstring(findFileData.cFileName)
                    .substr(7, _tcslen(findFileData.cFileName) - 10),
                Version_Info(
                    Path::GetFullPath(findFileData.cFileName, m_versionsFolder)
                )
            ));
        } while (FindNextFile(findFileHandle, &findFileData));
        FindClose(findFileHandle);
    }
}

Linter_Versions const &DownloadJSLint::GetVersions(Linter linter) const
{
    return linter == Linter::LINTER_JSLINT ? m_jsLintVersions
                                           : m_jsHintVersions;
}

bool DownloadJSLint::HasVersion(Linter linter, std::wstring const &version)
    const
{
    return GetVersions(linter).contains(version);
}

Version_Info const &DownloadJSLint::GetVersion(
    Linter linter, std::wstring const &version
) const
{
    return GetVersions(linter).at(version);
}

DownloadJSLint::DownloadResult DownloadJSLint::DownloadLatest(
    Linter linter, std::wstring &latestVersion
)
{
    Download_Progress_Bar progress_bar{plugin_, linter, GetVersions(linter)};
    DownloadResult res = static_cast<DownloadResult>(progress_bar.get_result());
    if (res == DOWNLOAD_OK)
    {
        latestVersion = progress_bar.version();
        if (latestVersion.empty())
        {
            SYSTEMTIME time;
            GetLocalTime(&time);

            TCHAR szTime[1024];
            _stprintf(
                szTime,
                L"%.4d-%.2d-%.2d %.2d-%.2d-%.2d",
                time.wYear,
                time.wMonth,
                time.wDay,
                time.wHour,
                time.wMinute,
                time.wSecond
            );

            latestVersion = szTime;
        }

        std::wstring fileName = Path::GetFullPath(
            (linter == Linter::LINTER_JSLINT ? L"jslint." : L"jshint.")
                + latestVersion + L".js",
            m_versionsFolder
        );

        auto const &data = progress_bar.data();

        size_t written = 0;
        FILE *fp = _tfopen(fileName.c_str(), L"wb+");
        if (fp != NULL)
        {
            written = fwrite(&data[0], 1, data.size(), fp);
            fclose(fp);
        }

        if (written == data.size())
        {
            std::string content(data.begin(), data.end());
            GetVersions(linter).insert(
                std::make_pair(latestVersion, Version_Info(fileName, content))
            );
        }
        else
        {
            res = DOWNLOAD_FAILED;
        }
    }
    return res;
}
