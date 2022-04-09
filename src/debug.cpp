#include "debug.h"

DebugInfo
DebugGroup::concat()
{
	if(info_vec.empty())
		return {nullptr, 0, 0, 0, 0, false};
	
	return info_vec[0];

	/*

	DebugInfo res_di = info_vec[0];

	const auto ro = [](const DebugInfo& di) -> uint32_t
	{
		return di.line_offset + di.token_offset;
	};

	uint32_t current_offset = 0;

	for(const DebugInfo& d : info_vec)
	{
		if(ro(d) > current_offset)
			current_offset = ro(d);
	}



	res_di.char_count = current_offset - ro(info_vec[0]);
	res_di.multiline = true;
	return res_di;*/


	
}