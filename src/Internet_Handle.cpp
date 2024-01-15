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

#include "Internet_Handle.h"

#include <winhttp.h>
#include <stdexcept>

Internet_Handle::Internet_Handle(HINTERNET handle) : handle_(handle)
{
    if (handle == nullptr)
    {
        throw std::runtime_error("Internet socket connection failure");
    }
}

Internet_Handle::~Internet_Handle()
{
    WinHttpCloseHandle(handle_);
}
