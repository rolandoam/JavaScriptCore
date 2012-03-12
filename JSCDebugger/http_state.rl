/**
 * simplest http-header state machine (for JSCDebugger)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef DEBUG_HTTP

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

class HTTPParserDelegate
{
public:
	virtual ~HTTPParserDelegate() {};
	virtual void gotHeader(const char *headerName, const char *headerValue) = 0;
	virtual void finishedParsingHeaders() = 0;
};

#endif

extern "C" {

int parseHeaders(int file, HTTPParserDelegate *delegate);

%%{
	machine http_simple;

	action copyKey {
		if (key) {
			free(key);
		}
		key = (char *)calloc(p - myTs + 1, 1);
		memcpy(key, myTs, p - myTs);
	}

	action copyValue {
		if (value) {
			free(value);
		}
		value = (char *)calloc(p - myTs + 1, 1);
		memcpy(value, myTs, p - myTs);
	}

	action markBegin {
		myTs = p;
	}

	action errorHandler {
		printf("parser error\n");
		done = 1;
	}

	crlf = '\r\n';
	key = (alpha | '-')+ % copyKey;
	value = print+ % copyValue;
	blank = ' ';
	header = key >markBegin blank* ':' blank* value >markBegin crlf;

	main := |*
		header => {
			if (delegate) delegate->gotHeader((const char *)key, (const char *)value);
		};
		crlf => {
			if (delegate) delegate->finishedParsingHeaders();
			done = 1;
			fbreak;
		};
	*|;
}%%

%% write data;

int parseHeaders(int file, HTTPParserDelegate *delegate)
{
	// machine state
	int cs, act, done = 0;
	char *ts, *te = 0;
	// key and value storage
	char *key = NULL, *value = NULL;
	char *myTs;

	%% write init;

	#define READ_CHUNK_SIZE 128
	int bufferSize = READ_CHUNK_SIZE;
	char *buffer = (char *)calloc(bufferSize + 1, 1);
	char *p = buffer;
	while (!done) {
		ssize_t readBytes = read(file, p, READ_CHUNK_SIZE);
		if (readBytes <= 0) {
			printf("EOF!?\n");
			return -1;
		}
		char *pe = (p + readBytes);

		%% write exec;

		if (cs == http_simple_error) {
			printf("error parsing\n");
		}
		if (!done) {
			bufferSize *= 2;
			char *newBuffer = (char *)realloc(buffer, bufferSize + 1);
			if (!newBuffer) {
				free(buffer);
				printf("OUT OF MEMORY\n");
				return 0;
			}
			if (newBuffer != buffer) {
				myTs = newBuffer + (myTs - buffer);
				p = newBuffer + (p - buffer);
				buffer = newBuffer;
			}
		}
	}
	free(buffer);
	// return how many bytes we actually parsed
	return (p - buffer);
}

#ifdef DEBUG_HTTP

class SomeDelegate : public HTTPParserDelegate
{
	void gotHeader(const char *name, const char *value)
	{
		printf("header: '%s' ~> '%s'\n", name, value);
	}

	void finishedParsingHeaders()
	{
		printf("we're done\n");
	}
};

int main(int argc, char **argv) {
	if (argc > 0) {
		char *buff = (char *)calloc(2048, 1);
		SomeDelegate *delegate = new SomeDelegate();
		int f = open(argv[1], O_RDONLY);
		int parsedBytes = parseHeaders(f, delegate);
		if (parsedBytes > 0) {
			read(f, buff, 2048);
			printf("body:\n%s", buff);
		}
		close(f);
		free(buff);
		delete delegate;
	}
}

#endif

}
