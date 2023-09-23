#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
using std::cout, std::cerr, std::fstream, std::ios, std::string, std::optional, std::vector;
typedef std::stringstream sstream;

enum class TokenType {
	_return,
	int_lit,
	semi
};

struct Token {
	TokenType type;
	optional<string> value{};
};

string tokens_to_asm(const vector<Token> &tokens) {
	sstream output;
	output<<"global _start\n_start:\n";

	for(size_t i=0; i<tokens.size(); i++) {
		const Token &token = tokens.at(i);

		if(token.type == TokenType::_return) {
			if(i+1 < tokens.size() && tokens.at(i+1).type == TokenType::int_lit) {
				if(i+2 < tokens.size() && tokens.at(i+2).type == TokenType::semi) {
					output<<"    mov rax, 60\n";
					output<<"    mov rdi, "<<tokens.at(i+1).value.value()<<"\n";
					output<<"    syscall";
				}
			}
		}
	}
	return output.str();
}

vector<Token> tokenize(string& str) {
	vector<Token> tokens{};
	string buf{};

	for(size_t i=0; i<str.length(); i++) {
		if(isalpha(str.at(i))) {
			buf.push_back(str.at(i));
			i++;
			while(isalnum(str.at(i))) {
				buf.push_back(str.at(i));
				i++;
			}
			i--;
			if(buf == "return") {
				tokens.push_back({.type = TokenType::_return});
				buf.clear();
				continue;
			}
			else {
				cerr<<"Basically you're dumb"<<"\n";
				exit(EXIT_FAILURE);
			}
		}
		else if(isdigit(str.at(i))) {
			buf.push_back(str.at(i));
			i++;

			while(isdigit(str.at(i))) {
				buf.push_back(str.at(i));
				i++;
			}
			i--;
			tokens.push_back({.type = TokenType::int_lit, .value = buf});
			buf.clear();
		}
		else if(str.at(i) == ';') {
			tokens.push_back({.type = TokenType::semi});
		}
		else if(isspace(str.at(i))) continue;
		else {
			cerr<<"Basically you're dumb"<<"\n";
			exit(EXIT_FAILURE);
		}
	}

	return tokens;
}

int main(int argc, char* argv[]) {
	if(argc != 2) {
		cerr<<"Incorrect usage. Please do the following..."<<"\n"
			<<"dumb <file.um>"<<"\n";
		return EXIT_FAILURE;
	}

	//ONLY READING THE FILE
	/*
	Following code works but the alternative is chosen for the project assuming the function has destructor

	fstream input(argv[1], ios::in);
	sstream content_stream;
	content_stream<<input.rdbuf();
	input.close();
	*/

	string contents;
	{
		//With the implementation of blocking of code we don't require to close the input as it'll be closed after the block ends.
		sstream contents_stream;
		fstream input(argv[1], ios::in);
		contents_stream<<input.rdbuf();
		contents = contents_stream.str();
	}

	vector<Token> tokens = tokenize(contents);

	{
		fstream file("./build/ass/out.asm", ios::out);
		file<<tokens_to_asm(tokens);
	}

	system("nasm -felf64 ./build/ass/out.asm");
	system("ld -o ./build/ass/out ./build/ass/out.o");
	
	return EXIT_SUCCESS;
}
