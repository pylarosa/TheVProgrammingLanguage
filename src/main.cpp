#include <iostream>
#include <fstream>
#include <ios>
#include <optional>
#include <sstream>
#include <vector>

#include "./generation.hpp"

int main(int argc, char *argv[]) {
    // The first argument is always the executable itself;

    if (argc != 2) {
        std::cerr << "Incorrect Usage" << std::endl;
        std::cerr << "Correct usage is: vdium <input.vdm>" << std::endl;
        return EXIT_FAILURE;
    }

    // read a whole ASCII file
    std::string contents; {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in); // reading the file passed in argv[1]
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    std::optional<NodeExit> tree = parser.parse();

    if(!tree.has_value()) {
        std::cerr << "no exit statement found" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(tree.value());
    {
        std::fstream file("out.asm", std::ios::out);
        file << generator.generate();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}
