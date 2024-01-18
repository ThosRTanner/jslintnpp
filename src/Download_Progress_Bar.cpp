#include "StdHeaders.h"

#include "Download_Progress_Bar.h"

#include "Downloader.h"
#include "Linter.h"

#include "resource.h"

#include <tchar.h>
#include <WinUser.h>

#define JSLINT_GITHUB_URL \
    L"https://raw.github.com/jslint-org/jslint/master/jslint.mjs"

#define JSHINT_GITHUB_URL \
    L"https://raw.github.com/jshint/jshint/master/dist/jshint.js"

#define WM_DOWNLOAD_FINISHED (WM_USER + 1)

Download_Progress_Bar::Download_Progress_Bar(
    Plugin const *plugin, Linter linter, Linter_Versions const &versions
) :
    Modal_Dialogue_Interface(plugin),
    linter_(linter),
    versions_(versions)
{
    create_modal_dialogue(IDD_DOWNLOAD_PROGRESS);
}

Download_Progress_Bar::~Download_Progress_Bar() = default;

void Download_Progress_Bar::update(std::size_t transferred)
{
    TCHAR szStatus[1024];
    _stprintf(szStatus, L"Received %lld bytes", transferred);
    SetWindowText(GetDlgItem(IDC_PROGRESS), szStatus);
}

void Download_Progress_Bar::completed(int result)
{
    PostMessage(window(), WM_DOWNLOAD_FINISHED, result, 0);
}

std::vector<uint8_t> const &Download_Progress_Bar::data() const
{
    return downloader_->data();
}

std::wstring Download_Progress_Bar::version() const
{
    return downloader_->version();
}

std::optional<LONG_PTR> Download_Progress_Bar::on_dialogue_message(
    UINT message, WPARAM wParam, LPARAM lParam
)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            TCHAR szTitleFormat[100];
            GetWindowText(window(), szTitleFormat, _countof(szTitleFormat));

            TCHAR szTitle[100];
            _stprintf(
                szTitle,
                szTitleFormat,
                linter_ == Linter::LINTER_JSLINT ? L"JSLint" : L"JSHint"
            );
            SetWindowText(window(), szTitle);

            wchar_t const *const url = linter_ == Linter::LINTER_JSLINT
                ? JSLINT_GITHUB_URL
                : JSHINT_GITHUB_URL;

            SetWindowText(GetDlgItem(IDC_URL), url);
            SetWindowText(GetDlgItem(IDC_PROGRESS), L"Starting ...");
            centre_dialogue();

            try
            {
                downloader_ =
                    std::make_unique<Downloader>(this, url, linter_, versions_);
            }
            catch (std::exception const &)
            {
                EndDialog(Clicked_Close);
            }
            break;
        }

        case WM_DOWNLOAD_FINISHED:
            EndDialog(wParam);
            break;

        default:
            break;
    }
    return std::nullopt;
}
