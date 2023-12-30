#pragma once

#include "JSLintOptions.h"
#include "Modal_Dialogue_Interface.h"

#include <memory>

class JSLintNpp;
class Settings;

class Settings_Dialogue : public Modal_Dialogue_Interface
{
  public:
    Settings_Dialogue(JSLintNpp const *plugin, Settings *settings);

    ~Settings_Dialogue();

  private:
    JSLintNpp const *plugin_;
    std::unique_ptr<Settings> settings_;

    std::optional<LONG_PTR> on_dialogue_message(
        UINT message, UINT_PTR wParam, LONG_PTR lParam
    ) override;

    void LoadVersions(int versionsComboBoxID, Linter linter);
    void display_options() const noexcept;
    BOOL update_options() noexcept;

    void UpdateControls();

    UINT IsDlgButtonChecked(int button) const noexcept;

    BOOL CheckDlgButton(int button, UINT check) const noexcept;

    BOOL CheckRadioButton(int first_button, int last_button, int check_button)
        const noexcept;
};
