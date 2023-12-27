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

#include "JSLint.h"

#include "DownloadJSLint.h"
#include "JSLintNpp.h"
#include "Settings.h"
#include "Util.h"

#include <libplatform/libplatform.h>
#include <v8.h>

using namespace v8;

////////////////////////////////////////////////////////////////////////////////

bool JSLintReportItem::IsReasonUndefVar(JSLintNpp const *plugin) const
{
    return ! GetUndefVar(plugin).empty();
}

std::wstring JSLintReportItem::GetUndefVar(JSLintNpp const *plugin) const
{
    std::wstring var;
    if (m_type == LINT_TYPE_ERROR)
    {
        ScriptSourceDef const &scriptSource =
            plugin->get_settings()->GetScriptSource(
                plugin->get_options()->GetSelectedLinter()
            );

        std::wstring errMsg =
            scriptSource.m_scriptSource == SCRIPT_SOURCE_DOWNLOADED
                && scriptSource.m_bSpecUndefVarErrMsg
            ? scriptSource.m_undefVarErrMsg
            : scriptSource.GetDefaultUndefVarErrMsg();

        std::wstring::size_type i = errMsg.find(L"%s");
        if (i != std::wstring::npos)
        {
            int nAfter = errMsg.size() - (i + 2);
            if (m_strReason.substr(0, i) == errMsg.substr(0, i)
                && m_strReason.substr(m_strReason.size() - nAfter)
                    == errMsg.substr(i + 2))
            {
                var = m_strReason.substr(i, m_strReason.size() - nAfter - i);
            }
        }
    }
    return var;
}

////////////////////////////////////////////////////////////////////////////////

/*
void fatalErrorHandler(const char* location, const char* message)
{
}

void messageListener(Handle<Message> message, Handle<Value> data)
{
    Local<String> msg = message->Get();
    String::AsciiValue str(msg);
    const char* sz = *str;
    const char* a = sz;
    int line = message->GetLineNumber();
    int sc = message->GetStartColumn();
    int ec = message->GetEndColumn();
}
*/

class V8_Instance
{
  public:
    V8_Instance()
    {
        // FIXME Remove hardcoded path
        V8::InitializeICUDefaultLocation(
            "C:\\Program Files\\Notepad++\\notepad++.exe",
            "C:\\Program Files\\Notepad++\\plugins\\JSLintNpp\\icudtl.dat"
        );
        platform_ = ::v8::platform::NewDefaultPlatform();
        V8::InitializePlatform(platform_.get());
        V8::Initialize();
    }

    ~V8_Instance()
    {
        V8::Dispose();
        V8::DisposePlatform();
        // A note: When you are debugging, notepad++ will crash or hang at this
        // point (when deleting platform_). It's OK when running normally.
    }

  private:
    std::unique_ptr<::v8::Platform> platform_;
};

std::unique_ptr<V8_Instance> v8_instance;

JSLint::JSLint(JSLintNpp const *plugin) : plugin_(plugin)
{
}

void JSLint::CheckScript(
    std::string const &strOptions, std::string const &strScript,
    int nppTabWidth, int jsLintTabWidth, std::list<JSLintReportItem> &items
)
{
    // V8::SetFatalErrorHandler(fatalErrorHandler);
    // V8::AddMessageListener(messageListener);

    // bug fix:
    // In presence of WebEdit plugin
    // (https://sourceforge.net/projects/notepad-plus/forums/forum/482781/topic/4875421)
    // JSLint crashes below when executing Context::New(). V8 expects that fp
    // divide by zero doesn't throw exception, but in presence of WebEdit it
    // does. So, here we are setting fp control word to the default (no fp
    // exceptions are thrown).
    unsigned int cw = _controlfp(0, 0);    // Get the default fp control word
    unsigned int cwOriginal = _controlfp(cw, MCW_EM);

    if (! v8_instance)
    {
        v8_instance = std::make_unique<V8_Instance>();
    }

    // Create a new Isolate and make it the current one.
    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
        ArrayBuffer::Allocator::NewDefaultAllocator();
    Isolate *isolate = Isolate::New(create_params);

    {
        Isolate::Scope isolate_scope(isolate);

        // Create a stack-allocated handle scope.
        HandleScope handle_scope(isolate);

        // Create a new context.
        Local<Context> context = Context::New(isolate);

        // Enter the context for compiling and running the hello world script.
        Context::Scope context_scope(context);

        ScriptSourceDef const &scriptSource =
            plugin_->get_settings()->GetScriptSource(
                plugin_->get_options()->GetSelectedLinter()
            );

        std::string strJSLintScript;
        if (scriptSource.m_scriptSource == SCRIPT_SOURCE_BUILTIN)
        {
            strJSLintScript = LoadCustomDataResource(
                plugin_->module(),
                MAKEINTRESOURCE(scriptSource.GetScriptResourceID()),
                L"JS"
            );
        }
        else
        {
            strJSLintScript =
                plugin_->get_downloader()
                    ->GetVersion(
                        scriptSource.m_linter, scriptSource.m_scriptVersion
                    )
                    .GetContent();
        }

        if (strJSLintScript.empty())
        {
            throw JSLintException("Invalid JSLint script!");
        }

        TryCatch catcher(isolate);

        Local<Script> script;
        Script::Compile(
            context,
            String::NewFromUtf8(isolate, strJSLintScript.c_str())
                .ToLocalChecked()
        )
            .ToLocal(&script);

        if (script.IsEmpty())
        {
            String::Utf8Value error(isolate, catcher.Exception());
            throw JSLintException(
                (std::string("Invalid JSLint script!: ") + *error).c_str()
            );
        }
        script->Run(context);

        // init script variable
        context->Global()->Set(
            context,
            String::NewFromUtf8Literal(isolate, "script"),
            String::NewFromUtf8(isolate, strScript.c_str()).ToLocalChecked()
        );

        // init options variable
        Script::Compile(
            context,
            String::NewFromUtf8(isolate, ("options = " + strOptions).c_str())
                .ToLocalChecked()
        )
            .ToLocal(&script);
        if (script.IsEmpty())
        {
            throw JSLintException(
                "Invalid JSLint options (probably error in additional options)!"
            );
        }
        script->Run(context);

        // call JSLINT
        Script::Compile(
            context,
            String::NewFromUtf8(
                isolate,
                (std::string(scriptSource.GetNamespace()) + "(script, options);"
                )
                    .c_str()
            )
                .ToLocalChecked()
        )
            .ToLocal(&script);
        if (script.IsEmpty())
        {
            throw JSLintUnexpectedException();
        }
        script->Run(context);

        // get JSLINT data
        Script::Compile(
            context,
            String::NewFromUtf8(
                isolate,
                (std::string(scriptSource.GetNamespace()) + ".data();").c_str()
            )
                .ToLocalChecked()
        )
            .ToLocal(&script);
        if (script.IsEmpty())
        {
            throw JSLintUnexpectedException();
        }

        Handle<Object> data =
            script->Run(context).ToLocalChecked().As<Object>();

        // read errors
        Handle<Object> errors;
        data->Get(context, String::NewFromUtf8Literal(isolate, "errors"))
            .ToLocalChecked()
            ->ToObject(context)
            .ToLocal(&errors);
        if (! errors.IsEmpty())
        {
            int32_t length =
                errors
                    ->Get(
                        context, String::NewFromUtf8Literal(isolate, "length")
                    )
                    .ToLocalChecked()
                    ->Int32Value(context)
                    .ToChecked();
            for (int32_t i = 0; i < length; ++i)
            {
                Local<Value> eVal = errors->Get(context, Int32::New(isolate, i))
                                        .ToLocalChecked();
                if (eVal->IsObject())
                {
                    Local<Object> e = eVal->ToObject(context).ToLocalChecked();

                    int line = e->Get(
                                    context,
                                    String::NewFromUtf8Literal(isolate, "line")
                    )
                                   .ToLocalChecked()
                                   ->Int32Value(context)
                                   .ToChecked();
                    int character =
                        e->Get(
                             context,
                             String::NewFromUtf8Literal(isolate, "character")
                        )
                            .ToLocalChecked()
                            ->Int32Value(context)
                            .ToChecked();
                    String::Utf8Value reason(
                        isolate,
                        e->Get(
                             context,
                             String::NewFromUtf8Literal(isolate, "reason")
                        )
                            .ToLocalChecked()
                    );
                    String::Utf8Value evidence(
                        isolate,
                        e->Get(
                             context,
                             String::NewFromUtf8Literal(isolate, "evidence")
                        )
                            .ToLocalChecked()
                    );

                    // adjust character position if there is a difference
                    // in tab width between Notepad++ and JSLint
                    if (nppTabWidth != jsLintTabWidth)
                    {
                        character +=
                            GetNumTabs(
                                strScript, line, character, jsLintTabWidth
                            )
                            * (nppTabWidth - jsLintTabWidth);
                    }

                    items.push_back(JSLintReportItem(
                        JSLintReportItem::LINT_TYPE_ERROR,
                        line - 1,
                        character - 1,
                        TextConversion::UTF8_To_T(*reason),
                        TextConversion::UTF8_To_T(*evidence)
                    ));
                }
            }
        }

        // read unused
        Handle<Object> unused;
        data->Get(context, String::NewFromUtf8Literal(isolate, "unused"))
            .ToLocalChecked()
            ->ToObject(context)
            .ToLocal(&unused);
        if (! unused.IsEmpty())
        {
            int32_t length =
                unused
                    ->Get(
                        context, String::NewFromUtf8Literal(isolate, "length")
                    )
                    .ToLocalChecked()
                    ->Int32Value(context)
                    .ToChecked();
            for (int32_t i = 0; i < length; ++i)
            {
                Handle<Value> eVal =
                    unused->Get(context, Int32::New(isolate, i))
                        .ToLocalChecked();
                if (eVal->IsObject())
                {
                    Handle<Object> e = eVal->ToObject(context).ToLocalChecked();

                    int line = e->Get(
                                    context,
                                    String::NewFromUtf8Literal(isolate, "line")
                    )
                                   .ToLocalChecked()
                                   ->Int32Value(context)
                                   .ToChecked();
                    String::Utf8Value name(
                        isolate,
                        e->Get(
                             context,
                             String::NewFromUtf8Literal(isolate, "name")
                        )
                            .ToLocalChecked()
                    );
                    String::Utf8Value function(
                        isolate,
                        e->Get(
                             context,
                             String::NewFromUtf8Literal(isolate, "function")
                        )
                            .ToLocalChecked()
                    );

                    std::wstring reason = L"'"
                        + TextConversion::UTF8_To_T(*name) + L"' in '"
                        + TextConversion::UTF8_To_T(*function) + L"'";

                    items.push_back(JSLintReportItem(
                        JSLintReportItem::LINT_TYPE_UNUSED,
                        line - 1,
                        0,
                        TextConversion::UTF8_To_T(*name),
                        TextConversion::UTF8_To_T(*function)
                    ));
                }
            }
        }
    }

    // FIXME If we throw an exception this doesn't get done and bad things
    // happen Dispose the isolate and tear down V8.
    isolate->Dispose();
    delete create_params.array_buffer_allocator;

    _controlfp(cwOriginal, MCW_EM);    // Restore the original gp control world
}

std::string JSLint::LoadCustomDataResource(
    HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType
)
{
    HRSRC hRes = FindResource(hModule, lpName, lpType);
    if (hRes == NULL)
    {
        throw JSLintResourceException();
    }

    DWORD dwSize = SizeofResource(hModule, hRes);
    if (dwSize == 0)
    {
        throw JSLintResourceException();
    }

    HGLOBAL hResLoad = LoadResource(hModule, hRes);
    if (hResLoad == NULL)
    {
        throw JSLintResourceException();
    }

    LPVOID pData = LockResource(hResLoad);
    if (pData == NULL)
    {
        throw JSLintResourceException();
    }

    return std::string((char const *)pData, dwSize);
}

int JSLint::GetNumTabs(
    std::string const &strScript, int line, int character, int tabWidth
)
{
    int numTabs = 0;

    size_t i = 0;

    while (line-- > 0)
    {
        i = strScript.find('\n', i) + 1;
    }

    while (character > 0)
    {
        if (i < strScript.length() && strScript[i++] == '\t')
        {
            ++numTabs;
            character -= tabWidth;
        }
        else
        {
            character--;
        }
    }

    return numTabs;
}
