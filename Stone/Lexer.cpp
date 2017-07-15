#include "Lexer.h"
#include "Token.h"
#include "Reader.h"

#include <vector>

#include <iostream>

struct Lexer::lexerData
{
	std::regex pattern;
	std::vector<TokenRef> allToken;
	Reader reader;
	bool hasMore;
};

Lexer::Lexer()
{
	data = std::make_unique<lexerData>();
	data->hasMore = true;
	data->pattern = "^\\s*((//.*)|([0-9]+)|(\"(\\\\\"|\\\\\\\\|\\\\n|[^\"])*\")|([A-Z_a-z][A-Z_a-z0-9]*|==|<=|>=|&&|\\|\\||[[:punct:]]))?";
}

Lexer::~Lexer()
{

}

void Lexer::load(std::ifstream & file)
{
	data->reader.load(file);
}

void Lexer::load(const std::string & buffer)
{
	data->reader.load(buffer);
}

TokenRef Lexer::read()
{
	if (fill(0))
	{
		TokenRef temp = std::move(*data->allToken.begin());
		data->allToken.erase(data->allToken.begin());
		return temp;
	}

	return std::make_shared<Token>(-1);
}

TokenRef Lexer::peek(int index)
{
	if (fill(index))
	{
		return data->allToken[index];
	}

	return std::make_shared<Token>(-1);
}

bool Lexer::fill(int size)
{
	while (size >= data->allToken.size())
	{
		if (data->hasMore)
		{
			readLine();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void Lexer::readLine()
{
	if (!data->reader.hasMore())
	{
		data->hasMore = false;
		return;
	}

	int lineNum = data->reader.getLineNumber(); 
	std::string buffer = data->reader.readLine();
	
	std::cmatch matcher;
	const char *start = buffer.c_str();

	while (std::regex_search(start, matcher, data->pattern))
	{
		addToken(lineNum, matcher);

		start = matcher.suffix().first;
		if (*start == 0)
		{
			break;
		}
	}

	if (*start != 0)
	{
		//todo，抛出一个词法错误异常
	}

	data->allToken.push_back(std::make_shared<IdToken>(lineNum,Token::eol));
}

void Lexer::addToken(int line, std::cmatch & matcher)
{
	if (matcher[1].length()==0|| matcher[2].length() != 0)
	{
		return;
	}

	TokenRef token;

	if (matcher[3].length() != 0)
	{
		token = std::make_shared<NumToken>(line, std::atoi(matcher[3].first));
	}
	else if (matcher[4].length() != 0)
	{
		std::string str = matcher[4];
		toLiteral(str);
		token = std::make_shared<StringToken>(line,str);
	}
	else if (matcher[6].length() != 0)
	{
		token =std::make_shared<IdToken>(line, std::string(matcher[6]));
	}
	else
	{
		return;
	}

	data->allToken.push_back(token);
}

void Lexer::toLiteral(std::string & str)
{
	std::string temp;
	int len = str.length() - 1;

	for (int i = 1; i < len; ++i)
	{
		char c = str[i];
		if (c == '\\'&&i + 1 < len)
		{
			int c1 = str[i + 1];
			
			if (c1 == 'n')
			{
				c = '\n';
			}
			else if (c1 == 'r')
			{
				c = '\r';
			}
			else if (c1 == 't')
			{
				c = '\t';
			}
			else if (c1 == 'a')
			{
				c = '\a';
			}
			else if (c1 == 'b')
			{
				c = '\b';
			}
			else
			{
				c = c1;
			}

			++i;
		}

		temp.push_back(c);
	}

	str = std::move(temp);
}
