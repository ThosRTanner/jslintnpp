#pragma once

#include "JSLintOptions.h"

#include <string>

enum ScriptSource
{
    SCRIPT_SOURCE_BUILTIN,
    SCRIPT_SOURCE_DOWNLOADED
};

struct ScriptSourceDef
{
  public:
    explicit ScriptSourceDef(Linter linter);

    void ReadOptions(std::wstring const &config_file);

    void SaveOptions(std::wstring const &config_file) const;

  public:    // FIXME Should be private
    Linter m_linter;

    ScriptSource m_scriptSource;
    std::wstring m_scriptVersion;
    bool m_bSpecUndefVarErrMsg;
    std::wstring m_undefVarErrMsg;

    int GetScriptResourceID() const noexcept;
    LPCTSTR GetDefaultUndefVarErrMsg() const noexcept;
    LPCSTR GetNamespace() const noexcept;

  private:
    std::wstring prefix() const;
};
