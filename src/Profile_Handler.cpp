#include "StdHeaders.h"

#include "Profile_Handler.h"

#include "Util.h"
#include "Version.h"

#include <tchar.h>

#include <list>
#include <map>
#include <memory>
#include <stdexcept>

constexpr auto Buff_Size = 65536UL;

Profile_Handler::Profile_Handler(std::wstring const &file) : profile_file_(file)
{
    if (! Path::IsFileExists(profile_file_))
    {
        return;
    }

    // Only or c++20
    // auto buff{std::make_unique_for_overwrite<wchar_t[]>(Buff_Size)};
    std::unique_ptr<wchar_t[]> buff{new wchar_t[Buff_Size]};

    // What we do here is extract all the keys and values so we can look them up
    // later. Cos we will use them all..

    // First we get all the section names

    std::list<std::wstring> sections{get_section_list(NULL, buff.get())};

    // Now we iterate through the sections reading the keys
    std::map<std::wstring, std::list<std::wstring>> keys_by_section;
    for (auto const &section : sections)
    {
        auto list{get_section_list(section.c_str(), buff.get())};
        keys_by_section[section] = list;
    }

    // and now we iterate through everything reading the values.
    for (auto const &section : sections)
    {
        for (auto const &key : keys_by_section[section])
        {
            auto const res = GetPrivateProfileString(
                section.c_str(),
                key.c_str(),
                nullptr,
                buff.get(),
                Buff_Size,
                profile_file_.c_str()
            );
            if (res >= Buff_Size - 1)
            {
                throw std::runtime_error("Profile string too long.");
            }
            std::wstring const value{buff.get()};
            values_[section][key] = value;
        }
    }
}

Profile_Handler::~Profile_Handler()
{
    //JSLint specific
    values_[JSLint_Group_Name][JSLint_Build_Key] = STR(VERSION_BUILD);

    for (auto const &section : values_)
    {
        for (auto const &key : section.second)
        {
            WritePrivateProfileString(
                section.first.c_str(),
                key.first.c_str(),
                key.second.c_str(),
                profile_file_.c_str()
            );
        }
    }
}

int Profile_Handler::get_int_value(
    std::wstring const &section, std::wstring const &key, int def_value
) const
{
    auto const &keys = values_.find(section);
    if (keys == values_.end())
    {
        return def_value;
    }
    auto const &value = keys->second.find(key);
    if (value == keys->second.end())
    {
        return def_value;
    }
    return _ttoi(value->second.c_str());
}

std::wstring Profile_Handler::get_str_value(
    std::wstring const &section, std::wstring const &key,
    std::wstring const &def_value
) const
{
    auto const &keys = values_.find(section);
    if (keys == values_.end())
    {
        return def_value;
    }
    auto const &value = keys->second.find(key);
    if (value == keys->second.end())
    {
        return def_value;
    }
    return value->second.c_str();
}

void Profile_Handler::set_str_value(
    std::wstring const &section, std::wstring const &key,
    std::wstring const &value
)
{
    values_[section][key] = value;
}

std::list<std::wstring> Profile_Handler::get_section_list(
    wchar_t const *section, wchar_t *pointer
) const
{
    std::list<std::wstring> list;
    auto res = GetPrivateProfileString(
        section, nullptr, nullptr, pointer, Buff_Size, profile_file_.c_str()
    );
    if (res >= Buff_Size - 1)
    {
        throw std::runtime_error("Too many keys in profile.");
    }

    while (*pointer != 0)
    {
        std::wstring const section{pointer};
        list.push_back(section);
        pointer += section.size() + 1;
    }
    return list;
}
