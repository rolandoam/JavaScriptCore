//
//  SimpleTCPServer.cpp
//  JavaScriptCore
//
//  Created by Rolando Abarca on 2/10/12.
//  Copyright (c) 2012 Zynga Inc. All rights reserved.
//

#include "config.h"
#include <string>
#include "SimpleTCPServer.h"

// auto-generated file with ragel
#include "simple_debug.c"

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

	int on = 1;
	int res = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	(void)res;
	ASSERT(res == 0);

	if (bind(m_socket, (const struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0) {
		close(m_socket);
		ASSERT(0);
	}
}

SimpleTCPServer::~SimpleTCPServer()
{
	close(m_socket);
}

void SimpleTCPServer::start(JSCDebug::JSCDebugger *debugger)
{
	listen(m_socket, 1);
	int t;
	while ((t = accept(m_socket, NULL, NULL)) > 0) {
		// we got something, process...
		write(t, ">> ", 3);
		parseInput(t, debugger);
	}
}
