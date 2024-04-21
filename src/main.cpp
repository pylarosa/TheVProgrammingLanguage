#include <iostream>
#include <fstream>
#include <ios>
#include <optional>
#include <sstream>
#include <vector>

#include "./tokenization.hpp"

std::string tokens_to_asm(std::vector<Token> &tokens) {
    std::stringstream output;
    output << "global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token &token = tokens.at(i);
        if (token.type == TokenType::exit) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << tokens.at(i + 1).value.value() << "\n";
                    output << "    syscall";
                }
            }
        }
    }
    return output.str();
}

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

    system("rm out.asm");

    {
        std::fstream file("out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}
