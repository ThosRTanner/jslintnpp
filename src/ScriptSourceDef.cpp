#include "StdHeaders.h"

#include "ScriptSourceDef.h"

#include "Util.h"
#include "Version.h"

#include "resource.h"

#include <tchar.h>

#define MIN_VERSION_BUILD 110

// These are used in JSLintOptions.cpp for no obvious reason
// Should probably do something about both these and Linter enum

#define PROFILE_JSLINT_GROUP_NAME L"JSLint"
#define PROFILE_BUILD_KEY_NAME L"build"

/////////^^^^

#define PROFILE_SETTINGS_GROUP_NAME L"Settings"
#define PROFILE_SCRIPT_SOURCE_KEY_NAME L"_script_source"
#define PROFILE_SCRIPT_VERSION_KEY_NAME L"_script_version"
#define PROFILE_SPEC_UNDEF_VAR_ERR_MSG_KEY_NAME L"_spec_undef_var_err_msg"
#define PROFILE_UNDEF_VAR_ERR_MSG_KEY_NAME L"_undef_var_err_msg"

#define PROFILE_SCRIPT_SOURCE_BUILTIN L"builtin"
#define PROFILE_SCRIPT_SOURCE_DOWNLOADED L"downloaded"

#define JSLINT_DEFAULT_UNDEF_VAR_ERR_MSG L"'%s' was used before it was defined."
#define JSHINT_DEFAULT_UNDEF_VAR_ERR_MSG L"'%s' is not defined."

ScriptSourceDef::ScriptSourceDef(Linter linter) :
    m_linter(linter),
    m_scriptSource(SCRIPT_SOURCE_BUILTIN),
    m_bSpecUndefVarErrMsg(false),
    m_undefVarErrMsg(GetDefaultUndefVarErrMsg())
{
}

void ScriptSourceDef::ReadOptions(std::wstring const &strConfigFileName)
{
    if (! Path::IsFileExists(strConfigFileName))
    {
        return;
    }

    TCHAR szValue[65536];    // memory is cheap
    GetPrivateProfileString(
        PROFILE_JSLINT_GROUP_NAME,
        PROFILE_BUILD_KEY_NAME,
        nullptr,
        szValue,
        _countof(szValue),
        strConfigFileName.c_str()
    );
    if (_ttoi(szValue) < MIN_VERSION_BUILD)
    {
        return;
    }

    if (_ttoi(szValue) >= VERSION_BUILD)
    {
        GetPrivateProfileString(
            PROFILE_SETTINGS_GROUP_NAME,
            (prefix() + PROFILE_SCRIPT_SOURCE_KEY_NAME).c_str(),
            nullptr,
            szValue,
            _countof(szValue),
            strConfigFileName.c_str()
        );
        if (_tcscmp(szValue, PROFILE_SCRIPT_SOURCE_DOWNLOADED) == 0)
        {
            m_scriptSource = SCRIPT_SOURCE_DOWNLOADED;
        }
        else
        {
            m_scriptSource = SCRIPT_SOURCE_BUILTIN;
        }
    }
    else
    {
        // if old config switch to builtin source
        m_scriptSource = SCRIPT_SOURCE_BUILTIN;
    }

    GetPrivateProfileString(
        PROFILE_SETTINGS_GROUP_NAME,
        (prefix() + PROFILE_SCRIPT_VERSION_KEY_NAME).c_str(),
        nullptr,
        szValue,
        _countof(szValue),
        strConfigFileName.c_str()
    );
    m_scriptVersion = szValue;

    GetPrivateProfileString(
        PROFILE_SETTINGS_GROUP_NAME,
        (prefix() + PROFILE_SPEC_UNDEF_VAR_ERR_MSG_KEY_NAME).c_str(),
        nullptr,
        szValue,
        _countof(szValue),
        strConfigFileName.c_str()
    );
    m_bSpecUndefVarErrMsg = _tcscmp(szValue, L"true") == 0;

    GetPrivateProfileString(
        PROFILE_SETTINGS_GROUP_NAME,
        (prefix() + PROFILE_UNDEF_VAR_ERR_MSG_KEY_NAME).c_str(),
        nullptr,
        szValue,
        _countof(szValue),
        strConfigFileName.c_str()
    );
    if (_tcslen(szValue) > 0)
    {
        m_undefVarErrMsg = szValue;
    }
    else
    {
        m_undefVarErrMsg = GetDefaultUndefVarErrMsg();
    }
}

void ScriptSourceDef::SaveOptions(std::wstring const &strConfigFileName) const
{
    WritePrivateProfileString(
        PROFILE_JSLINT_GROUP_NAME,
        PROFILE_BUILD_KEY_NAME,
        STR(VERSION_BUILD),
        strConfigFileName.c_str()
    );

    WritePrivateProfileString(
        PROFILE_SETTINGS_GROUP_NAME,
        (prefix() + PROFILE_SCRIPT_SOURCE_KEY_NAME).c_str(),
        m_scriptSource == SCRIPT_SOURCE_BUILTIN
            ? PROFILE_SCRIPT_SOURCE_BUILTIN
            : PROFILE_SCRIPT_SOURCE_DOWNLOADED,
        strConfigFileName.c_str()
    );

    WritePrivateProfileString(
        PROFILE_SETTINGS_GROUP_NAME,
        (prefix() + PROFILE_SCRIPT_VERSION_KEY_NAME).c_str(),
        m_scriptVersion.c_str(),
        strConfigFileName.c_str()
    );

    WritePrivateProfileString(
        PROFILE_SETTINGS_GROUP_NAME,
        (prefix() + PROFILE_SPEC_UNDEF_VAR_ERR_MSG_KEY_NAME).c_str(),
        m_bSpecUndefVarErrMsg ? L"true" : L"false",
        strConfigFileName.c_str()
    );

    WritePrivateProfileString(
        PROFILE_SETTINGS_GROUP_NAME,
        (prefix() + PROFILE_UNDEF_VAR_ERR_MSG_KEY_NAME).c_str(),
        m_undefVarErrMsg.c_str(),
        strConfigFileName.c_str()
    );
}

int ScriptSourceDef::GetScriptResourceID() const noexcept
{
    return m_linter == LINTER_JSLINT ? IDR_JSLINT : IDR_JSHINT;
}

LPCTSTR ScriptSourceDef::GetDefaultUndefVarErrMsg() const noexcept
{
    return m_linter == LINTER_JSLINT ? JSLINT_DEFAULT_UNDEF_VAR_ERR_MSG
                                     : JSHINT_DEFAULT_UNDEF_VAR_ERR_MSG;
}

LPCSTR ScriptSourceDef::GetNamespace() const noexcept
{
    return m_linter == LINTER_JSLINT ? "JSLINT" : "JSHINT";
}

std::wstring ScriptSourceDef::prefix() const
{
    return m_linter == LINTER_JSLINT ? L"jslint" : L"jshint";
}
