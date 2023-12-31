#include "StdHeaders.h"

#include "ScriptSourceDef.h"

#include "Util.h"
#include "Version.h"

#include "resource.h"

#include <Profile_Handler.h>
#include <tchar.h>

// #define MIN_VERSION_BUILD 110

// These are used in JSLintOptions.cpp for no obvious reason
// Should probably do something about both these and Linter enum

//#define PROFILE_JSLINT_GROUP_NAME L"JSLint"
//#define PROFILE_BUILD_KEY_NAME L"build"

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

ScriptSourceDef::ScriptSourceDef(
    Linter linter, Profile_Handler *profile_handler
) :
    m_linter(linter),
    profile_handler_(profile_handler),
    m_scriptSource(SCRIPT_SOURCE_BUILTIN),
    m_bSpecUndefVarErrMsg(false),
    m_undefVarErrMsg(GetDefaultUndefVarErrMsg())
{
    if (profile_handler_->get_build_version()
        < Profile_Handler::Min_Version_Build)
    {
        return;
    }

    if (profile_handler_->get_build_version() >= VERSION_BUILD)
    {
        if (get_settings_value(PROFILE_SCRIPT_SOURCE_KEY_NAME)
            == PROFILE_SCRIPT_SOURCE_DOWNLOADED)
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

    m_scriptVersion = get_settings_value(PROFILE_SCRIPT_VERSION_KEY_NAME);

    m_bSpecUndefVarErrMsg =
        get_settings_value(PROFILE_SPEC_UNDEF_VAR_ERR_MSG_KEY_NAME) == L"true";

    m_undefVarErrMsg = get_settings_value(
        PROFILE_UNDEF_VAR_ERR_MSG_KEY_NAME, GetDefaultUndefVarErrMsg()
    );
}

ScriptSourceDef::~ScriptSourceDef()
{
    set_settings_value(
        PROFILE_SCRIPT_SOURCE_KEY_NAME,
        m_scriptSource == SCRIPT_SOURCE_BUILTIN
            ? PROFILE_SCRIPT_SOURCE_BUILTIN
            : PROFILE_SCRIPT_SOURCE_DOWNLOADED
    );

    set_settings_value(
        PROFILE_SCRIPT_VERSION_KEY_NAME, m_scriptVersion.c_str()
    );

    set_settings_value(
        PROFILE_SPEC_UNDEF_VAR_ERR_MSG_KEY_NAME,
        m_bSpecUndefVarErrMsg ? L"true" : L"false"
    );

    set_settings_value(
        PROFILE_UNDEF_VAR_ERR_MSG_KEY_NAME, m_undefVarErrMsg.c_str()
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

std::wstring ScriptSourceDef::get_settings_value(
    std::wstring const &key, std::wstring const &def_value
) const
{
    return profile_handler_->get_str_value(
        PROFILE_SETTINGS_GROUP_NAME, prefix() + key, def_value
    );
}

void ScriptSourceDef::set_settings_value(
    std::wstring const &key, std::wstring const &value
) const
{
    profile_handler_->set_str_value(
        PROFILE_SETTINGS_GROUP_NAME, prefix() + key, value
    );
}
