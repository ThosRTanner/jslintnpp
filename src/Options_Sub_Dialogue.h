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

#include "Plugin/Non_Modal_Dialogue_Interface.h"

class Linter_Options;
class Options_Dialogue;

class Options_Sub_Dialogue : public Non_Modal_Dialogue_Interface
{
  public:
    Options_Sub_Dialogue(
        int dialogue_id, Plugin const *plugin, Options_Dialogue const *parent, Linter_Options *options
    );

    ~Options_Sub_Dialogue();

    void show();

    void hide();

  private:
    std::optional<LONG_PTR> on_dialogue_message(
        UINT message, WPARAM wParam, LPARAM lParam
    ) noexcept override;

    Options_Dialogue const *parent_;
    Linter_Options *options_;
};
