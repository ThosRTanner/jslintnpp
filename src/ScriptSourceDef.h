#pragma once

#include "JSLintOptions.h"

#include <string>

class Profile_Handler;

enum ScriptSource
{
    SCRIPT_SOURCE_BUILTIN,
    SCRIPT_SOURCE_DOWNLOADED
};

class ScriptSourceDef
{
  public:
    ScriptSourceDef(Linter linter, Profile_Handler *profile_handler);

    ~ScriptSourceDef();

    LPCSTR GetNamespace() const noexcept;
    int GetScriptResourceID() const noexcept;

  //FIXME everything from here should be private.
  public:    
    Linter m_linter;

  private:
    Profile_Handler *profile_handler_;

  public:
    ScriptSource m_scriptSource;
    std::wstring m_scriptVersion;
    bool m_bSpecUndefVarErrMsg;
    std::wstring m_undefVarErrMsg;

  public:
    LPCTSTR GetDefaultUndefVarErrMsg() const noexcept;

  private:
    std::wstring prefix() const;
    std::wstring get_settings_value(
        std::wstring const &key, std::wstring const &def_value = L""
    ) const;
    void set_settings_value(
        std::wstring const &key, std::wstring const &value
    ) const;
};
