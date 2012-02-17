
#line 1 "http_state.rl"
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


#line 74 "http_state.rl"



#line 38 "http_state.c"
static const char _http_simple_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6
};

static const char _http_simple_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	7
};

static const char _http_simple_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	9
};

static const int http_simple_start = 8;
static const int http_simple_first_final = 8;
static const int http_simple_error = 0;

static const int http_simple_en_main = 8;


#line 77 "http_state.rl"

int parseHeaders(char *buffer, int length, ParserDelegate *delegate)
{
	// machine state
	int cs, act, done = 0;
	char *ts, *te = 0;
	// key and value storage
	char *key = NULL, *value = NULL;
	char *myTs;

	
#line 73 "http_state.c"
	{
	cs = http_simple_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 88 "http_state.rl"

	while (!done) {
		char *p = buffer, *pe = (buffer + length);

		
#line 87 "http_state.c"
	{
	const char *_acts;
	unsigned int _nacts;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_acts = _http_simple_actions + _http_simple_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 4:
#line 1 "NONE"
	{ts = p;}
	break;
#line 105 "http_state.c"
		}
	}

	switch ( cs ) {
case 8:
	switch( (*p) ) {
		case 13: goto tr12;
		case 45: goto tr13;
	}
	if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr13;
	} else if ( (*p) >= 65 )
		goto tr13;
	goto tr1;
case 0:
	goto _out;
case 1:
	if ( (*p) == 10 )
		goto tr0;
	goto tr1;
case 2:
	switch( (*p) ) {
		case 32: goto tr2;
		case 45: goto tr3;
		case 58: goto tr4;
	}
	if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr3;
	} else if ( (*p) >= 65 )
		goto tr3;
	goto tr1;
case 3:
	switch( (*p) ) {
		case 32: goto tr5;
		case 58: goto tr6;
	}
	goto tr1;
case 4:
	if ( (*p) == 32 )
		goto tr7;
	if ( 33 <= (*p) && (*p) <= 126 )
		goto tr8;
	goto tr1;
case 5:
	switch( (*p) ) {
		case 13: goto tr9;
		case 32: goto tr7;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto tr8;
	goto tr1;
case 6:
	if ( (*p) == 10 )
		goto tr10;
	goto tr1;
case 7:
	if ( (*p) == 13 )
		goto tr9;
	if ( 32 <= (*p) && (*p) <= 126 )
		goto tr11;
	goto tr1;
	}

	tr1: cs = 0; goto _again;
	tr12: cs = 1; goto _again;
	tr3: cs = 2; goto _again;
	tr13: cs = 2; goto f2;
	tr5: cs = 3; goto _again;
	tr2: cs = 3; goto f1;
	tr6: cs = 4; goto _again;
	tr4: cs = 4; goto f1;
	tr7: cs = 5; goto f2;
	tr9: cs = 6; goto f3;
	tr11: cs = 7; goto _again;
	tr8: cs = 7; goto f2;
	tr0: cs = 8; goto f0;
	tr10: cs = 8; goto f4;

	f1: _acts = _http_simple_actions + 1; goto execFuncs;
	f3: _acts = _http_simple_actions + 3; goto execFuncs;
	f2: _acts = _http_simple_actions + 5; goto execFuncs;
	f4: _acts = _http_simple_actions + 11; goto execFuncs;
	f0: _acts = _http_simple_actions + 13; goto execFuncs;

execFuncs:
	_nacts = *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 0:
#line 33 "http_state.rl"
	{
		if (key) {
			free(key);
		}
		key = (char *)calloc(p - myTs + 1, 1);
		memcpy(key, myTs, p - myTs);
	}
	break;
	case 1:
#line 41 "http_state.rl"
	{
		if (value) {
			free(value);
		}
		value = (char *)calloc(p - myTs + 1, 1);
		memcpy(value, myTs, p - myTs);
	}
	break;
	case 2:
#line 49 "http_state.rl"
	{
		myTs = p;
	}
	break;
	case 5:
#line 65 "http_state.rl"
	{te = p+1;{
			if (delegate) delegate->gotHeader((const char *)key, (const char *)value);
		}}
	break;
	case 6:
#line 68 "http_state.rl"
	{te = p+1;{
			if (delegate) delegate->finishedParsingHeaders();
			done = 1;
			{p++; goto _out; }
		}}
	break;
#line 236 "http_state.c"
		}
	}
	goto _again;

_again:
	_acts = _http_simple_actions + _http_simple_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 3:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 250 "http_state.c"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 93 "http_state.rl"

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
