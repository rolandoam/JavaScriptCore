//
//  SimpleTCPServer.h
//  JavaScriptCore
//
//  Created by Rolando Abarca on 2/10/12.
//  Copyright (c) 2012 Zynga Inc. All rights reserved.
//

#ifndef JavaScriptCore_SimpleTCPServer_h
#define JavaScriptCore_SimpleTCPServer_h

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class ParserDelegate
{
public:
	virtual ~ParserDelegate() {};
	virtual void gotHeader(const char *headerName, const char *headerValue) = 0;
	virtual void finishedParsingHeaders() = 0;
};

namespace JSCDebug {
	class SimpleTCPServer : public ParserDelegate
	{
	private:
		int m_socket;
		int m_currentDataLength;
		
	public:
		SimpleTCPServer(int port);
		~SimpleTCPServer();
		
		void start();
		void jsonSend();
		void jsonReceive();
		
		// parser delegate
		virtual void gotHeader(const char *name, const char *value);
		virtual void finishedParsingHeaders();
	};
}
#endif
