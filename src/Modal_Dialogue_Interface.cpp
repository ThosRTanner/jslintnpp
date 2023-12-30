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

#include "Modal_Dialogue_Interface.h"

#include "Plugin/Plugin.h"    //Fudgstickling

#include "DockingFeature/Docking.h"
#include "DockingFeature/dockingResource.h"
#include "Notepad_plus_msgs.h"

#include <comutil.h>
#include <ShlwApi.h>
#include <WinUser.h>

#include <stdexcept>
#include <system_error>

#ifdef _DEBUG
#pragma comment(lib, "comsuppwd.lib")
#else
#pragma comment(lib, "comsuppw.lib")
#endif
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shlwapi.lib")

namespace
{
std::wstring get_module_name(HINSTANCE module_instance)
{
    TCHAR temp[MAX_PATH] = {0};
    ::GetModuleFileName(module_instance, &temp[0], MAX_PATH);
    return ::PathFindFileName(&temp[0]);
}

std::wstring get_dialogue_name(HWND dialog_handle)
{
    TCHAR temp[MAX_PATH] = {0};
    ::GetWindowText(dialog_handle, &temp[0], MAX_PATH);
    return &temp[0];
}

}    // namespace

Modal_Dialogue_Interface::Modal_Dialogue_Interface(Plugin const *plugin) :
    plugin_(plugin),
    module_name_(get_module_name(plugin_->module()))
{
}

Modal_Dialogue_Interface::~Modal_Dialogue_Interface()
{
    // Stop process_dialogue_message from doing anything, since it calls a
    // virtual method which won't be there.
    //    ::SetWindowLongPtr(dialogue_window_, GWLP_USERDATA, NULL);
    //    send_dialogue_info(NPPM_MODELESSDIALOG, MODELESSDIALOGREMOVE);
    //    ::DestroyWindow(dialogue_window_);
}

void Modal_Dialogue_Interface::InvalidateRect(RECT const *rect) const noexcept
{
    ::InvalidateRect(dialogue_window_, rect, TRUE);
}

RECT Modal_Dialogue_Interface::getClientRect() const noexcept
{
    RECT rc;
    ::GetClientRect(dialogue_window_, &rc);
    return rc;
}

RECT Modal_Dialogue_Interface::getWindowRect() const noexcept
{
    RECT rc;
    ::GetWindowRect(dialogue_window_, &rc);
    return rc;
}

RECT Modal_Dialogue_Interface::getParentRect() const noexcept
{
    RECT rc;
    ::GetWindowRect(plugin_->get_notepad_window(), &rc);
    return rc;
}

HWND Modal_Dialogue_Interface::GetDlgItem(int item) const noexcept
{
    return ::GetDlgItem(dialogue_window_, item);
}

BOOL Modal_Dialogue_Interface::EndDialog(INT_PTR retval) const noexcept
{
    return ::EndDialog(dialogue_window_, retval);
}

BOOL Modal_Dialogue_Interface::centre_dialogue() const noexcept
{
    RECT const rect = getWindowRect();
    int const width = rect.right - rect.left;
    int const height = rect.bottom - rect.top;

    RECT const rect_npp = getParentRect();
    int const x = ((rect_npp.right - rect_npp.left) - width) / 2 + rect_npp.left;
    int const y = ((rect_npp.bottom - rect_npp.top) - height) / 2 + rect_npp.top;

    return ::MoveWindow(dialogue_window_, x, y, width, height, TRUE);
}

int Modal_Dialogue_Interface::message_box(
    std::wstring const &message, UINT type
) const noexcept
{
    return ::MessageBox(
        dialogue_window_, message.c_str(), dialogue_name_.c_str(), type
    );
}

std::optional<LONG_PTR> Modal_Dialogue_Interface::on_dialogue_message(
    UINT message, UINT_PTR wParam, LONG_PTR lParam
)
{
    return std::nullopt;
}

INT_PTR __stdcall Modal_Dialogue_Interface::process_dialogue_message(
    HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam
) noexcept
{
    if (message == WM_INITDIALOG)
    {
        auto instance = reinterpret_cast<Modal_Dialogue_Interface *>(lParam);
        instance->dialogue_window_ = window_handle;
        instance->dialogue_name_ = get_dialogue_name(window_handle);

        ::SetWindowLongPtr(
            window_handle,
            GWLP_USERDATA,
#pragma warning(suppress : 26490)
            static_cast<LONG_PTR>(lParam)
        );
    }
#pragma warning(suppress : 26490)
    auto instance = reinterpret_cast<Modal_Dialogue_Interface *>(
        ::GetWindowLongPtr(window_handle, GWLP_USERDATA)
    );
    if (instance == nullptr)
    {
        return FALSE;
    }

    try
    {
        auto retval = instance->on_dialogue_message(message, wParam, lParam);
        if (retval)
        {
            SetWindowLongPtr(window_handle, DWLP_MSGRESULT, *retval);
        }
        return static_cast<bool>(retval);
    }
    catch (std::exception const &e)
    {
        try
        {
            instance->message_box(
                static_cast<wchar_t *>(static_cast<_bstr_t>(e.what())),
                MB_OK | MB_ICONERROR
            );
        }
        catch (std::exception const &)
        {
            instance->message_box(
                L"Caught exception but cannot get reason", MB_OK | MB_ICONERROR
            );
        }
        return FALSE;
    }
}

void Modal_Dialogue_Interface::create_dialogue_window(int dialogID)
{
    HWND focus = ::GetFocus();
    result_ = ::DialogBoxParam(
        plugin_->module(),
        MAKEINTRESOURCE(dialogID),
        plugin_->get_notepad_window(),
        process_dialogue_message,
        reinterpret_cast<LPARAM>(this)
    );
    ::SetFocus(focus);
}
