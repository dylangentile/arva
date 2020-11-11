#include <cstdio>
#include "lexer.h"

#include "token.h"
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

	Lexer the_lexer;
	the_lexer.init_lexer(argv[1]);

	Token x;
	the_lexer.fetch_token(&x);


	return 0;
}