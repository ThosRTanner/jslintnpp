#include "StdHeaders.h"

#include "Download_Progress_Bar.h"

//#include "DownloadJSLint.h"
#include "Linter.h"

#include "resource.h"

#include <tchar.h>
#include <WinUser.h>

//#include <functional>

// FIXME Pass these as params maybe?
#define JSLINT_GITHUB_URL \
    L"https://raw.github.com/jslint-org/jslint/master/jslint.mjs"

#define JSHINT_GITHUB_URL \
    L"https://raw.github.com/jshint/jshint/master/dist/jshint.js"

#define WM_DOWNLOAD_FINISHED (WM_USER + 1)

Download_Progress_Bar::Download_Progress_Bar(
    Plugin const *plugin, Linter linter, Download_Func start_download
) :
    Modal_Dialogue_Interface(plugin),
    linter_(linter),
    start_download_(start_download)
{
    create_modal_dialogue(IDD_DOWNLOAD_PROGRESS);
}

Download_Progress_Bar::~Download_Progress_Bar() = default;

void Download_Progress_Bar::update(DWORD transferred)
{
    TCHAR szStatus[1024];
    _stprintf(szStatus, L"Received %d bytes", transferred);
    SetWindowText(GetDlgItem(IDC_PROGRESS), szStatus);
}

void Download_Progress_Bar::completed(int result)
{
    PostMessage(window(), WM_DOWNLOAD_FINISHED, result, 0);
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

            SetWindowText(
                GetDlgItem(IDC_URL),
                linter_ == Linter::LINTER_JSLINT ? JSLINT_GITHUB_URL
                                                 : JSHINT_GITHUB_URL
            );
            SetWindowText(GetDlgItem(IDC_PROGRESS), L"Starting ...");
            std::invoke(start_download_, this);
            centre_dialogue();
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
