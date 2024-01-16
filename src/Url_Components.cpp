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

#include "Url_Components.h"

#include <winhttp.h>

#include <stdexcept>

Url_Components::Url_Components(wchar_t const *url) :
    url_components_(std::make_unique<URL_COMPONENTS>())
{
    url_components_->dwStructSize = sizeof(URL_COMPONENTS);
    url_components_->lpszHostName = &hostname_[0];
    url_components_->dwHostNameLength =
        sizeof(hostname_) / sizeof(hostname_[0]);
    url_components_->dwUrlPathLength = -1;
    url_components_->dwSchemeLength = -1;
    if (WinHttpCrackUrl(url, 0, 0, url_components_.get()) == 0)
    {
        throw std::runtime_error("Failed to decode URL");
    }
}

Url_Components::~Url_Components()
{
}
