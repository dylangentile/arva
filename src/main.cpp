#include <cstdio>
#include "lexer.h"
#include "token.h"
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

	Lexer the_lexer;
	the_lexer.init_lexer(argv[1]);

	Token x;

	do
	{
		the_lexer.fetch_token(&x);
		printf("%u: %s\n", x.m_line_num, x.m_str.c_str());
	} while(x.m_cat != TokenCat::FileEnd);

	
	Error::report();

	SymbolTable::destroy();
	Error::destroy();

	return 0;
}