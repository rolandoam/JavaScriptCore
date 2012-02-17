//
//  JSCDebugger.cpp
//  JavaScriptCore
//
//  Created by Rolando Abarca on 1/26/12.
//  Copyright (c) 2012 Zynga Inc. All rights reserved.
//

#include "config.h"
#include <stdio.h>
#include <JavaScriptCore/APICast.h>
#include <parser/SourceProvider.h>
#include "DebuggerCallFrame.h"
#include "JSCDebuggerPrivate.h"
#include "JSCDebugger.h"
#include "SimpleTCPServer.h"

using namespace JSC;

namespace JSCDebug {
	
	JSCDebugger::JSCDebugger() : Debugger(),
								 m_delegate(NULL),
								 m_shouldPause(false),
								 m_shouldPauseOnExceptions(true),
								 m_paused(false),
								 m_breakpointsActivated(true)
	{
	}
	
	void JSCDebugger::createCallFrameAndPauseIfNeeded(const JSC::DebuggerCallFrame &debuggerCallFrame, intptr_t sourceID, int lineNumber)
	{
		TextPosition textPosition(OrdinalNumber::fromOneBasedInt(lineNumber), OrdinalNumber::first());
		m_currentCallFrame = JavaScriptCallFrame::create(debuggerCallFrame, m_currentCallFrame, sourceID, textPosition);
		pauseIfNeeded(debuggerCallFrame.dynamicGlobalObject());
	}
	
	void JSCDebugger::updateCallFrameAndPauseIfNeeded(const JSC::DebuggerCallFrame &debuggerCallFrame, intptr_t sourceID, int lineNumber)
	{
		if (!m_currentCallFrame)
			return;
		
		TextPosition textPosition(OrdinalNumber::fromOneBasedInt(lineNumber), OrdinalNumber::first());
		m_currentCallFrame->update(debuggerCallFrame, sourceID, textPosition);
		pauseIfNeeded(debuggerCallFrame.dynamicGlobalObject());
	}
	
	void JSCDebugger::pauseIfNeeded(JSC::JSGlobalObject *)
	{
		if (m_paused)
			return;
		
		bool pauseNow = m_shouldPause;
		pauseNow |= hasBreakpoint(m_currentCallFrame->sourceID(), m_currentCallFrame->position());
		
		if (!pauseNow)
			return;
		
		m_shouldPause = false;
		m_paused = true;
		
		// enter debug console mode
		// block everything
		m_delegate->shouldPauseJS();
		
		SimpleTCPServer server(2345);
		server.start();
		
		m_paused = false;		
	}
	
	String JSCDebugger::setBreakpoint(const String& sourceID, const ScriptBreakpoint& scriptBreakpoint, int* actualLineNumber, int* actualColumnNumber)
	{
		intptr_t sourceIDValue = sourceID.toIntPtr();
		if (!sourceIDValue)
			return "";
		SourceIdToBreakpointsMap::iterator it = m_sourceIdToBreakpoints.find(sourceIDValue);
		if (it == m_sourceIdToBreakpoints.end())
			it = m_sourceIdToBreakpoints.set(sourceIDValue, LineToBreakpointMap()).first;
		if (it->second.contains(scriptBreakpoint.lineNumber + 1))
			return "";
		it->second.set(scriptBreakpoint.lineNumber + 1, scriptBreakpoint);
		*actualLineNumber = scriptBreakpoint.lineNumber;
		// FIXME(WK53003): implement setting breakpoints by line:column.
		*actualColumnNumber = 0;
		return sourceID + ":" + String::number(scriptBreakpoint.lineNumber);
	}
	
	void JSCDebugger::setBreakpointsActivated(bool activated)
	{
		m_breakpointsActivated = activated;
	}
	
	bool JSCDebugger::hasBreakpoint(intptr_t sourceID, const WTF::TextPosition &position) const
	{
		if (!m_breakpointsActivated)
			return false;
		
		SourceIdToBreakpointsMap::const_iterator it = m_sourceIdToBreakpoints.find(sourceID);
		if (it == m_sourceIdToBreakpoints.end())
			return false;
		int lineNumber = position.m_line.oneBasedInt();
		if (lineNumber <= 0)
			return false;
		LineToBreakpointMap::const_iterator breakIt = it->second.find(lineNumber);
		if (breakIt == it->second.end())
			return false;
		
		// An empty condition counts as no condition which is equivalent to "true".
		if (breakIt->second.condition.isEmpty())
			return true;
		
		JSValue exception;
		JSValue result = m_currentCallFrame->evaluate(stringToUString(breakIt->second.condition), exception);
		if (exception) {
			// An erroneous condition counts as "false".
			return false;
		}
		return result.toBoolean(m_currentCallFrame->scopeChain()->globalObject->globalExec());
	}
	
#pragma mark - JSC events
	
	void JSCDebugger::sourceParsed(ExecState*, SourceProvider* sourceProvider, int errorLineNumber, const UString& errorMessage)
	{
		intptr_t sourceID = sourceProvider->asID();
		m_sourceMap.set(sourceID, sourceProvider->url());
		if (errorLineNumber > 0) {
			CString str = errorMessage.ascii();
			printf("error at %d - %s\n", errorLineNumber, str.data());
		}
	};

	void JSCDebugger::exception(const DebuggerCallFrame& debugFrame, intptr_t sourceID, int lineNumber, bool)
	{
		UString urlString = m_sourceMap.get(sourceID);
		if (m_delegate) {
			// we should pass the utf8 file name and the exception (message)
			CString str = urlString.ascii();
			m_delegate->didReachException(str.data(), toRef(debugFrame.dynamicGlobalObject()->globalExec(), debugFrame.exception()), lineNumber);
			if (!m_paused && m_shouldPauseOnExceptions) {
				m_shouldPause = true;
				updateCallFrameAndPauseIfNeeded(debugFrame, sourceID, lineNumber);
			}
		}
	}

	/**
	 * we should check if we had a breakpoint set at this point
	 */
	void JSCDebugger::atStatement(const DebuggerCallFrame& debugFrame, intptr_t sourceID, int lineNumber)
	{
		UString urlString = m_sourceMap.get(sourceID);
		CString str = urlString.ascii();
//		printf("atStatement %s:%d\n", str.data(), lineNumber);
		if (!m_paused)
			updateCallFrameAndPauseIfNeeded(debugFrame, sourceID, lineNumber);
	}

	/**
	 * we should check if we had a breakpoint set at this point
	 */
	void JSCDebugger::callEvent(const DebuggerCallFrame& debugFrame, intptr_t sourceID, int lineNumber)
	{
		UString urlString = m_sourceMap.get(sourceID);
		CString str = urlString.ascii();
//		printf("callEvent %s:%d\n", str.data(), lineNumber);
		if (!m_paused)
			createCallFrameAndPauseIfNeeded(debugFrame, sourceID, lineNumber);
	}

	/**
	 * we should check if we had a breakpoint set at this point
	 */
	void JSCDebugger::returnEvent(const DebuggerCallFrame& debugFrame, intptr_t sourceID, int lineNumber)
	{
		UString urlString = m_sourceMap.get(sourceID);
		CString str = urlString.ascii();
//		printf("returnEvent %s:%d\n", str.data(), lineNumber);
		
		if (m_paused)
			return;
		
		updateCallFrameAndPauseIfNeeded(debugFrame, sourceID, lineNumber);
		
		if (!m_currentCallFrame)
			return;
		
		// Treat stepping over a return statement like stepping out.
//		if (m_currentCallFrame == m_pauseOnCallFrame)
//			m_pauseOnCallFrame = m_currentCallFrame->caller();
		m_currentCallFrame = m_currentCallFrame->caller();
	}

	void JSCDebugger::willExecuteProgram(const DebuggerCallFrame& debugFrame, intptr_t sourceID, int lineNumber)
	{
		UString urlString = m_sourceMap.get(sourceID);
		CString str = urlString.ascii();
//		printf("willExecuteProgram %s:%d\n", str.data(), lineNumber);
		if (!m_paused)
			createCallFrameAndPauseIfNeeded(debugFrame, sourceID, lineNumber);
	}

	void JSCDebugger::didExecuteProgram(const DebuggerCallFrame&, intptr_t sourceID, int)
	{
		UString urlString = m_sourceMap.get(sourceID);
		CString str = urlString.ascii();
//		printf("didExecuteProgram %s:%d\n", str.data(), lineNumber);
		if (m_paused)
			return;
	}
	
	void JSCDebugger::didReachBreakpoint(const DebuggerCallFrame& debugFrame, intptr_t sourceID, int lineNumber)
	{
		UString urlString = m_sourceMap.get(sourceID);
		CString str = urlString.ascii();
//		printf("didReachBreakpoint %s:%d\n", str.data(), lineNumber);
		if (m_paused)
			return;
		m_shouldPause = true;
		updateCallFrameAndPauseIfNeeded(debugFrame, sourceID, lineNumber);
	}

}

#pragma mark - JSCDebuggerController

JSCDebuggerController::JSCDebuggerController(JSGlobalContextRef globalContext, JSCDebuggerDelegate *delegate)
{
	m_globalContext = globalContext;
	JSC::ExecState *state = toJS(m_globalContext);
	m_debugger = new JSCDebug::JSCDebugger();
	m_debugger->setDelegate(delegate);
	m_debugger->attach(state->lexicalGlobalObject());
}

JSCDebuggerController::~JSCDebuggerController()
{
	JSC::ExecState *state = toJS(m_globalContext);
	m_debugger->detach(state->lexicalGlobalObject());
	delete m_debugger;
}

long JSCDebuggerController::setBreakpoint(const char *scriptURL, int lineNumber)
{
	String sourceId = String(scriptURL);
	JSCDebug::ScriptBreakpoint breakpoint;
	int actualLineNumber = lineNumber, actualColumnNumber;
	m_debugger->setBreakpoint(sourceId, breakpoint, &actualLineNumber, &actualColumnNumber);
	
	return 0;
}

void JSCDebuggerController::evaluateInCurrentFrame(const char *)
{
}
