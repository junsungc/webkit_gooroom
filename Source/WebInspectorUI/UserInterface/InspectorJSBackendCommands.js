// File is generated by JavaScriptCore/inspector/scripts/CodeGeneratorInspector.py

// Copyright (c) 2013 Apple Inc. All Rights Reserved.
// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


// Debugger.
InspectorBackend.registerDebuggerDispatcher = InspectorBackend.registerDomainDispatcher.bind(InspectorBackend, "Debugger");
InspectorBackend.registerEnum("Debugger.BreakpointActionType", {Log: "log", Evaluate: "evaluate", Sound: "sound"});
InspectorBackend.registerEnum("Debugger.ScopeType", {Global: "global", Local: "local", With: "with", Closure: "closure", Catch: "catch"});
InspectorBackend.registerEvent("Debugger.globalObjectCleared", []);
InspectorBackend.registerEvent("Debugger.scriptParsed", ["scriptId", "url", "startLine", "startColumn", "endLine", "endColumn", "isContentScript", "sourceMapURL", "hasSourceURL"]);
InspectorBackend.registerEvent("Debugger.scriptFailedToParse", ["url", "scriptSource", "startLine", "errorLine", "errorMessage"]);
InspectorBackend.registerEvent("Debugger.breakpointResolved", ["breakpointId", "location"]);
InspectorBackend.registerEvent("Debugger.paused", ["callFrames", "reason", "data"]);
InspectorBackend.registerEvent("Debugger.resumed", []);
InspectorBackend.registerCommand("Debugger.enable", [], []);
InspectorBackend.registerCommand("Debugger.disable", [], []);
InspectorBackend.registerCommand("Debugger.setBreakpointsActive", [{"name": "active", "type": "boolean", "optional": false}], []);
InspectorBackend.registerCommand("Debugger.setBreakpointByUrl", [{"name": "lineNumber", "type": "number", "optional": false}, {"name": "url", "type": "string", "optional": true}, {"name": "urlRegex", "type": "string", "optional": true}, {"name": "columnNumber", "type": "number", "optional": true}, {"name": "options", "type": "object", "optional": true}], ["breakpointId", "locations"]);
InspectorBackend.registerCommand("Debugger.setBreakpoint", [{"name": "location", "type": "object", "optional": false}, {"name": "options", "type": "object", "optional": true}], ["breakpointId", "actualLocation"]);
InspectorBackend.registerCommand("Debugger.removeBreakpoint", [{"name": "breakpointId", "type": "string", "optional": false}], []);
InspectorBackend.registerCommand("Debugger.continueToLocation", [{"name": "location", "type": "object", "optional": false}], []);
InspectorBackend.registerCommand("Debugger.stepOver", [], []);
InspectorBackend.registerCommand("Debugger.stepInto", [], []);
InspectorBackend.registerCommand("Debugger.stepOut", [], []);
InspectorBackend.registerCommand("Debugger.pause", [], []);
InspectorBackend.registerCommand("Debugger.resume", [], []);
InspectorBackend.registerCommand("Debugger.searchInContent", [{"name": "scriptId", "type": "string", "optional": false}, {"name": "query", "type": "string", "optional": false}, {"name": "caseSensitive", "type": "boolean", "optional": true}, {"name": "isRegex", "type": "boolean", "optional": true}], ["result"]);
InspectorBackend.registerCommand("Debugger.getScriptSource", [{"name": "scriptId", "type": "string", "optional": false}], ["scriptSource"]);
InspectorBackend.registerCommand("Debugger.getFunctionDetails", [{"name": "functionId", "type": "string", "optional": false}], ["details"]);
InspectorBackend.registerCommand("Debugger.setPauseOnExceptions", [{"name": "state", "type": "string", "optional": false}], []);
InspectorBackend.registerCommand("Debugger.evaluateOnCallFrame", [{"name": "callFrameId", "type": "string", "optional": false}, {"name": "expression", "type": "string", "optional": false}, {"name": "objectGroup", "type": "string", "optional": true}, {"name": "includeCommandLineAPI", "type": "boolean", "optional": true}, {"name": "doNotPauseOnExceptionsAndMuteConsole", "type": "boolean", "optional": true}, {"name": "returnByValue", "type": "boolean", "optional": true}, {"name": "generatePreview", "type": "boolean", "optional": true}], ["result", "wasThrown"]);
InspectorBackend.registerCommand("Debugger.setOverlayMessage", [{"name": "message", "type": "string", "optional": true}], []);

// Inspector.
InspectorBackend.registerInspectorDispatcher = InspectorBackend.registerDomainDispatcher.bind(InspectorBackend, "Inspector");
InspectorBackend.registerEvent("Inspector.evaluateForTestInFrontend", ["testCallId", "script"]);
InspectorBackend.registerEvent("Inspector.inspect", ["object", "hints"]);
InspectorBackend.registerEvent("Inspector.detached", ["reason"]);
InspectorBackend.registerEvent("Inspector.targetCrashed", []);
InspectorBackend.registerCommand("Inspector.enable", [], []);
InspectorBackend.registerCommand("Inspector.disable", [], []);

// Runtime.
InspectorBackend.registerRuntimeDispatcher = InspectorBackend.registerDomainDispatcher.bind(InspectorBackend, "Runtime");
InspectorBackend.registerEnum("Runtime.RemoteObjectType", {Object: "object", Function: "function", Undefined: "undefined", String: "string", Number: "number", Boolean: "boolean"});
InspectorBackend.registerEnum("Runtime.RemoteObjectSubtype", {Array: "array", Null: "null", Node: "node", Regexp: "regexp", Date: "date"});
InspectorBackend.registerEnum("Runtime.PropertyPreviewType", {Object: "object", Function: "function", Undefined: "undefined", String: "string", Number: "number", Boolean: "boolean"});
InspectorBackend.registerEnum("Runtime.PropertyPreviewSubtype", {Array: "array", Null: "null", Node: "node", Regexp: "regexp", Date: "date"});
InspectorBackend.registerEnum("Runtime.SyntaxErrorType", {None: "none", Irrecoverable: "irrecoverable", UnterminatedLiteral: "unterminated-literal", Recoverable: "recoverable"});
InspectorBackend.registerEvent("Runtime.executionContextCreated", ["context"]);
InspectorBackend.registerCommand("Runtime.parse", [{"name": "source", "type": "string", "optional": false}], ["result", "message", "range"]);
InspectorBackend.registerCommand("Runtime.evaluate", [{"name": "expression", "type": "string", "optional": false}, {"name": "objectGroup", "type": "string", "optional": true}, {"name": "includeCommandLineAPI", "type": "boolean", "optional": true}, {"name": "doNotPauseOnExceptionsAndMuteConsole", "type": "boolean", "optional": true}, {"name": "contextId", "type": "number", "optional": true}, {"name": "returnByValue", "type": "boolean", "optional": true}, {"name": "generatePreview", "type": "boolean", "optional": true}], ["result", "wasThrown"]);
InspectorBackend.registerCommand("Runtime.callFunctionOn", [{"name": "objectId", "type": "string", "optional": false}, {"name": "functionDeclaration", "type": "string", "optional": false}, {"name": "arguments", "type": "object", "optional": true}, {"name": "doNotPauseOnExceptionsAndMuteConsole", "type": "boolean", "optional": true}, {"name": "returnByValue", "type": "boolean", "optional": true}, {"name": "generatePreview", "type": "boolean", "optional": true}], ["result", "wasThrown"]);
InspectorBackend.registerCommand("Runtime.getProperties", [{"name": "objectId", "type": "string", "optional": false}, {"name": "ownProperties", "type": "boolean", "optional": true}], ["result", "internalProperties"]);
InspectorBackend.registerCommand("Runtime.releaseObject", [{"name": "objectId", "type": "string", "optional": false}], []);
InspectorBackend.registerCommand("Runtime.releaseObjectGroup", [{"name": "objectGroup", "type": "string", "optional": false}], []);
InspectorBackend.registerCommand("Runtime.run", [], []);
InspectorBackend.registerCommand("Runtime.enable", [], []);
InspectorBackend.registerCommand("Runtime.disable", [], []);
