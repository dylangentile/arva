#pragma once
#include <cstdint>
#include <string>
#include "token.h"




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

	static void initialize();
	static void terminate();
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

void dinfo_error_(ErrorType type, const DebugInfo& d, const char* msg, ...);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#define log_token_fatal(tok, msg, ...) dinfo_error_(ErrorType::Fatal, (tok).debug, msg, ##__VA_ARGS__)
#define log_token_error(tok, msg, ...) dinfo_error_(ErrorType::Error, (tok).debug, msg, ##__VA_ARGS__)
#define log_token_warn(tok, msg, ...) dinfo_error_(ErrorType::Warning, (tok).debug, msg, ##__VA_ARGS__)

#define log_air_error(node, msg, ...) dinfo_error_(ErrorType::Error, (node)->debug.concat(), msg, ##__VA_ARGS__);

#pragma clang diagnostic pop