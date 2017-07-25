#include "Lexer.h"
#include "Token.h"

#include <iostream>
#include <fstream>

#include <memory>

int main()
{

	try
	{
		Lexer lexer;
		std::ifstream ifs("data.txt");
		lexer.load(ifs);
		while (true)
		{
			auto str = std::move(lexer.read());
			
			if (*str == Token::eof)
			{
				break;
			}

			if (str->getText() == Token::eol)
			{
				continue;
			}

			std::cout << str->getText() << '\n';
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what();
	}
	return 0;
}