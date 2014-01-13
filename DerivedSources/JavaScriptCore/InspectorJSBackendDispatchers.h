// File is generated by JavaScriptCore/inspector/scripts/CodeGeneratorInspector.py

// Copyright (c) 2013 Apple Inc. All Rights Reserved.
// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef InspectorJSBackendDispatchers_h
#define InspectorJSBackendDispatchers_h

#include "InspectorJSTypeBuilders.h"
#include <inspector/InspectorBackendDispatcher.h>
#include <wtf/PassRefPtr.h>
#include <wtf/text/WTFString.h>

namespace Inspector {

typedef String ErrorString;

#if ENABLE(JAVASCRIPT_DEBUGGER)
class JS_EXPORT_PRIVATE InspectorDebuggerBackendDispatcherHandler {
public:
    virtual void enable(ErrorString*) = 0;
    virtual void disable(ErrorString*) = 0;
    virtual void setBreakpointsActive(ErrorString*, bool in_active) = 0;
    virtual void setBreakpointByUrl(ErrorString*, int in_lineNumber, const String* in_url, const String* in_urlRegex, const int* in_columnNumber, const RefPtr<Inspector::InspectorObject>* in_options, Inspector::TypeBuilder::Debugger::BreakpointId* out_breakpointId, RefPtr<Inspector::TypeBuilder::Array<Inspector::TypeBuilder::Debugger::Location> >& out_locations) = 0;
    virtual void setBreakpoint(ErrorString*, const RefPtr<Inspector::InspectorObject>& in_location, const RefPtr<Inspector::InspectorObject>* in_options, Inspector::TypeBuilder::Debugger::BreakpointId* out_breakpointId, RefPtr<Inspector::TypeBuilder::Debugger::Location>& out_actualLocation) = 0;
    virtual void removeBreakpoint(ErrorString*, const String& in_breakpointId) = 0;
    virtual void continueToLocation(ErrorString*, const RefPtr<Inspector::InspectorObject>& in_location) = 0;
    virtual void stepOver(ErrorString*) = 0;
    virtual void stepInto(ErrorString*) = 0;
    virtual void stepOut(ErrorString*) = 0;
    virtual void pause(ErrorString*) = 0;
    virtual void resume(ErrorString*) = 0;
    virtual void searchInContent(ErrorString*, const String& in_scriptId, const String& in_query, const bool* in_caseSensitive, const bool* in_isRegex, RefPtr<Inspector::TypeBuilder::Array<Inspector::TypeBuilder::GenericTypes::SearchMatch> >& out_result) = 0;
    virtual void getScriptSource(ErrorString*, const String& in_scriptId, String* out_scriptSource) = 0;
    virtual void getFunctionDetails(ErrorString*, const String& in_functionId, RefPtr<Inspector::TypeBuilder::Debugger::FunctionDetails>& out_details) = 0;
    virtual void setPauseOnExceptions(ErrorString*, const String& in_state) = 0;
    virtual void evaluateOnCallFrame(ErrorString*, const String& in_callFrameId, const String& in_expression, const String* in_objectGroup, const bool* in_includeCommandLineAPI, const bool* in_doNotPauseOnExceptionsAndMuteConsole, const bool* in_returnByValue, const bool* in_generatePreview, RefPtr<Inspector::TypeBuilder::Runtime::RemoteObject>& out_result, Inspector::TypeBuilder::OptOutput<bool>* opt_out_wasThrown) = 0;
    virtual void setOverlayMessage(ErrorString*, const String* in_message) = 0;
protected:
    virtual ~InspectorDebuggerBackendDispatcherHandler();
};

#endif // ENABLE(JAVASCRIPT_DEBUGGER)
class JS_EXPORT_PRIVATE InspectorInspectorBackendDispatcherHandler {
public:
    virtual void enable(ErrorString*) = 0;
    virtual void disable(ErrorString*) = 0;
protected:
    virtual ~InspectorInspectorBackendDispatcherHandler();
};

class JS_EXPORT_PRIVATE InspectorRuntimeBackendDispatcherHandler {
public:
    virtual void parse(ErrorString*, const String& in_source, Inspector::TypeBuilder::Runtime::SyntaxErrorType::Enum* out_result, Inspector::TypeBuilder::OptOutput<String>* opt_out_message, RefPtr<Inspector::TypeBuilder::Runtime::ErrorRange>& opt_out_range) = 0;
    virtual void evaluate(ErrorString*, const String& in_expression, const String* in_objectGroup, const bool* in_includeCommandLineAPI, const bool* in_doNotPauseOnExceptionsAndMuteConsole, const int* in_contextId, const bool* in_returnByValue, const bool* in_generatePreview, RefPtr<Inspector::TypeBuilder::Runtime::RemoteObject>& out_result, Inspector::TypeBuilder::OptOutput<bool>* opt_out_wasThrown) = 0;
    virtual void callFunctionOn(ErrorString*, const String& in_objectId, const String& in_functionDeclaration, const RefPtr<Inspector::InspectorArray>* in_arguments, const bool* in_doNotPauseOnExceptionsAndMuteConsole, const bool* in_returnByValue, const bool* in_generatePreview, RefPtr<Inspector::TypeBuilder::Runtime::RemoteObject>& out_result, Inspector::TypeBuilder::OptOutput<bool>* opt_out_wasThrown) = 0;
    virtual void getProperties(ErrorString*, const String& in_objectId, const bool* in_ownProperties, RefPtr<Inspector::TypeBuilder::Array<Inspector::TypeBuilder::Runtime::PropertyDescriptor> >& out_result, RefPtr<Inspector::TypeBuilder::Array<Inspector::TypeBuilder::Runtime::InternalPropertyDescriptor> >& opt_out_internalProperties) = 0;
    virtual void releaseObject(ErrorString*, const String& in_objectId) = 0;
    virtual void releaseObjectGroup(ErrorString*, const String& in_objectGroup) = 0;
    virtual void run(ErrorString*) = 0;
    virtual void enable(ErrorString*) = 0;
    virtual void disable(ErrorString*) = 0;
protected:
    virtual ~InspectorRuntimeBackendDispatcherHandler();
};



#if ENABLE(JAVASCRIPT_DEBUGGER)
class JS_EXPORT_PRIVATE InspectorDebuggerBackendDispatcher FINAL : public Inspector::InspectorSupplementalBackendDispatcher {
public:
    static PassRefPtr<InspectorDebuggerBackendDispatcher> create(Inspector::InspectorBackendDispatcher*, InspectorDebuggerBackendDispatcherHandler*);
    virtual void dispatch(long callId, const String& method, PassRefPtr<Inspector::InspectorObject> message) OVERRIDE;
private:
    void enable(long callId, const Inspector::InspectorObject& message);
    void disable(long callId, const Inspector::InspectorObject& message);
    void setBreakpointsActive(long callId, const Inspector::InspectorObject& message);
    void setBreakpointByUrl(long callId, const Inspector::InspectorObject& message);
    void setBreakpoint(long callId, const Inspector::InspectorObject& message);
    void removeBreakpoint(long callId, const Inspector::InspectorObject& message);
    void continueToLocation(long callId, const Inspector::InspectorObject& message);
    void stepOver(long callId, const Inspector::InspectorObject& message);
    void stepInto(long callId, const Inspector::InspectorObject& message);
    void stepOut(long callId, const Inspector::InspectorObject& message);
    void pause(long callId, const Inspector::InspectorObject& message);
    void resume(long callId, const Inspector::InspectorObject& message);
    void searchInContent(long callId, const Inspector::InspectorObject& message);
    void getScriptSource(long callId, const Inspector::InspectorObject& message);
    void getFunctionDetails(long callId, const Inspector::InspectorObject& message);
    void setPauseOnExceptions(long callId, const Inspector::InspectorObject& message);
    void evaluateOnCallFrame(long callId, const Inspector::InspectorObject& message);
    void setOverlayMessage(long callId, const Inspector::InspectorObject& message);
private:
    InspectorDebuggerBackendDispatcher(Inspector::InspectorBackendDispatcher*, InspectorDebuggerBackendDispatcherHandler*);
    InspectorDebuggerBackendDispatcherHandler* m_agent;
};

#endif // ENABLE(JAVASCRIPT_DEBUGGER)
class JS_EXPORT_PRIVATE InspectorInspectorBackendDispatcher FINAL : public Inspector::InspectorSupplementalBackendDispatcher {
public:
    static PassRefPtr<InspectorInspectorBackendDispatcher> create(Inspector::InspectorBackendDispatcher*, InspectorInspectorBackendDispatcherHandler*);
    virtual void dispatch(long callId, const String& method, PassRefPtr<Inspector::InspectorObject> message) OVERRIDE;
private:
    void enable(long callId, const Inspector::InspectorObject& message);
    void disable(long callId, const Inspector::InspectorObject& message);
private:
    InspectorInspectorBackendDispatcher(Inspector::InspectorBackendDispatcher*, InspectorInspectorBackendDispatcherHandler*);
    InspectorInspectorBackendDispatcherHandler* m_agent;
};

class JS_EXPORT_PRIVATE InspectorRuntimeBackendDispatcher FINAL : public Inspector::InspectorSupplementalBackendDispatcher {
public:
    static PassRefPtr<InspectorRuntimeBackendDispatcher> create(Inspector::InspectorBackendDispatcher*, InspectorRuntimeBackendDispatcherHandler*);
    virtual void dispatch(long callId, const String& method, PassRefPtr<Inspector::InspectorObject> message) OVERRIDE;
private:
    void parse(long callId, const Inspector::InspectorObject& message);
    void evaluate(long callId, const Inspector::InspectorObject& message);
    void callFunctionOn(long callId, const Inspector::InspectorObject& message);
    void getProperties(long callId, const Inspector::InspectorObject& message);
    void releaseObject(long callId, const Inspector::InspectorObject& message);
    void releaseObjectGroup(long callId, const Inspector::InspectorObject& message);
    void run(long callId, const Inspector::InspectorObject& message);
    void enable(long callId, const Inspector::InspectorObject& message);
    void disable(long callId, const Inspector::InspectorObject& message);
private:
    InspectorRuntimeBackendDispatcher(Inspector::InspectorBackendDispatcher*, InspectorRuntimeBackendDispatcherHandler*);
    InspectorRuntimeBackendDispatcherHandler* m_agent;
};


} // namespace Inspector

#endif // !defined(InspectorJSBackendDispatchers_h)
