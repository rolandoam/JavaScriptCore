//
//  JSCDebugger.h
//  TestCocos2dX
//
//  Created by Rolando Abarca on 1/26/12.
//  Copyright (c) 2012 Zynga Inc. All rights reserved.
//

#ifndef TestCocos2dX_JSCDebugger_h
#define TestCocos2dX_JSCDebugger_h

#include "JavaScriptCallFrame.h"
#include "ScriptBreakpoint.h"
#include "Debugger.h"

class JSCDebuggerDelegate;

namespace JSCDebug {
	inline String ustringToString(const JSC::UString& u)
    {
        return u.impl();
    }
	
    inline JSC::UString stringToUString(const String& s)
    {
        return JSC::UString(s.impl());
    }
	
	class JSSimpleAccessors
	{
	public:
		static JSC::JSValue getProperty(JSC::JSObject *obj, JSC::ExecState *state, const char *property);
		static void setProperty(JSC::JSObject *obj, const char *property, JSC::JSValue value);
	};

	class JSCDebugger : public JSC::Debugger
	{
	protected:
		typedef HashMap<long, ScriptBreakpoint> LineToBreakpointMap;
		typedef HashMap<intptr_t, LineToBreakpointMap> SourceIdToBreakpointsMap;
		
		JSCDebuggerDelegate*				m_delegate;
		bool								m_shouldPauseOnExceptions;
		bool								m_pauseOnNextStatement;
		bool								m_paused;
		bool								m_breakpointsActivated;
		JavaScriptCallFrame*				m_pauseOnCallFrame;
		WTF::HashMap<intptr_t,JSC::UString>	m_sourceMap;
		RefPtr<JavaScriptCallFrame>			m_currentCallFrame;
		SourceIdToBreakpointsMap			m_sourceIdToBreakpoints;
		JSC::JSGlobalObject*				m_currentGlobalObject;

		void updateCallFrameAndPauseIfNeeded(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		void createCallFrameAndPauseIfNeeded(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		void pauseIfNeeded(JSC::JSGlobalObject* dynamicGlobalObject);
		bool hasBreakpoint(intptr_t sourceID, const TextPosition&) const;
	public:
		JSCDebugger();
		
		void setDelegate(JSCDebuggerDelegate *delegate) { m_delegate = delegate; };
		void setShouldPause(bool shouldPause) { m_pauseOnNextStatement = shouldPause; };
		String setBreakpoint(const String& sourceID, const ScriptBreakpoint& scriptBreakpoint, int* actualLineNumber, int* actualColumnNumber);
		void setBreakpointsActivated(bool activated);
		// control execution
		const unsigned char* evaluateInCurrentFrame(const unsigned char *scriptSource, unsigned *outLen);
		void continueProgram();
		void stepIntoStatement();
		void stepOverStatement();
		void stepOutOfFunction();

		virtual void sourceParsed(JSC::ExecState*, JSC::SourceProvider* sourceProvider, int errorLineNumber, const JSC::UString& errorMessage);
		virtual void exception(const JSC::DebuggerCallFrame& debugFrame, intptr_t sourceID, int lineNumber, bool);
		virtual void atStatement(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		virtual void callEvent(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		virtual void returnEvent(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		
		virtual void willExecuteProgram(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		virtual void didExecuteProgram(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		virtual void didReachBreakpoint(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
	};
}

#endif
