#pragma once

#include <memory>
#include <string>
#include <regex>

class Token;
using TokenRef = std::shared_ptr <Token>;

class Lexer
{
public:
	Lexer();
	~Lexer();
	void load(std::ifstream &file);
	void load(const std::string &buffer);
	TokenRef read();
	TokenRef peek(int index);
protected:
	bool fill(int size);
	void readLine();
	void addToken(int line,std::cmatch &matcher);
	void toLiteral(std::string &str);
protected:
	struct lexerData;
	std::unique_ptr<lexerData> data;
};

