// This file is part of JSLint Plugin for Notepad++
// Copyright (C) 2010 Martin Vladic <martin.vladic@gmail.com>
//
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

#include "StdHeaders.h"

#include "Linter_Options.h"

#include "Profile_Handler.h"
#include "Util.h"

#include "resource.h"

#include <tchar.h>
#include <windowsx.h>

////////////////////////////////////////////////////////////////////////////////

#define PROFILE_ADDITIONAL_OPTIONS_KEY_NAME L"jslintnpp_additional_options"

////////////////////////////////////////////////////////////////////////////////

Linter_Options::Linter_Options(
    LPCTSTR optionsGroupName, Profile_Handler *profile_handler

) :
    m_optionsGroupName(optionsGroupName),
    profile_handler_(profile_handler)
{
    m_options[IDC_PREDEFINED] = Option(OPTION_TYPE_ARR_STRING, L"predef", L"");
}

void Linter_Options::ReadOptions()
{
    if (profile_handler_ == nullptr)
    {
        return;
    }

    if (profile_handler_->get_build_version()
        < profile_handler_->Min_Version_Build)
    {
        return;
    }
    // This is moderately annoying as we have to know what the options are so we
    // currently have to call this after construction.
    //
    // Also the handling of non specified options is passing weird, in so far as
    // e.g. if a value is empty, it's considered as not being set and ignored,
    // but if it's spaces, it's considered to be valid. And bools have 3 states
    // - yes, no, unset..
    for (auto &option : m_options)
    {
        if (not profile_handler_->has_key(
                m_optionsGroupName, option.second.name
            ))
        {
            continue;
        }
        auto const strValue = TrimSpaces(profile_handler_->get_str_value(
            m_optionsGroupName, option.second.name
        ));
        switch (option.second.type)
        {
            case OPTION_TYPE_BOOL:
                if (strValue == L"true" || strValue == L"false"
                    || strValue.empty())
                {
                    option.second.value = strValue;
                }
                break;

            case OPTION_TYPE_INT:
            {
                int value;
                if (_stscanf(strValue.c_str(), L"%d", &value) != EOF
                    && value > 0)
                {
                    option.second.value = strValue;
                }
                break;
            }

            case OPTION_TYPE_ARR_STRING:
                option.second.value = strValue;
                break;
        }
    }

    m_additionalOptions = profile_handler_->get_str_value(
        m_optionsGroupName, PROFILE_ADDITIONAL_OPTIONS_KEY_NAME
    );
}

void Linter_Options::SaveOptions()
{
    for (auto const &option : m_options)
    {
        profile_handler_->set_str_value(
            m_optionsGroupName, option.second.name, option.second.value
        );
    }

    profile_handler_->set_str_value(
        m_optionsGroupName,
        PROFILE_ADDITIONAL_OPTIONS_KEY_NAME,
        m_additionalOptions
    );
}

bool Linter_Options::IsOptionIncluded(Option const &option) const
{
    return ! option.value.empty();
}

std::wstring Linter_Options::GetOptionsCommentString() const
{
    std::wstring strOptions;

    for (auto const &option : m_options)
    {
        if (IsOptionIncluded(option.second))
        {
            if (option.first != IDC_PREDEFINED)
            {
                if (! strOptions.empty())
                {
                    strOptions += L", ";
                }
                strOptions += option.second.name + L": " + option.second.value;
            }
        }
    }

    return strOptions;
}

std::wstring Linter_Options::GetOptionsJSONString() const
{
    std::wstring strOptions;

    for (auto const &option : m_options)
    {
        if (not IsOptionIncluded(option.second))
        {
            continue;
        }

        std::wstring value;

        if (option.second.type == OPTION_TYPE_ARR_STRING)
        {
            std::vector<std::wstring> arr;
            StringSplit(option.second.value, L",", arr);
            std::vector<std::wstring>::const_iterator itArr;
            for (itArr = arr.begin(); itArr != arr.end(); ++itArr)
            {
                if (value.empty())
                {
                    value += L"[";
                }
                else
                {
                    value += L", ";
                }

                std::wstring element = TrimSpaces(*itArr);
                FindReplace(element, L"\\", L"\\\\");
                FindReplace(element, L"\"", L"\\\"");

                value += L"\"" + element + L"\"";
            }
            if (! value.empty())
            {
                value += L"]";
            }
        }
        else
        {
            value = option.second.value;
        }

        if (! value.empty())
        {
            if (! strOptions.empty())
            {
                strOptions += L", ";
            }
            strOptions += option.second.name + L": " + value;
        }
    }

    if (! m_additionalOptions.empty())
    {
        if (! strOptions.empty())
        {
            strOptions += L", ";
        }
        strOptions += m_additionalOptions;
    }

    return L"{ " + strOptions + L" }";
}

void Linter_Options::CheckOption(UINT id)
{
    m_options[id].value = L"true";
}

void Linter_Options::UncheckOption(UINT id)
{
    m_options[id].value = L"false";
}

void Linter_Options::ClearOption(UINT id)
{
    m_options[id].value = L"";
}

void Linter_Options::SetOption(UINT id, std::wstring const &value)
{
    m_options[id].value = value;
}

void Linter_Options::AppendOption(UINT id, std::wstring const &value)
{
    Option &option = m_options[id];
    if (option.value.empty())
    {
        option.value = value;
    }
    else
    {
        option.value += L", " + value;
    }
}

void Linter_Options::ResetOption(UINT id)
{
    m_options[id].value = m_options[id].defaultValue;
}

void Linter_Options::SetAdditionalOptions(std::wstring const &additionalOptions)
{
    m_additionalOptions = additionalOptions;
}

void Linter_Options::ClearAllOptions()
{
    for (auto &option : m_options)
    {
        if (option.first != IDC_PREDEFINED)
        {
            option.second.value = option.second.defaultValue;
        }
    }
}

BOOL Linter_Options::UpdateOptions(
    HWND hDlg, HWND hSubDlg, bool bSaveOrValidate, bool bShowErrorMessage
)
{
    ////AARRGGGHH
    if (bSaveOrValidate)
    {
        for (auto const &option : m_options)
        {
            if (option.second.type == OPTION_TYPE_BOOL)
            {
                int checkState =
                    Button_GetCheck(GetDlgItem(hSubDlg, option.first));
                if (checkState == BST_UNCHECKED)
                {
                    UncheckOption(option.first);
                }
                else if (checkState == BST_CHECKED)
                {
                    CheckOption(option.first);
                }
                else
                {
                    ClearOption(option.first);
                }
            }
        }

        // predefined
        std::wstring strPredefined =
            TrimSpaces(GetWindowText(GetDlgItem(hDlg, IDC_PREDEFINED)));
        if (strPredefined.empty())
        {
            ResetOption(IDC_PREDEFINED);
        }
        else
        {
            SetOption(IDC_PREDEFINED, strPredefined);
        }

        // additional options
        std::wstring strAdditionalOptions =
            TrimSpaces(GetWindowText(GetDlgItem(hDlg, IDC_ADDITIONAL_OPTIONS)));
        SetAdditionalOptions(strAdditionalOptions);
    }
    else
    {
        std::map<UINT, Option>::iterator it;
        for (it = m_options.begin(); it != m_options.end(); ++it)
        {
            if (it->second.type == OPTION_TYPE_BOOL)
            {
                int checkState;
                if (it->second.value == L"false")
                {
                    checkState = BST_UNCHECKED;
                }
                else if (it->second.value == L"true")
                {
                    checkState = BST_CHECKED;
                }
                else
                {
                    checkState = BST_INDETERMINATE;
                }
                Button_SetCheck(GetDlgItem(hSubDlg, it->first), checkState);
            }
            else if (it->second.type == OPTION_TYPE_INT || it->second.type == OPTION_TYPE_ARR_STRING)
            {
                if (GetDlgItem(hDlg, it->first))
                {
                    SetWindowText(
                        GetDlgItem(hDlg, it->first), it->second.value.c_str()
                    );
                }
                else
                {
                    SetWindowText(
                        GetDlgItem(hSubDlg, it->first), it->second.value.c_str()
                    );
                }
            }
        }

        SetWindowText(
            GetDlgItem(hDlg, IDC_ADDITIONAL_OPTIONS),
            m_additionalOptions.c_str()
        );
    }

    return TRUE;
}