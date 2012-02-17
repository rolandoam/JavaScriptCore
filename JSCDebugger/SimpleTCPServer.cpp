//
//  SimpleTCPServer.cpp
//  JavaScriptCore
//
//  Created by Rolando Abarca on 2/10/12.
//  Copyright (c) 2012 Zynga Inc. All rights reserved.
//

#include "config.h"
#include "SimpleTCPServer.h"

// auto-generated file with ragel
#include "http_state.c"

using namespace JSCDebug;

SimpleTCPServer::SimpleTCPServer(int port)
{
	struct hostent *hp;
	struct sockaddr_in sa;
	
	printf("starting tcp server on port %d\n", port);
		
	memset(&sa, 0, sizeof(struct sockaddr_in));
	// listen all
	hp = gethostbyname("0.0.0.0");
	ASSERT(hp);
	
	sa.sin_port = htons(port);
	sa.sin_family = hp->h_addrtype;
	
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	ASSERT(m_socket > 0);
	
	if (bind(m_socket, (const struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0) {
		close(m_socket);
		ASSERT(0);
	}
}

SimpleTCPServer::~SimpleTCPServer()
{
	close(m_socket);
}

void SimpleTCPServer::gotHeader(const char *name, const char *value)
{
	printf("got key: '%s' ~> '%s'\n", name, value);
}

void SimpleTCPServer::finishedParsingHeaders()
{
	printf("end of headers - body pending\n");
}

void SimpleTCPServer::start()
{
	listen(m_socket, 1);
	int t;
	if ((t = accept(m_socket, NULL, NULL)) > 0) {
		// we got something, process...
		char buff[128];
		memset(buff, 0, 128);
		int bytesRead = 0;
		char *bodyPointer = (char *)buff;
		while ((bytesRead = read(t, buff, 128)) > 0) {
			int parsedBytes = parseHeaders(buff, bytesRead, this);
			if (parsedBytes > 0) {
				bodyPointer += parsedBytes;
				printf("body: '%s'\n", bodyPointer);
			} else {
				printf("got an error...\n");
				printf("%s\n", buff);
			}
		}
	}
}

void SimpleTCPServer::jsonSend()
{
}

void SimpleTCPServer::jsonReceive()
{
}
