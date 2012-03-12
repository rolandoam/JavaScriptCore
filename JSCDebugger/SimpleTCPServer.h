//
//  SimpleTCPServer.h
//  JavaScriptCore
//
//  Created by Rolando Abarca on 2/10/12.
//  Copyright (c) 2012 Zynga Inc. All rights reserved.
//

#ifndef JavaScriptCore_SimpleTCPServer_h
#define JavaScriptCore_SimpleTCPServer_h

#include "JSCDebuggerPrivate.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace JSCDebug {
	class SimpleTCPServer
	{
	private:
		int m_socket;
		int m_currentDataLength;

		int parseLine(int socket);

	public:
		SimpleTCPServer(int port);
		~SimpleTCPServer();

		void start(JSCDebug::JSCDebugger *delegate);
	};
}
#endif
