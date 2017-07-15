#pragma once

#include <string>
#include <exception>
#include <memory>

class StoneException :public std::exception
{
public:
	StoneException(const char *str) :std::exception(str){}
};

class Token
{
public:
	static Token eof;
	static std::string eol;
public:
	explicit Token(int line);
	virtual ~Token();
	virtual bool operator!=(const Token &t);
	virtual bool operator==(const Token &t);
	virtual int getLineNumber();
	virtual bool isIdentifier();
	virtual bool isNumber();
	virtual bool isString();
	virtual int getNumber();
	virtual std::string& getText();
protected:
	int lineNumber;
	std::string text;
};

class NumToken final:public Token
{
public:
	explicit NumToken(int line,int value);
	virtual bool isNumber() override;
	virtual std::string& getText() override;
	virtual int getNumber()override;
protected:
	int value;
};

class IdToken final:public Token
{
public:
	explicit IdToken(int line,const std::string &id);
	virtual bool isIdentifier() override;
	virtual std::string& getText() override;
};

class StringToken final :public Token
{
public:
	StringToken(int line, const std::string &string);
	virtual bool isString()override;
	virtual std::string& getText()override;
};

using TokenRef=std::shared_ptr <Token>;