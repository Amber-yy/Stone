#pragma once

#include <memory>

class ASTree;
using ASTreeRef = std::shared_ptr<ASTree>;
class Lexer;

class BasicParser
{
public:
	BasicParser();
	virtual ~BasicParser();
	virtual ASTreeRef parse(Lexer &lexer);
protected:
	struct basicParserData;
	std::unique_ptr<basicParserData> data;
};

