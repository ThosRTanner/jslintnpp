#include "StdHeaders.h"

#include "Downloader.h"

#include "Download_Progress_Bar.h"
#include "Linter.h"
#include "Util.h"

#include <winhttp.h>

#include <stdexcept>

#pragma comment(lib, "winhttp.lib")

namespace
{
auto open_session()
{
    auto session = WinHttpOpen(
        L"JSLint Plugin for Notepad++",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC
    );
    if (session == nullptr)
    {
        throw std::runtime_error("Failed to create http session");
    }
    return session;
}
}    // namespace

Downloader::Downloader(
    Download_Progress_Bar *progress_bar, wchar_t const *url, Linter linter
) :
    progress_bar_(progress_bar),
    linter_(linter),
    url_components_(url),
    session_(open_session()),
    connect_(open_connection()),
    request_(open_request())
{
    if (WinHttpSetStatusCallback(
            request_,
            winhttp_callback,
            WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS
                | WINHTTP_CALLBACK_FLAG_REDIRECT,
            0
        )
        != nullptr)
    {
        DownloadFailed();
        return;
    }

    if (! WinHttpSendRequest(
            request_,
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

Downloader::~Downloader()
{
    WinHttpSetStatusCallback(request_, nullptr, 0, 0);
}

HINTERNET Downloader::open_connection()
{
    return WinHttpConnect(
        session_,
        url_components_.get_hostname(),
        url_components_.get()->nPort,
        0
    );
}

HINTERNET Downloader::open_request()
{
    return WinHttpOpenRequest(
        connect_,
        L"GET",
        url_components_.get()->lpszUrlPath,
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        (url_components_.get()->nScheme == INTERNET_SCHEME_HTTPS)
            ? WINHTTP_FLAG_SECURE
            : 0
    );
}

void Downloader::cleanup_context(DownloadResult result)
{
    progress_bar_->completed(result);
}

void CALLBACK Downloader::winhttp_callback(
    HINTERNET, DWORD_PTR context, DWORD status, LPVOID information, DWORD length
)
{
    reinterpret_cast<Downloader *>(context)->winhttp_callback_handler(
        status, information, length
    );
}

void Downloader::winhttp_callback_handler(
    DWORD status, LPVOID information, DWORD length
)
{
    switch (status)
    {
        case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
            if (! WinHttpReceiveResponse(request_, nullptr))
            {
                DownloadFailed();
            }
            break;

        case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
            if (! WinHttpQueryDataAvailable(request_, nullptr))
            {
                DownloadFailed();
            }
            break;

        case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
        {
            auto size = *((LPDWORD)information);

            if (size == 0)
            {
                if (out_buffer_.size() == 0)
                {
                    DownloadFailed();
                }
                else
                {
                    DownloadOK();
                }
            }
            else
            {
                out_buffer_.resize(total_size_ + size);
                if (! WinHttpReadData(
                        request_, &out_buffer_[total_size_], size, nullptr
                    ))
                {
                    DownloadFailed();
                }
            }
            break;
        }

        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
            if (length != 0)
            {
                // So we've allocated the data to read and not got there.
                // Update where we've got to.
                total_size_ += length;
                progress_bar_->update(total_size_);

                if (already_have_version())
                {
                    DownloadNoNewVersion();
                }
                else
                {
                    if (! WinHttpQueryDataAvailable(request_, nullptr))
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

bool Downloader::already_have_version()
{
    if (not version_.empty())
    {
        // If we know the version and haven't baled out already, we need to
        // carry on downloading.
        return false;
    }

    std::string const match_start{
        linter_ == Linter::LINTER_JSLINT ? "\nlet jslint_edition = \"v" : "/*! "
    };
    std::string const match_end{
        linter_ == Linter::LINTER_JSLINT ? "\";\n" : " */\n"
    };
    std::string const code{out_buffer_.begin(), out_buffer_.end()};
    auto pos = code.find(match_start);
    if (pos == std::string::npos)
    {
        // Don't know what the version is so we probably don't have it.
        return false;
    }
    pos += match_start.length();
    auto pos2 = code.find(match_end, pos);
    if (pos2 == std::string::npos)
    {
        // Nope, still don't know.
        return false;
    }
    version_ = TextConversion::A_To_T(code.substr(pos, pos2 - pos));
    // FIXME!
    return false;
    // return not HasVersion(linter_, _version);
}

Url_Components::Url_Components(wchar_t const *url) :
    url_components_(std::make_unique<URL_COMPONENTS>())
{
    url_components_->dwStructSize = sizeof(URL_COMPONENTS);
    url_components_->lpszHostName = &hostname_[0];
    url_components_->dwHostNameLength =
        sizeof(hostname_) / sizeof(hostname_[0]);
    url_components_->dwUrlPathLength = -1;
    url_components_->dwSchemeLength = -1;
    if (WinHttpCrackUrl(url, 0, 0, url_components_.get()) == 0)
    {
        throw std::runtime_error("Failed to decode URL");
    }
}

Url_Components::~Url_Components()
{
}
