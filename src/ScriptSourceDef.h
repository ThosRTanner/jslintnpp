#pragma once

#include "JSLintOptions.h"

#include <string>

class Profile_Handler;

enum ScriptSource
{
    SCRIPT_SOURCE_BUILTIN,
    SCRIPT_SOURCE_DOWNLOADED
};

class Settings_Dialogue;

class ScriptSourceDef
{
  public:
    ScriptSourceDef(Linter linter, Profile_Handler *profile_handler);

    ~ScriptSourceDef();

    LPCSTR GetNamespace() const noexcept;
    int GetScriptResourceID() const noexcept;
    std::wstring get_undef_errmsg() const;

  public:
    Linter m_linter;

  private:
    Profile_Handler *profile_handler_;

  public:
    ScriptSource m_scriptSource;
    std::wstring m_scriptVersion;

  private:
    bool m_bSpecUndefVarErrMsg;
    std::wstring m_undefVarErrMsg;

    LPCTSTR GetDefaultUndefVarErrMsg() const noexcept;

    std::wstring prefix() const;

    std::wstring get_settings_value(
        std::wstring const &key, std::wstring const &def_value = L""
    ) const;

    void set_settings_value(std::wstring const &key, std::wstring const &value)
        const;

    friend class Settings_Dialogue;
};
