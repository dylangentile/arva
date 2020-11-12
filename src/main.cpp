#include <cstdio>
#include "parser.h"
#include "symbol.h"
#include "error.h"
/*

const char* useable_flags[] = {
	"-g",
	"--print-debug",
	"-o"
};


constexpr const size_t useable_flag_count = sizeof(useable_flags);
*/



int 
main(int argc, const char* argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Usage: %s source.arva\n", argv[0]);
		return 1;
	}
	Error::init();
	SymbolTable::init();

	Parser parser;
	parser.init(argv[1]);
	parser.parse();

	
	Error::report();

	SymbolTable::destroy();
	Error::destroy();

	return 0;
}