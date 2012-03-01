//
//  SimpleTCPServer.cpp
//  JavaScriptCore
//
//  Created by Rolando Abarca on 2/10/12.
//  Copyright (c) 2012 Zynga Inc. All rights reserved.
//

#include "config.h"
#include <runtime/JSONObject.h>
#include <runtime/LiteralParser.h>
#include "SimpleTCPServer.h"
#include <string>

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

void SimpleTCPServer::gotHeader(const char *name, const char *value)
{
	std::string key(name);
	if (key.compare("Content-Length") == 0) {
		m_currentDataLength = atoi(value);
	}
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
	while ((t = accept(m_socket, NULL, NULL)) > 0) {
		// we got something, process...
		int parsed = parseHeaders(t, this);
		if (parsed && m_currentDataLength) {
			// parsed headers and actually got a body length, now read that body
			unsigned char *buff = (unsigned char *)calloc(m_currentDataLength + 1, 1);
			int bytesRead = read(t, buff, m_currentDataLength);
			while (bytesRead < m_currentDataLength) {
				bytesRead = read(t, buff + bytesRead, m_currentDataLength - bytesRead);
			}
			parseRequest(buff, m_currentDataLength);
		}
	}
}

void SimpleTCPServer::jsonSend()
{
}

void SimpleTCPServer::jsonReceive()
{
}
