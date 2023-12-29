// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <basetsd.h>

#include <optional>
#include <string>

// Forward declarations from windows headers
typedef struct tagRECT RECT;
typedef struct HWND__ *HWND;
typedef struct HICON__ *HICON;
typedef unsigned int UINT;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef int BOOL;

// Forward declarations.
class Plugin;

/** This provides an abstraction for creating a docking dialogue. */
class Modal_Dialogue_Interface
{
  public:
    /** Create a modal dialogue. */
    Modal_Dialogue_Interface(Plugin const *plugin);

    Modal_Dialogue_Interface(Modal_Dialogue_Interface const &) = delete;
    Modal_Dialogue_Interface(Modal_Dialogue_Interface &&) = delete;
    Modal_Dialogue_Interface &operator=(Modal_Dialogue_Interface const &) =
        delete;
    Modal_Dialogue_Interface &operator=(Modal_Dialogue_Interface &&) = delete;

    virtual ~Modal_Dialogue_Interface() = 0;

    auto get_result() const noexcept
    {
        return result_;
    }

  protected:
    /** You need to call this from your constructor */
    void create_dialogue_window(int dialogID);

    /** Get hold of plugin object for useful boilerplate */
    Plugin const *plugin() const noexcept
    {
        return plugin_;
    }

    /** Get hold of the current dialogue window handle */
    HWND window() const noexcept
    {
        return dialogue_window_;
    }

    /** Requests a redraw */
    void InvalidateRect(RECT const *rect = nullptr) const noexcept;

    /** Utility to get the current client rectangle */
    RECT getClientRect() const noexcept;

    /** Utility to get the current window rectangle */
    RECT getWindowRect() const noexcept;

    /** Utility to get the parent window rectangle */
    RECT getParentRect() const noexcept;

    /** Utility to get a dialogue item */
    HWND GetDlgItem(int) const noexcept;

    BOOL EndDialog(INT_PTR retval) const noexcept;

    /** Centre the dialogue on the Notepad++ window */ 
    BOOL centre_dialogue() const noexcept;

    /** Throw up a message box
     *
     * The title will be the same as the docking dialogue title.
     *
     * This would take a string_view, but there's no guarantee that that is null
     * terminated.
     */
    int message_box(std::wstring const &message, UINT type) const noexcept;

  private:
    /** Implement this to handle messages.
     *
     * Return std::nullopt to (to return FALSE to windows dialog processing), or
     * a value to be set with SetWindowLongPtr (in which case TRUE will be
     * returned). Note that some messages require you to return FALSE
     * (std::nullopt) even if you do handle them.
     *
     * If you don't handle the message, you MUST call the base class version of
     * this.
     *
     * message, wParam and lParam are the values passed to
     * process_dialogue_message by windows
     */
    virtual std::optional<LONG_PTR> on_dialogue_message(
        UINT message, UINT_PTR wParam, LONG_PTR lParam
    ) noexcept(false);

    /** Callback handler for messages */
    static INT_PTR __stdcall process_dialogue_message(
        HWND, UINT message, WPARAM, LPARAM
    ) noexcept;

    Plugin const *plugin_;
    HWND dialogue_window_;
    std::wstring module_name_;
    std::wstring dialogue_name_;

    INT_PTR result_;
};
