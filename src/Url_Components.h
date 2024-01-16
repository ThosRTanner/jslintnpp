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

#include <memory>

// Forward includes from windows headers
typedef struct _WINHTTP_URL_COMPONENTS URL_COMPONENTS;

class Url_Components
{
  public:
    Url_Components(wchar_t const *url);

    ~Url_Components();

    URL_COMPONENTS const *get() const
    {
        return url_components_.get();
    }

    wchar_t const *get_hostname()
    {
        return &hostname_[0];
    }

  private:
    std::unique_ptr<URL_COMPONENTS> url_components_;
    wchar_t hostname_[256];
};
