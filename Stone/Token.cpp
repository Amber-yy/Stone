#include "Token.h"

Token Token::eof(-1);
std::string Token::eol("\n");

Token::Token(int line)
{
	lineNumber = line;
}

Token::~Token()
{
}

bool Token::operator!=(const Token & t)
{
	return !operator==(t);
}

bool Token::operator==(const Token & t)
{
	if (lineNumber == -1 && t.lineNumber == -1)
	{
		return true;
	}
	return false;
}

int Token::getLineNumber()
{
	return lineNumber;
}

bool Token::isIdentifier()
{
	return false;
}

bool Token::isNumber()
{
	return false;
}

bool Token::isString()
{
	return false;
}

int Token::getNumber()
{
	throw StoneException("not a number token\n");
}

std::string& Token::getText()
{
	return text;
}

NumToken::NumToken(int line,int value):Token(line),value(value)
{
}

bool NumToken::isNumber()
{
	return true;
}

std::string& NumToken::getText()
{
	text = std::to_string(value);
	return text;
}

int NumToken::getNumber()
{
	return value;
}

IdToken::IdToken(int line,const std::string& id):Token(line)
{
	text = id;
}

bool IdToken::isIdentifier()
{
	return true;
}

std::string& IdToken::getText()
{
	return text;
}

StringToken::StringToken(int line, const std::string & string):Token(line)
{
	text = string;
}

bool StringToken::isString()
{
	return true;
}

std::string & StringToken::getText()
{
	return text;
}
