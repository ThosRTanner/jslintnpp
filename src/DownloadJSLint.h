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

#pragma once

#include "JSLintOptions.h"

#include <winhttp.h>

#include <map>
#include <string>

////////////////////////////////////////////////////////////////////////////////

class JSLintVersion {
public:
    JSLintVersion() {}
    JSLintVersion(const std::wstring& fileName) : m_fileName(fileName) {}
    JSLintVersion(const std::wstring& fileName, const std::string& content) 
        : m_fileName(fileName), m_content(content) {}

    std::wstring GetFileName() const { return m_fileName; }
    std::string GetContent();

private:
    std::wstring m_fileName;
    std::string m_content;
};

////////////////////////////////////////////////////////////////////////////////

class DownloadJSLint
{
    DownloadJSLint();

public:
    static DownloadJSLint& GetInstance();

    void LoadVersions();
    
    enum DownloadResult {
        DOWNLOAD_OK,
        DOWNLOAD_NO_NEW_VERSION,
        DOWNLOAD_FAILED
    };
    DownloadResult DownloadLatest(Linter linter, std::wstring& latestVersion);

    const std::map<std::wstring, JSLintVersion>& GetVersions(Linter linter) const;
    bool HasVersion(Linter linter, const std::wstring& version);
    JSLintVersion& GetVersion(Linter linter, const std::wstring& version);

private:
    std::wstring m_versionsFolder;
    std::map<std::wstring, JSLintVersion> m_jsLintVersions;
    std::map<std::wstring, JSLintVersion> m_jsHintVersions;

    HWND m_hDlg;
    int m_nStatusID;
    HINTERNET m_hSession;
    HINTERNET m_hConnect;
    HINTERNET m_hRequest;
    DWORD m_dwSize;
    DWORD m_dwTotalSize;
    LPSTR m_lpBuffer;
    std::wstring m_version;
    DownloadResult m_result;

    static Linter m_linter;

    void LoadVersions(const std::wstring& fileSpec, std::map<std::wstring, JSLintVersion>& versions);

    std::wstring GetVersionsFolder();

    void CleanupContext();
    void DownloadOK();
    void DownloadNoNewVersion();
    void DownloadFailed();
    bool CheckVersion();
    static void CALLBACK AsyncCallback(HINTERNET hInternet,
        DWORD_PTR dwContext,
        DWORD dwInternetStatus,
        LPVOID lpvStatusInformation,
        DWORD dwStatusInformationLength);
    void AsyncCallbackHandler(DWORD dwInternetStatus,
        LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
    void StartDownload(HWND hDlg, int nStatusID);
    static INT_PTR CALLBACK JSLintDownloadProgressDlgProc(HWND hDlg, UINT uMessage,
        WPARAM wParam, LPARAM lParam);
};
