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

class ParserDelegate
{
public:
	virtual ~ParserDelegate() {};
	virtual void gotHeader(const char *headerName, const char *headerValue) = 0;
	virtual void finishedParsingHeaders() = 0;
};

#endif

extern "C" {

int parseHeaders(char *buffer, int length, ParserDelegate *delegate);

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

int parseHeaders(char *buffer, int length, ParserDelegate *delegate)
{
	// machine state
	int cs, act, done = 0;
	char *ts, *te = 0;
	// key and value storage
	char *key = NULL, *value = NULL;
	char *myTs;

	%% write init;

	while (!done) {
		char *p = buffer, *pe = (buffer + length);

		%% write exec;

		if (cs == http_simple_error) {
			printf("error state\n");
			return -1;
		}
		if (!done && p == pe) {
			// EOF
			printf("EOF??");
			return -1;
		}

	}
	// return how many bytes we actually parsed
	return (te - buffer);
}

#ifdef DEBUG_HTTP

class SomeDelegate : public ParserDelegate
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
		char *buff = (char *)malloc(2048);
		SomeDelegate *delegate = new SomeDelegate();
		int f = open(argv[1], O_RDONLY);
		int readBytes = read(f, buff, 2048);
		parseHeaders(buff, readBytes, delegate);
		close(f);
		free(buff);
		delete delegate;
	}
}

#endif

}
