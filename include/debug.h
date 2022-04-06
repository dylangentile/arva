#pragma once
#include <vector>
#include <cstdint>


struct DebugInfo
{
	const char* file_path;
	uint32_t line_offset; //offset from start of file to beginning of line
	uint32_t token_offset; //offset from start of line to token
	uint32_t char_count;
	uint32_t line_num;
};


struct DebugGroup
{
	std::vector<DebugInfo> info_vec;
};
