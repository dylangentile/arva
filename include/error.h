#pragma once
#include <cstdint>

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
	static void strf_error(ErrorType, const char*, ...);
};