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

	class JSCDebugger : public JSC::Debugger
	{
	public:
		JSCDebugger();
		
		void setDelegate(JSCDebuggerDelegate *delegate) { m_delegate = delegate; };
		void setShouldPause(bool shouldPause) { m_shouldPause = shouldPause; };
		String setBreakpoint(const String& sourceID, const ScriptBreakpoint& scriptBreakpoint, int* actualLineNumber, int* actualColumnNumber);
		void setBreakpointsActivated(bool activated);
		void evaluateInCurrentFrame(const char *script);
		
		virtual void sourceParsed(JSC::ExecState*, JSC::SourceProvider* sourceProvider, int errorLineNumber, const JSC::UString& errorMessage);
		virtual void exception(const JSC::DebuggerCallFrame& debugFrame, intptr_t sourceID, int lineNumber, bool);
		virtual void atStatement(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		virtual void callEvent(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		virtual void returnEvent(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		
		virtual void willExecuteProgram(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		virtual void didExecuteProgram(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		virtual void didReachBreakpoint(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		
	protected:
		typedef HashMap<long, ScriptBreakpoint> LineToBreakpointMap;
		typedef HashMap<intptr_t, LineToBreakpointMap> SourceIdToBreakpointsMap;

		JSCDebuggerDelegate					*m_delegate;
		bool								m_shouldPause;
		bool								m_shouldPauseOnExceptions;
		bool								m_paused;
		bool								m_breakpointsActivated;
		WTF::HashMap<intptr_t,JSC::UString>	m_sourceMap;
		RefPtr<JavaScriptCallFrame>			m_currentCallFrame;
		SourceIdToBreakpointsMap			m_sourceIdToBreakpoints;
		
		void updateCallFrameAndPauseIfNeeded(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		void createCallFrameAndPauseIfNeeded(const JSC::DebuggerCallFrame&, intptr_t sourceID, int lineNumber);
		void pauseIfNeeded(JSC::JSGlobalObject* dynamicGlobalObject);
		bool hasBreakpoint(intptr_t sourceID, const TextPosition&) const;
		
	};
}

#endif
