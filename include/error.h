#pragma once
#include <cstdint>
#include <string>
#include "token.h"


struct DebugInfo
{
	const char* m_file_path;
	uint32_t m_offset;
	uint32_t m_line_num;
};

enum class ErrorType : uint8_t
{
	Info,
	Warning,
	Error,
	Fatal,
};



class Error
{
	Error();
	~Error();
public:

	static void init();
	static void destroy();
	static void str_error(ErrorType, const char*);
	static void strf_error(ErrorType, const char*, ...);
	static void report();

	static void record();
	static bool got_errors();

private:
	static Error* g_error;
	
	std::string error_str;
	uint32_t error_count;
	std::string warning_str;
	uint32_t warning_count;

	bool m_recieved_errors = false;
};

void token_error(ErrorType type, Token* tok, const char* msg, ...);
void dinfo_error(ErrorType type, const DebugInfo& d, const char* msg, ...);