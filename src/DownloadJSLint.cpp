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

#include "resource.h"

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "winhttp.lib")

// FIXME These 3 currently duplicated in progress bar.
//  This thinks it runs in node so it might not run any more. not sure.
#define JSLINT_GITHUB_URL \
    L"https://raw.github.com/jslint-org/jslint/master/jslint.mjs"

#define JSHINT_GITHUB_URL \
    L"https://raw.github.com/jshint/jshint/master/dist/jshint.js"

////////////////////////////////////////////////////////////////////////////////

Linter DownloadJSLint::m_linter;

DownloadJSLint::DownloadJSLint(JSLintNpp const *plugin) :
    plugin_(plugin),
    config_dir_(plugin_->GetConfigDir()),
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

DownloadJSLint::Linter_Versions const &DownloadJSLint::GetVersions(Linter linter
) const
{
    return linter == Linter::LINTER_JSLINT ? m_jsLintVersions
                                           : m_jsHintVersions;
}

bool DownloadJSLint::HasVersion(Linter linter, std::wstring const &version)
{
    if (linter == Linter::LINTER_JSLINT)
    {
        return m_jsLintVersions.find(version) != m_jsLintVersions.end();
    }
    else
    {
        return m_jsHintVersions.find(version) != m_jsHintVersions.end();
    }
}

Version_Info &DownloadJSLint::GetVersion(
    Linter linter, std::wstring const &version
)
{
    return linter == Linter::LINTER_JSLINT ? m_jsLintVersions[version]
                                           : m_jsHintVersions[version];
}

DownloadJSLint::DownloadResult DownloadJSLint::DownloadLatest(
    Linter linter, std::wstring &latestVersion
)
{
    m_linter = linter;
    Download_Progress_Bar progress_bar{
        plugin_,
        linter,
        [this](Download_Progress_Bar *progress_bar)
        {
            StartDownload(progress_bar);
        }
    };
    DownloadResult res = static_cast<DownloadResult>(progress_bar.get_result());
    if (res == DOWNLOAD_OK)
    {
        latestVersion = m_version;
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

        size_t nWritten = 0;

        FILE *fp = _tfopen(fileName.c_str(), L"wb+");
        if (fp != NULL)
        {
            nWritten = fwrite(m_lpBuffer, 1, m_dwTotalSize, fp);
            fclose(fp);
        }

        std::string content(m_lpBuffer, m_dwTotalSize);

        if (nWritten == m_dwTotalSize)
        {
            if (linter == Linter::LINTER_JSLINT)
            {
                m_jsLintVersions.insert(std::make_pair(
                    latestVersion, Version_Info(fileName, content)
                ));
            }
            else
            {
                m_jsHintVersions.insert(std::make_pair(
                    latestVersion, Version_Info(fileName, content)
                ));
            }
        }
        else
        {
            res = DOWNLOAD_FAILED;
        }
    }
    delete[] m_lpBuffer;
    m_lpBuffer = NULL;

    return res;
}

void DownloadJSLint::CleanupContext(DownloadResult result)
{
    if (m_hRequest)
    {
        WinHttpSetStatusCallback(m_hRequest, NULL, NULL, NULL);
        WinHttpCloseHandle(m_hRequest);
        m_hRequest = NULL;
    }

    if (m_hConnect)
    {
        WinHttpCloseHandle(m_hConnect);
        m_hConnect = NULL;
    }

    if (m_hSession != NULL)
    {
        WinHttpCloseHandle(m_hSession);
        m_hSession = NULL;
    }
    progress_bar_->completed(result);
}

void DownloadJSLint::DownloadOK()
{
    CleanupContext(DOWNLOAD_OK);
}

void DownloadJSLint::DownloadNoNewVersion()
{
    CleanupContext(DOWNLOAD_NO_NEW_VERSION);
}

void DownloadJSLint::DownloadFailed()
{
    CleanupContext(DOWNLOAD_FAILED);
}

bool DownloadJSLint::CheckVersion()
{
    if (! m_version.empty())
    {
        return true;
    }
    std::string const match_start{
        m_linter == Linter::LINTER_JSLINT ? "\nlet jslint_edition = \"v"
                                          : "/*! "
    };
    std::string const match_end{
        m_linter == Linter::LINTER_JSLINT ? "\";\n" : " */\n"
    };
    std::string const code{m_lpBuffer, m_dwTotalSize};
    auto pos = code.find(match_start);
    if (pos == std::string::npos)
    {
        return true;
    }
    pos += match_start.length();
    auto pos2 = code.find(match_end, pos);
    if (pos2 == std::string::npos)
    {
        return true;
    }
    m_version = TextConversion::A_To_T(code.substr(pos, pos2 - pos));
    return ! HasVersion(m_linter, m_version);
}

void CALLBACK DownloadJSLint::AsyncCallback(
    HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
    LPVOID lpvStatusInformation, DWORD dwStatusInformationLength
)
{
    reinterpret_cast<DownloadJSLint *>(dwContext)->AsyncCallbackHandler(
        dwInternetStatus, lpvStatusInformation, dwStatusInformationLength
    );
}

void DownloadJSLint::AsyncCallbackHandler(
    DWORD dwInternetStatus, LPVOID lpvStatusInformation,
    DWORD dwStatusInformationLength
)
{
    // Create a std::string that reflects the status flag.
    switch (dwInternetStatus)
    {
        case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
            if (! WinHttpReceiveResponse(m_hRequest, NULL))
            {
                DownloadFailed();
            }
            break;

        case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
            if (! WinHttpQueryDataAvailable(m_hRequest, NULL))
            {
                DownloadFailed();
            }
            break;

        case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
            m_dwSize = *((LPDWORD)lpvStatusInformation);

            if (m_dwSize == 0)
            {
                if (m_dwTotalSize)
                {
                    DownloadOK();
                }
                else
                {
                    DownloadFailed();
                }
            }
            else
            {
                LPSTR lpOutBuffer = new char[m_dwSize + 1];
                ZeroMemory(lpOutBuffer, m_dwSize + 1);
                if (! WinHttpReadData(
                        m_hRequest, (LPVOID)lpOutBuffer, m_dwSize, NULL
                    ))
                {
                    delete[] lpOutBuffer;
                    DownloadFailed();
                }
            }
            break;

        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
            if (dwStatusInformationLength != 0)
            {
                LPSTR lpReadBuffer = (LPSTR)lpvStatusInformation;
                DWORD dwBytesRead = dwStatusInformationLength;

                m_dwSize = dwBytesRead;

                if (! m_lpBuffer)
                {
                    m_lpBuffer = lpReadBuffer;
                }
                else
                {
                    LPSTR lpOldBuffer = m_lpBuffer;
                    m_lpBuffer = new char[m_dwTotalSize + m_dwSize];

                    memcpy(m_lpBuffer, lpOldBuffer, m_dwTotalSize);
                    memcpy(m_lpBuffer + m_dwTotalSize, lpReadBuffer, m_dwSize);

                    delete[] lpOldBuffer;
                    delete[] lpReadBuffer;
                }

                m_dwTotalSize += m_dwSize;

                progress_bar_->update(m_dwTotalSize);

                if (! CheckVersion())
                {
                    DownloadNoNewVersion();
                }
                else
                {
                    if (! WinHttpQueryDataAvailable(m_hRequest, NULL))
                    {
                        DownloadFailed();
                    }
                }
            }
            break;

        case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
            DownloadFailed();
            break;
    }
}

void DownloadJSLint::StartDownload(Download_Progress_Bar *progress_bar)
{
    progress_bar_ = progress_bar;
    m_hSession = 0;
    m_hConnect = 0;
    m_hRequest = 0;
    m_lpBuffer = NULL;
    m_dwSize = 0;
    m_dwTotalSize = 0;
    m_version = L"";

    m_hSession = WinHttpOpen(
        L"JSLint Plugin for Notepad++",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC
    );

    if (m_hSession == NULL)
    {
        DownloadFailed();
        return;
    }

    WCHAR szHost[256];
    URL_COMPONENTS urlComp;

    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.lpszHostName = szHost;
    urlComp.dwHostNameLength = sizeof(szHost) / sizeof(szHost[0]);
    urlComp.dwUrlPathLength = (DWORD)-1;
    urlComp.dwSchemeLength = (DWORD)-1;
    WinHttpCrackUrl(
        m_linter == Linter::LINTER_JSLINT ? JSLINT_GITHUB_URL
                                          : JSHINT_GITHUB_URL,
        0,
        0,
        &urlComp
    );

    m_hConnect = WinHttpConnect(m_hSession, szHost, urlComp.nPort, 0);

    DWORD dwOpenRequestFlag =
        (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;

    m_hRequest = WinHttpOpenRequest(
        m_hConnect,
        L"GET",
        urlComp.lpszUrlPath,
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        dwOpenRequestFlag
    );
    if (m_hRequest == NULL)
    {
        DownloadFailed();
        return;
    }

    WINHTTP_STATUS_CALLBACK pCallback = WinHttpSetStatusCallback(
        m_hRequest,
        (WINHTTP_STATUS_CALLBACK)AsyncCallback,
        WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_REDIRECT,
        0
    );
    if (pCallback != NULL)
    {
        DownloadFailed();
        return;
    }

    if (! WinHttpSendRequest(
            m_hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            reinterpret_cast<DWORD_PTR>(this)
        ))
    {
        DownloadFailed();
        return;
    }
}
