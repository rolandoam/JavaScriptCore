
#line 1 "simple_debug.rl"
/**
 * debugger state machine (for JSCDebugger)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef DEBUG_PARSER

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

namespace JSCDebug
{
	class JSCDebugger {
		// control execution
	public:
		const unsigned char* evaluateInCurrentFrame(const unsigned char *scriptSource, unsigned *outLen) { *outLen = strlen((char *)scriptSource); return scriptSource; };
		void continueProgram() {};
		void stepIntoStatement() {};
		void stepOverStatement() {};
		void stepOutOfFunction() {};
	};
}

#endif

extern "C" {

enum {
	kInvalidCommand = 0,
	kNextCommand,
	kStepIntoCommand,
	kBacktraceCommand,
	kBreakpointCommand,
	kEvalCommand,
	kPrintCommand,
	kContinueCommand,
	kRepeatLastCommand
};

void performAction(int command, char **args, JSCDebug::JSCDebugger *debugger, int socket);
int parseInput(int file, JSCDebug::JSCDebugger *debugger);
static bool shouldExit = false;


#line 80 "simple_debug.rl"



#line 57 "simple_debug.c"
static const char _simple_debug_actions[] = {
	0, 1, 0, 1, 2, 1, 3, 1, 
	4, 1, 5, 1, 7, 2, 1, 6
	
};

static const char _simple_debug_key_offsets[] = {
	0, 0, 2, 4, 5, 6, 8, 9, 
	10, 11, 12, 14, 15, 16, 17, 18, 
	19, 20, 22, 23, 24
};

static const char _simple_debug_trans_keys[] = {
	10, 32, 10, 111, 110, 116, 10, 105, 
	110, 117, 101, 10, 32, 118, 10, 10, 
	97, 108, 32, 10, 10, 116, 101, 112, 
	10, 32, 99, 101, 110, 115, 0
};

static const char _simple_debug_single_lengths[] = {
	0, 2, 2, 1, 1, 2, 1, 1, 
	1, 1, 2, 1, 1, 1, 1, 1, 
	1, 2, 1, 1, 6
};

static const char _simple_debug_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const char _simple_debug_index_offsets[] = {
	0, 0, 3, 6, 8, 10, 13, 15, 
	17, 19, 21, 24, 26, 28, 30, 32, 
	34, 36, 39, 41, 43
};

static const char _simple_debug_trans_targs[] = {
	20, 1, 0, 20, 3, 0, 4, 0, 
	5, 0, 20, 6, 0, 7, 0, 8, 
	0, 9, 0, 20, 0, 11, 13, 0, 
	0, 12, 20, 12, 14, 0, 15, 0, 
	11, 0, 20, 0, 20, 18, 0, 19, 
	0, 16, 0, 20, 1, 2, 10, 16, 
	17, 0, 0
};

static const char _simple_debug_trans_actions[] = {
	11, 0, 0, 9, 0, 0, 0, 0, 
	0, 0, 9, 0, 0, 0, 0, 0, 
	0, 0, 0, 9, 0, 0, 0, 0, 
	0, 1, 13, 0, 0, 0, 0, 0, 
	0, 0, 7, 0, 7, 0, 0, 0, 
	0, 0, 0, 11, 0, 0, 0, 0, 
	0, 0, 0
};

static const char _simple_debug_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 3
};

static const char _simple_debug_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 5
};

static const int simple_debug_start = 20;
static const int simple_debug_first_final = 20;
static const int simple_debug_error = 0;

static const int simple_debug_en_main = 20;


#line 83 "simple_debug.rl"

void performAction(int command, char **args, JSCDebug::JSCDebugger *debugger, int socket)
{
	switch (command) {
	case kNextCommand:
		printf("step over\n");
		debugger->stepOverStatement();
		break;
	case kStepIntoCommand:
		printf("step into\n");
		debugger->stepIntoStatement();
		break;
	case kContinueCommand:
		debugger->continueProgram();
		shouldExit = true;
		break;
	case kEvalCommand:
		printf("eval: '%s'\n", *args);
		unsigned outLen;
		const unsigned char *result = debugger->evaluateInCurrentFrame((unsigned char *)*args, &outLen);
		write(socket, result, outLen);
		write(socket, "\n>> ", 4);
		free(*args); *args = NULL;
		break;
	}
}

int parseInput(int file, JSCDebug::JSCDebugger *debugger)
{
#define BUF_SIZE 128
	char buff[BUF_SIZE];
	memset(buff, '\0', BUF_SIZE);
	// machine state
	int cs, act;
	char *ts, *te = 0;
	// arguments storage
	char *args = NULL, *argsBegin = NULL;

	
#line 174 "simple_debug.c"
	{
	cs = simple_debug_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 122 "simple_debug.rl"
	ssize_t readBytes;
	while (!shouldExit && (readBytes = read(file, buff, BUF_SIZE))) {
		char *p = buff, *eof = buff + readBytes;
		char *pe = eof;

		
#line 189 "simple_debug.c"
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
	_acts = _simple_debug_actions + _simple_debug_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 3:
#line 1 "NONE"
	{ts = p;}
	break;
#line 210 "simple_debug.c"
		}
	}

	_keys = _simple_debug_trans_keys + _simple_debug_key_offsets[cs];
	_trans = _simple_debug_index_offsets[cs];

	_klen = _simple_debug_single_lengths[cs];
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

	_klen = _simple_debug_range_lengths[cs];
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
	cs = _simple_debug_trans_targs[_trans];

	if ( _simple_debug_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _simple_debug_actions + _simple_debug_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 62 "simple_debug.rl"
	{
		argsBegin = p;
	}
	break;
	case 1:
#line 65 "simple_debug.rl"
	{
		if (args) {
			free(args);
		}
		ssize_t len = p - argsBegin;
		args = (char *)calloc(len + 1, 1);
		memcpy(args, argsBegin, len);
	}
	break;
	case 4:
#line 75 "simple_debug.rl"
	{te = p+1;{ performAction(kNextCommand, NULL, debugger, file); }}
	break;
	case 5:
#line 76 "simple_debug.rl"
	{te = p+1;{ performAction(kContinueCommand, NULL, debugger, file); {p++; goto _out; } }}
	break;
	case 6:
#line 77 "simple_debug.rl"
	{te = p+1;{ performAction(kEvalCommand, &args, debugger, file); }}
	break;
	case 7:
#line 78 "simple_debug.rl"
	{te = p+1;}
	break;
#line 307 "simple_debug.c"
		}
	}

_again:
	_acts = _simple_debug_actions + _simple_debug_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 2:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 320 "simple_debug.c"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 128 "simple_debug.rl"

		if (cs == simple_debug_error) {
			printf("parser error @ '%s'\n", p);
			return -1;
		}
	}
	return 0;
}

} // extern "C"

#ifdef DEBUG_PARSER
int main(int argc, char **argv) {
	if (argc > 0) {
		int file = open(argv[1], O_RDONLY);
		JSCDebug::JSCDebugger *debugger = new JSCDebug::JSCDebugger();
		parseInput(file, debugger);
		close(file);
	}
}
#endif
