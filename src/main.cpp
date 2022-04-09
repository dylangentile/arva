#include "parser.h"
#include "symbol.h"
#include "error.h"


#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>


enum class CmdArgID
{
	print_air,
	g,
	o,
	silent,
};

struct CmdArg
{
	const char* key;
	CmdArgID id;
	bool has_value;
	const char* description;
};

const CmdArg arg_array[] = //must be in str len order bc 'bad' implementation
{
	{"--print-air", CmdArgID::print_air, false, "Prints the AIR (Stage-A intermediate representation)"},
	{"-g", CmdArgID::g, false, "adds debug symbols to binary"},
	{"-o", CmdArgID::o, true, "output binary path"},
	{"--silent", CmdArgID::silent, false, "silences error/warning reporting (used for debugging the compiler)"},
	//{"-O", true},
};

constexpr const size_t arg_array_size = sizeof(arg_array)/sizeof(*arg_array);


struct Opt
{
	CmdArgID id;
	const char* value;
};


void print_help()
{
	printf("ARVA Compiler\nUsage: arvac source.arva\nCommand line options:\n");
	for(size_t i = 0; i < arg_array_size; i++)
	{
		const CmdArg cmd = arg_array[i];
		size_t printed_len = strlen(cmd.key);
		
		printf("  %s", cmd.key);
		if(cmd.has_value)
		{
			const char* v_str = "<value>";
			printf("<value>");
			printed_len += strlen(v_str);
		}

		
		if(printed_len >= 24)
		{
			printf("\n  ");
			printed_len = 0;
		}
		
		for(size_t s = printed_len; s < 24; s++)
			printf(" ");

		printf("%s\n", cmd.description);

	}
}

std::vector<Opt> 
cmd_parse_args(int argc, const char* argv[], std::vector<const char*>& file_vec)
{
	std::vector<Opt> opt_vec;

	for(int i = 1; i < argc; i++)
	{
		const char* arg = argv[i];
		if(strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
		{
			print_help();
			exit(0);
		}

		bool is_flag = false;
		for(size_t k = 0; k < arg_array_size; k++)
		{
			const CmdArg cmd = arg_array[k];

			if(cmd.has_value)
			{
				const size_t cmd_len = strlen(cmd.key);
				if(strncmp(cmd.key, arg, cmd_len) == 0)
				{
					if(strlen(arg) == cmd_len) //consume next arg as value
					{
						if(i + 1 >= argc)
						{
							fprintf(stderr, "Missing value for key: %s", arg);
							exit(1);
						}

						i++;
						opt_vec.push_back({cmd.id, argv[i]});
						is_flag = true;
						break;
					}
					else //use rest of arg as value
					{
						if(strlen(arg) <= cmd_len)
						{
							fprintf(stderr, "sanity check error! bad arg? %s", arg);
							exit(1);
						}
						opt_vec.push_back({cmd.id, arg + cmd_len});
						is_flag = true;
						break;
					}
				}
				//continue
			}
			else
			{
				if(strcmp(cmd.key, arg) == 0)
				{
					opt_vec.push_back({cmd.id, nullptr});
					is_flag = true;
					break;
				}
			}
		}


		if(!is_flag)
			file_vec.push_back(arg);
	}

	return opt_vec;
}



int 
main(int argc, const char* argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s source.arva\n", argv[0]);
		return 1;
	}

	std::string out_file = "a.out";
	bool print_air = false;
	bool silent = false;
	//bool debug_build = false;

	std::vector<const char*> source_file_vec;
	std::vector<Opt> opt_vec = cmd_parse_args(argc, argv, source_file_vec);
	for(const Opt opt : opt_vec)
	{
		switch(opt.id)
		{
			case CmdArgID::print_air:
			{
				print_air = true;
			}
			break;
			case CmdArgID::g:
			{
				//debug_build = true;
			}
			break;
			case CmdArgID::o:
			{
				out_file = opt.value;
			}
			break;
			case CmdArgID::silent:
			{
				silent = true;
			}
			break;
		}
	}


	if(source_file_vec.empty())
	{
		fprintf(stderr, "Usage: %s source.arva\n", argv[0]);
		return 1;
	}





	Error::initialize();

	Parser parser;
	parser.initialize(source_file_vec[0]);
	AIR_Scope* air = parser.parse();
	
	

	if(print_air)
	{
		printf("%s AIR: \n", source_file_vec[0]);
		for(AIR_Node* n : air->node_vec)
			printf("\t%s\n", print_air_node(n, true).c_str());
		printf("\n");
	}

	parser.terminate();

	if(!silent)
		Error::report();
	Error::terminate();



	return 0;
}