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

%%{
	machine simple_debug;

	# commands
	step       = ('s' | 'step' | 'n');
	cont       = ('c' | 'continue');
	breakpoint = ('b' | 'breakpoint');
	step_into  = ('si');
	backtrace  = ('bt' | 'backtrace');
	eval       = ('e' | 'eval');
	print_cmd  = ('p' | 'print');
	newline    = ('\n');

	action markArgsBegin {
		argsBegin = p;
	}
	action markArgsEnd {
		if (args) {
			free(args);
		}
		ssize_t len = p - argsBegin;
		args = (char *)calloc(len + 1, 1);
		memcpy(args, argsBegin, len);
	}

	main := |*
		step newline => { performAction(kNextCommand, NULL, debugger, file); };
		cont newline => { performAction(kContinueCommand, NULL, debugger, file); };
		eval ' ' (any - newline)+ > markArgsBegin % markArgsEnd newline => { performAction(kEvalCommand, &args, debugger, file); };
	*|;
}%%

%% write data;

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
		break;
	case kEvalCommand:
		printf("eval: '%s'\n", *args);
		unsigned outLen;
		const unsigned char *result = debugger->evaluateInCurrentFrame((unsigned char *)*args, &outLen);
		write(socket, result, outLen);
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

	%% write init;
	ssize_t readBytes;
	while ((readBytes = read(file, buff, BUF_SIZE))) {
		char *p = buff, *eof = buff + readBytes;
		char *pe = eof;

		%% write exec;

		if (cs == simple_debug_error) {
			printf("parser error @ '%s'", p);
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
