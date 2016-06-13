#include "MathParser.h"


int main(int argc, char* argv[])
{
    if (argc == 1) {
        cout << "input expected" << endl;
    }

    ast_generator ast_gen;
    code_generator code_gen;
    vm vm;

    auto ast_root = ast_gen.parse(std::string(argv[1]));

    if (ast_root) {
        script sc = code_gen.create(ast_root);
        vm.execute(sc);
    }

    cin.get();
    return 0;
}

