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

#pragma once

////////////////////////////////////////////////////////////////////////////////

#include <list>
#include <string>

class JSLintNpp;

////////////////////////////////////////////////////////////////////////////////

#define JSLINT_DEFAULT_UNDEF_VAR_ERR_MSG \
    L"'%s' was used before it was defined."
#define JSHINT_DEFAULT_UNDEF_VAR_ERR_MSG L"'%s' is not defined."

////////////////////////////////////////////////////////////////////////////////

class JSLintReportItem
{
  public:
    enum Type
    {
        LINT_TYPE_ERROR,
        LINT_TYPE_UNUSED
    };

    JSLintReportItem(
        Type type, int line, int character, std::wstring const &strReason,
        std::wstring const &strEvidence
    ) :
        m_type(type),
        m_line(line),
        m_character(character),
        m_strReason(strReason),
        m_strEvidence(strEvidence)
    {
    }

    Type GetType() const
    {
        return m_type;
    }
    int GetLine() const
    {
        return m_line;
    }
    int GetCharacter() const
    {
        return m_character;
    }
    std::wstring GetReason() const
    {
        return m_strReason;
    }
    std::wstring GetEvidence() const
    {
        return m_strEvidence;
    }

    bool IsReasonUndefVar(JSLintNpp const *plugin) const;
    std::wstring GetUndefVar(JSLintNpp const *plugin) const;

  private:
    Type m_type;
    int m_line;
    int m_character;
    std::wstring m_strReason;
    std::wstring m_strEvidence;
};

////////////////////////////////////////////////////////////////////////////////

class JSLint
{
  public:
    JSLint(JSLintNpp const *);

    void CheckScript(
        std::string const &strOptions, std::string const &strScript,
        int nppTabWidth, int jsLintTabWidth, std::list<JSLintReportItem> &items
    );

  private:
    JSLintNpp const *plugin_;
    std::string LoadCustomDataResource(
        HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType
    );

    int GetNumTabs(
        std::string const &strScript, int line, int character, int tabWidth
    );
};

////////////////////////////////////////////////////////////////////////////////

class JSLintException : public std::exception
{
  public:
    JSLintException(char const *what) : std::exception(what)
    {
    }
};

class JSLintResourceException : public JSLintException
{
  public:
    JSLintResourceException() :
        JSLintException("Failed to load JSLINT script from resource!")
    {
    }
};

class JSLintUnexpectedException : public JSLintException
{
  public:
    JSLintUnexpectedException() :
        JSLintException("Unexpected error while running JSLINT script!")
    {
    }
};
