#include <iostream>
#include <fstream>
#include <ios>
#include <optional>
#include <sstream>
#include <vector>

enum class TokenType {
    _return,
    int_lit,
    semi
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

std::vector<Token> tokenize(const std::string &str) {
    std::vector<Token> tokens{};
    std::string buf;

    for (int i = 0; i < str.length(); i++) {
        char c = str.at(i);
        if (std::isalpha(c)) {
            buf.push_back(c);
            i++;

            while (std::isalnum(str.at(i))) {
                buf.push_back(str.at(i));
                i++;
            }
            i--;

            if (buf == "return") {
                tokens.push_back({.type = TokenType::_return});
                buf.clear();
            } else {
                std::cerr << "You messed up" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (std::isspace(c)) {
            continue;
        } else if (std::isdigit(c)) {
            buf.push_back(c);
            i++;
            while (std::isdigit(str.at(i))) {
                buf.push_back(str.at(i));
                i++;
            }
            i--;
            tokens.push_back({.type = TokenType::int_lit, .value = buf});
            buf.clear();
        } else if (c == ';') {
            tokens.push_back({.type = TokenType::semi});
        } else {
            std::cerr << "You messed up 2" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}

std::string tokens_to_asm(std::vector<Token> &tokens) {
    std::stringstream output;
    output << "global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token &token = tokens.at(i);
        if (token.type == TokenType::_return) {
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

    std::vector<Token> tokens = tokenize(contents);

    system("rm out.asm");
    {
        std::fstream file("out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}
