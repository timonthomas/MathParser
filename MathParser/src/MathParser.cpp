#include "MathParser.h"


int main(int argc, char* argv[])
{
    ast_generator generator;
    auto ast_root = generator.parse(std::string(argv[1]));    

	script sc;
	sc.create(ast_root);
    return 0;
}

