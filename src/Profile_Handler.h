#pragma once

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

#include <list>
#include <map>
#include <string>

class Profile_Handler
{
  public:
    Profile_Handler(std::wstring const &);

    ~Profile_Handler();

    /** Check if a key exists in a section */
    bool has_key(std::wstring const &section, std::wstring const &key) const;

    /** Get the value of a key in a section.
     *
     * This returns the supplied default value if the key doesn't exist in the
     * section.
     */
    int get_int_value(
        std::wstring const &section, std::wstring const &key, int def_value = 0
    ) const;

    /** Get the value of a key in a section.
     *
     * This returns the supplied default value if the key doesn't exist in the
     * section.
     */
    std::wstring get_str_value(
        std::wstring const &section, std::wstring const &key,
        std::wstring const &def_value = L""
    ) const;

    /** Set the value of a key in a section. */
    void set_str_value(
        std::wstring const &section, std::wstring const &key,
        std::wstring const &value
    );

    // JSLint specific
    /** Get the build version */
    int get_build_version() const
    {
        return get_int_value(JSLint_Group_Name, JSLint_Build_Key);
    }

    // JSLint specific
    static constexpr auto Min_Version_Build = 110;

  private:
    std::wstring profile_file_;
    std::map<std::wstring, std::map<std::wstring, std::wstring>> values_;

    std::list<std::wstring> get_section_list(
        wchar_t const *section, wchar_t *pointer
    ) const;

    // JSLint specific
    static constexpr auto JSLint_Group_Name = L"JSLint";
    // JSLint specific
    static constexpr auto JSLint_Build_Key = L"build";
};
