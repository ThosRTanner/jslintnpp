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

#include <string>

enum OptionType
{
    OPTION_TYPE_UNKNOWN,
    OPTION_TYPE_BOOL,
    OPTION_TYPE_INT,
    OPTION_TYPE_ARR_STRING
};

struct Option
{
    Option();

    Option(std::wstring const &name);

    Option(
        OptionType type, std::wstring const &name, std::wstring const &value
    );

    OptionType type;
    std::wstring name;
    std::wstring value;
    std::wstring defaultValue;
};
