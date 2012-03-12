
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


#line 74 "http_state.rl"



#line 38 "http_state.c"
static const char _http_simple_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6
};

static const char _http_simple_key_offsets[] = {
	0, 0, 1, 8, 10, 13, 17, 18, 
	21
};

static const char _http_simple_trans_keys[] = {
	10, 32, 45, 58, 65, 90, 97, 122, 
	32, 58, 32, 33, 126, 13, 32, 33, 
	126, 10, 13, 32, 126, 13, 45, 65, 
	90, 97, 122, 0
};

static const char _http_simple_single_lengths[] = {
	0, 1, 3, 2, 1, 2, 1, 1, 
	2
};

static const char _http_simple_range_lengths[] = {
	0, 0, 2, 0, 1, 1, 0, 1, 
	2
};

static const char _http_simple_index_offsets[] = {
	0, 0, 2, 8, 11, 14, 18, 20, 
	23
};

static const char _http_simple_trans_targs[] = {
	8, 0, 3, 2, 4, 2, 2, 0, 
	3, 4, 0, 5, 7, 0, 6, 5, 
	7, 0, 8, 0, 6, 7, 0, 1, 
	2, 2, 2, 0, 0
};

static const char _http_simple_trans_actions[] = {
	13, 0, 1, 0, 1, 0, 0, 0, 
	0, 0, 0, 5, 5, 0, 3, 5, 
	5, 0, 11, 0, 3, 0, 0, 0, 
	5, 5, 5, 0, 0
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

int parseHeaders(int file, HTTPParserDelegate *delegate)
{
	// machine state
	int cs, act, done = 0;
	char *ts, *te = 0;
	// key and value storage
	char *key = NULL, *value = NULL;
	char *myTs;

	
#line 114 "http_state.c"
	{
	cs = http_simple_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 88 "http_state.rl"

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

		
#line 137 "http_state.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

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
#line 158 "http_state.c"
		}
	}

	_keys = _http_simple_trans_keys + _http_simple_key_offsets[cs];
	_trans = _http_simple_index_offsets[cs];

	_klen = _http_simple_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _http_simple_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	cs = _http_simple_trans_targs[_trans];

	if ( _http_simple_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _http_simple_actions + _http_simple_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
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
#line 262 "http_state.c"
		}
	}

_again:
	_acts = _http_simple_actions + _http_simple_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 3:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 275 "http_state.c"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 102 "http_state.rl"

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
