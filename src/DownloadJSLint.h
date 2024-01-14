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

#include <winhttp.h>

#include <unordered_map>
#include <string>

class Download_Progress_Bar;
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

    typedef std::unordered_map<std::wstring, Version_Info> Linter_Versions;
    Linter_Versions const &GetVersions(Linter linter) const;
    bool HasVersion(Linter linter, std::wstring const &version);
    Version_Info &GetVersion(Linter linter, std::wstring const &version);

  private:
    JSLintNpp const *plugin_;
    std::wstring config_dir_;
    std::wstring m_versionsFolder;
    Linter_Versions m_jsLintVersions;
    Linter_Versions m_jsHintVersions;

    Download_Progress_Bar *progress_bar_;
    HINTERNET m_hSession;
    HINTERNET m_hConnect;
    HINTERNET m_hRequest;
    DWORD m_dwSize;
    DWORD m_dwTotalSize;
    LPSTR m_lpBuffer;
    std::wstring m_version;

    static Linter m_linter;

    void LoadVersions(std::wstring const &fileSpec, Linter_Versions &versions);

    void CleanupContext(DownloadResult result);
    void DownloadOK();
    void DownloadNoNewVersion();
    void DownloadFailed();
    bool CheckVersion();
    static void CALLBACK AsyncCallback(
        HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
        LPVOID lpvStatusInformation, DWORD dwStatusInformationLength
    );
    void AsyncCallbackHandler(
        DWORD dwInternetStatus, LPVOID lpvStatusInformation,
        DWORD dwStatusInformationLength
    );
    void StartDownload(Download_Progress_Bar *);
    static INT_PTR CALLBACK JSLintDownloadProgressDlgProc(
        HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam
    );
};
