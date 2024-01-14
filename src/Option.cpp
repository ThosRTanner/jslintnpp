#include "Option.h"

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

Option::Option() : type(OPTION_TYPE_UNKNOWN)
{
}

Option::Option(std::wstring const &name) :
    type(OPTION_TYPE_BOOL),
    name(name),
    value(L""),
    defaultValue(L"")
{
}

Option::Option(
    OptionType type, std::wstring const &name, std::wstring const &value
) :
    type(type),
    name(name),
    value(value),
    defaultValue(value)
{
}
