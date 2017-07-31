#include "Lexer.h"
#include "Token.h"

#include <iostream>
#include <fstream>

#include <memory>

#include "BasicParser.h"
#include "ASTree.h"

int main()
{

	try
	{
		Lexer lexer;
		std::ifstream ifs("data.txt");
		lexer.load(ifs);
		
		BasicParser bp;

		while (*lexer.peek(0) != Token::eof)
		{
			if (lexer.peek(0)->getText() == Token::eol)
			{
				lexer.read();
				continue;
			}
			ASTreeRef ref = bp.parse(lexer);
			std::cout << ref->toString() << '\n';
		}

		//while (*lexer.peek(0) != Token::eof)
		//{
		//	if (lexer.peek(0)->getText() == "\n")
		//	{
		//		lexer.read();
		//		continue;
		//	}
		//	std::cout << lexer.read()->getText() << '\n';
		//}
	}
	catch (std::exception &e)
	{
		std::cout << e.what();
	}
	return 0;
}