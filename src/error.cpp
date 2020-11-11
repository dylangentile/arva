#include "error.h"
#include <cstdlib>
#include <cstdarg>
#include <cstdio>


void 
Error::strf_error(ErrorType type, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	vprintf(fmt, args);

	va_end(args);

	if(type == ErrorType::Fatal)
	{
		printf("\n\nFatal Error!\n");
		exit(1);
	}
}