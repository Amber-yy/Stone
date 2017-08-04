#include "Lexer.h"
#include "Token.h"

#include <iostream>
#include <fstream>

#include <memory>

#include "BasicParser.h"
#include "ASTree.h"

int main(int argc,char **argv)
{
	if (argc < 2)
	{
		return 0;
	}

	Enviroment e;

	try
	{
		Lexer lexer;
		std::ifstream ifs(argv[1]);
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

			ref->eval(e);
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what();
	}

	return 0;
}