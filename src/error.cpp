#include "error.h"
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cassert>

Error* Error::g_error = nullptr;


Error::Error() : error_count(0), warning_count(0) {}
Error::~Error(){}

void
Error::init()
{
	g_error = new Error();
}

void
Error::destroy()
{
	delete g_error;
}

void 
Error::str_error(ErrorType type, const char* msg)
{
	assert(g_error != nullptr && "Error reporter not initialized...");

	if(type == ErrorType::Info)
	{
		printf("INFO: %s\n", msg);
	}
	else if(type == ErrorType::Error || type == ErrorType::Fatal)
	{
		g_error->m_recieved_errors = true;
		g_error->error_str += msg;
		g_error->error_count++;

		if(type == ErrorType::Fatal)
		{
			g_error->report();
			exit(1);
		}
	}
	else
	{
		g_error->warning_str += msg;
		g_error->warning_count++;
	}


}

void
Error::strf_error(ErrorType type, const char* fmt, ...)
{
	assert(g_error != nullptr && "Error reporter not initialized...");

	va_list args1, args2;
	va_start(args1, fmt);
	va_copy(args2, args1);

	size_t s = vsnprintf(nullptr, 0, fmt, args1) + 1;
	char* buf = (char*)calloc(s, sizeof(char));
	vsnprintf(buf, s, fmt, args2);

	va_end(args2);
	va_end(args1);


	str_error(type, (const char*)buf);
}

void
Error::record()
{
	assert(g_error != nullptr && "Error reporter not initialized...");
	g_error->m_recieved_errors = false;
}

bool
Error::got_errors()
{
	assert(g_error != nullptr && "Error reporter not initialized...");
	return g_error->m_recieved_errors;
}


void
Error::report()
{
	assert(g_error != nullptr && "Error reporter not initialized...");

	if(g_error->warning_count == 0 && g_error->error_count == 0)
		return;

	printf("WARNINGS:\n%s\n\nERRORS:\n%s\narvac exited with %u warnings and %u errors.\n",
		g_error->warning_str.c_str(), g_error->error_str.c_str(), g_error->warning_count, g_error->error_count);
}
