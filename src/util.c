#ifndef UTIL_C
#define UTIL_C

#define ABORT(code, fmt, ...)   _abort(code, __FILE__, __LINE__, fmt, ## __VA_ARGS__)

#ifdef EBUG
#define DEBUG(fmt, ...)   _debug(__FILE__, __LINE__, fmt, ## __VA_ARGS__)
#define ASSERT(condition) if (!(condition)) ABORT(1, #condition)
#else
#define DEBUG(fmt, ...) do{}while(0)
#define ASSERT(condition) do{}while(0)
#endif


void _abort(int code, char *file, int line, const char *fmt, ...) __attribute__((noreturn));
void _debug(char *file, int line, const char *fmt, ...);


#if __INCLUDE_LEVEL__ == 0 || defined(NOLIB)

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void _abort(int code, char *file, int line, const char *fmt, ...)
{
	if (errno)
		perror("Errno:");
	fprintf(stderr, "%s:%d:", file, line);
	va_list args;
	va_start (args, fmt);
	vfprintf (stderr, fmt, args);	
	va_end (args);
	fprintf(stderr, "\n");
	fflush(stderr);
	exit(code);
}

void _debug(char *file, int line, const char *fmt, ...)
{
	fprintf(stderr, "%20s:%4d: ", file, line);
	va_list args;
	va_start (args, fmt);
	vfprintf (stderr, fmt, args);	
	va_end (args);
	fprintf(stderr, "\n");
	fflush(stderr);
}

#endif
#endif
