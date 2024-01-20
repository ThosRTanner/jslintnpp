#pragma once

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

#include "Plugin/Modal_Dialogue_Interface.h"

#include "Linter.h"

#include <map>
#include <memory>

class JSLintNpp;
class JSLintOptions;
class Options_Sub_Dialogue;

class Options_Dialogue : public Modal_Dialogue_Interface
{
  public:
    Options_Dialogue(JSLintNpp const *);

    ~Options_Dialogue();

    auto window() const noexcept
    {
        return Modal_Dialogue_Interface::window();
    }

  private:
    std::optional<LONG_PTR> on_dialogue_message(
        UINT message, WPARAM wParam, LPARAM lParam
    ) noexcept override;

    std::optional<LONG_PTR> on_command(WPARAM wParam) noexcept;

    std::unique_ptr<JSLintOptions> options_;
    std::map<Linter, std::unique_ptr<Options_Sub_Dialogue>> sub_dialogues_;
    Linter current_linter_;
};
