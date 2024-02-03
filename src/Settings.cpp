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

#include "Settings.h"

#include "JSLintNpp.h"
#include "Linter.h"
#include "Settings_Dialogue.h"

Settings::Settings(
    JSLintNpp const *plugin, Profile_Handler *profile_handler,
    DownloadJSLint *downloader
) :
    plugin_(plugin),
    downloader_(downloader),
    m_jsLintScript(Linter::LINTER_JSLINT, profile_handler),
    m_jsHintScript(Linter::LINTER_JSHINT, profile_handler)
{
}

void Settings::ShowDialog()
{
    //FIXME make it return ok, etc and provide a method to get a copy of the contents
    Settings_Dialogue dlg(plugin_, this, downloader_);
    auto res = dlg.get_result();
    if (res > 0)
    {
        *this = *reinterpret_cast<Settings *>(res);
    }
}

ScriptSourceDef const &Settings::GetScriptSource(Linter linter) const
{
    return linter == Linter::LINTER_JSLINT ? m_jsLintScript : m_jsHintScript;
}
