/*
 * Copyright (C) 2010, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2010-2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InspectorDebuggerAgent_h
#define InspectorDebuggerAgent_h

#if ENABLE(JAVASCRIPT_DEBUGGER) && ENABLE(INSPECTOR)
#include "BreakpointID.h"
#include "ConsoleAPITypes.h"
#include "ConsoleTypes.h"
#include "InjectedScript.h"
#include "InspectorBaseAgent.h"
#include "InspectorFrontend.h"
#include "ScriptBreakpoint.h"
#include "ScriptDebugListener.h"
#include "ScriptState.h"
#include "SourceID.h"
#include <wtf/Forward.h>
#include <wtf/HashMap.h>
#include <wtf/PassOwnPtr.h>
#include <wtf/PassRefPtr.h>
#include <wtf/Vector.h>
#include <wtf/text/StringHash.h>

namespace WebCore {

class InjectedScriptManager;
class InspectorArray;
class InspectorObject;
class InspectorValue;
class InstrumentingAgents;
class ScriptDebugServer;
class ScriptValue;

typedef String ErrorString;

class InspectorDebuggerAgent : public InspectorBaseAgent, public ScriptDebugListener, public InspectorDebuggerBackendDispatcherHandler {
    WTF_MAKE_NONCOPYABLE(InspectorDebuggerAgent); WTF_MAKE_FAST_ALLOCATED;
public:
    static const char* backtraceObjectGroup;

    virtual ~InspectorDebuggerAgent();

    virtual void causesRecompilation(ErrorString*, bool*);
    virtual void canSetScriptSource(ErrorString*, bool*);
    virtual void supportsSeparateScriptCompilationAndExecution(ErrorString*, bool*);

    virtual void didCreateFrontendAndBackend(InspectorFrontendChannel*, InspectorBackendDispatcher*) OVERRIDE;
    virtual void willDestroyFrontendAndBackend() OVERRIDE;

    bool isPaused();
    bool runningNestedMessageLoop();
    void addMessageToConsole(MessageSource, MessageType);

    // Part of the protocol.
    virtual void enable(ErrorString*);
    virtual void disable(ErrorString*);
    virtual void setBreakpointsActive(ErrorString*, bool active);

    virtual void setBreakpointByUrl(ErrorString*, int lineNumber, const String* optionalURL, const String* optionalURLRegex, const int* optionalColumnNumber, const RefPtr<InspectorObject>* options, TypeBuilder::Debugger::BreakpointId*, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::Location>>& locations);
    virtual void setBreakpoint(ErrorString*, const RefPtr<InspectorObject>& location, const RefPtr<InspectorObject>* options, TypeBuilder::Debugger::BreakpointId*, RefPtr<TypeBuilder::Debugger::Location>& actualLocation);
    virtual void removeBreakpoint(ErrorString*, const String& breakpointIdentifier);
    virtual void continueToLocation(ErrorString*, const RefPtr<InspectorObject>& location);

    virtual void searchInContent(ErrorString*, const String& scriptID, const String& query, const bool* optionalCaseSensitive, const bool* optionalIsRegex, RefPtr<TypeBuilder::Array<TypeBuilder::Page::SearchMatch>>&);
    virtual void setScriptSource(ErrorString*, const String& scriptID, const String& newContent, const bool* preview, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::CallFrame>>& newCallFrames, RefPtr<InspectorObject>& result);
    virtual void getScriptSource(ErrorString*, const String& scriptID, String* scriptSource);
    virtual void getFunctionDetails(ErrorString*, const String& functionId, RefPtr<TypeBuilder::Debugger::FunctionDetails>&);
    virtual void pause(ErrorString*);
    virtual void resume(ErrorString*);
    virtual void stepOver(ErrorString*);
    virtual void stepInto(ErrorString*);
    virtual void stepOut(ErrorString*);
    virtual void setPauseOnExceptions(ErrorString*, const String& pauseState);
    virtual void evaluateOnCallFrame(ErrorString*,
                             const String& callFrameId,
                             const String& expression,
                             const String* objectGroup,
                             const bool* includeCommandLineAPI,
                             const bool* doNotPauseOnExceptionsAndMuteConsole,
                             const bool* returnByValue,
                             const bool* generatePreview,
                             RefPtr<TypeBuilder::Runtime::RemoteObject>& result,
                             TypeBuilder::OptOutput<bool>* wasThrown);
    void compileScript(ErrorString*, const String& expression, const String& sourceURL, TypeBuilder::OptOutput<TypeBuilder::Debugger::ScriptId>*, TypeBuilder::OptOutput<String>* syntaxErrorMessage);
    void runScript(ErrorString*, const TypeBuilder::Debugger::ScriptId&, const int* executionContextId, const String* objectGroup, const bool* doNotPauseOnExceptionsAndMuteConsole, RefPtr<TypeBuilder::Runtime::RemoteObject>& result, TypeBuilder::OptOutput<bool>* wasThrown);
    virtual void setOverlayMessage(ErrorString*, const String*);

    void schedulePauseOnNextStatement(InspectorDebuggerFrontendDispatcher::Reason::Enum breakReason, PassRefPtr<InspectorObject> data);
    void cancelPauseOnNextStatement();
    void breakProgram(InspectorDebuggerFrontendDispatcher::Reason::Enum breakReason, PassRefPtr<InspectorObject> data);
    virtual void scriptExecutionBlockedByCSP(const String& directiveText);

    class Listener {
    public:
        virtual ~Listener() { }
        virtual void debuggerWasEnabled() = 0;
        virtual void debuggerWasDisabled() = 0;
        virtual void stepInto() = 0;
        virtual void didPause() = 0;
    };
    void setListener(Listener* listener) { m_listener = listener; }

    virtual ScriptDebugServer& scriptDebugServer() = 0;

protected:
    InspectorDebuggerAgent(InstrumentingAgents*, InjectedScriptManager*);

    virtual void startListeningScriptDebugServer() = 0;
    virtual void stopListeningScriptDebugServer() = 0;
    virtual void muteConsole() = 0;
    virtual void unmuteConsole() = 0;
    InjectedScriptManager* injectedScriptManager() { return m_injectedScriptManager; }
    virtual InjectedScript injectedScriptForEval(ErrorString*, const int* executionContextId) = 0;

    virtual void enable();
    virtual void disable();
    virtual void didPause(JSC::ExecState*, const ScriptValue& callFrames, const ScriptValue& exception);
    virtual void didContinue();
    void reset();

private:
    bool enabled() const { return m_enabled; };

    PassRefPtr<TypeBuilder::Array<TypeBuilder::Debugger::CallFrame>> currentCallFrames();

    virtual void didParseSource(SourceID, const Script&) OVERRIDE FINAL;
    virtual void failedToParseSource(const String& url, const String& data, int firstLine, int errorLine, const String& errorMessage) OVERRIDE FINAL;

    void setPauseOnExceptionsImpl(ErrorString*, int);

    PassRefPtr<TypeBuilder::Debugger::Location> resolveBreakpoint(const String& breakpointIdentifier, SourceID, const ScriptBreakpoint&);
    void clear();
    bool assertPaused(ErrorString*);
    void clearBreakDetails();

    String sourceMapURLForScript(const Script&);

    typedef HashMap<SourceID, Script> ScriptsMap;
    typedef HashMap<String, Vector<BreakpointID>> BreakpointIdentifierToDebugServerBreakpointIDsMap;
    typedef HashMap<String, RefPtr<InspectorObject>> BreakpointIdentifierToBreakpointMap;

    InjectedScriptManager* m_injectedScriptManager;
    std::unique_ptr<InspectorDebuggerFrontendDispatcher> m_frontendDispatcher;
    RefPtr<InspectorDebuggerBackendDispatcher> m_backendDispatcher;
    JSC::ExecState* m_pausedScriptState;
    ScriptValue m_currentCallStack;
    ScriptsMap m_scripts;
    BreakpointIdentifierToDebugServerBreakpointIDsMap m_breakpointIdentifierToDebugServerBreakpointIDs;
    BreakpointIdentifierToBreakpointMap m_javaScriptBreakpoints;
    BreakpointID m_continueToLocationBreakpointID;
    InspectorDebuggerFrontendDispatcher::Reason::Enum m_breakReason;
    RefPtr<InspectorObject> m_breakAuxData;
    bool m_enabled;
    bool m_javaScriptPauseScheduled;
    Listener* m_listener;
};

} // namespace WebCore

#endif // ENABLE(JAVASCRIPT_DEBUGGER) && ENABLE(INSPECTOR)

#endif // !defined(InspectorDebuggerAgent_h)
