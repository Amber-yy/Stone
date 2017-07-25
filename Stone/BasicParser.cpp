#include "BasicParser.h"

#include "Lexer.h"
#include "ASTree.h"
#include "Parser.h"
#include "Token.h"

#include <set>
#include <string>

struct BasicParser::basicParserData
{
	std::set<std::string> reserved;
	Operator operators;
	ParserRef expr0;
	ParserRef primary;
	ParserRef factor;
	ParserRef expr;
	ParserRef statement0;
	ParserRef block;
	ParserRef simple;
	ParserRef statement;
	ParserRef program;
};

BasicParser::BasicParser()
{
	std::vector<ASTreeRef> tempRef;

	data->expr0 = Parser::rule();
	data->primary = Parser::rule(std::make_shared<PrimaryExpr>(tempRef))-> or
	({ 
		Parser::rule()->sep({"("})->ast(data->expr0)->sep({")"}),
		Parser::rule()->number(std::make_shared<NumberLiteral>(TokenRef())),
		Parser::rule()->identifier(std::make_shared<Name>(TokenRef()),data->reserved),
		Parser::rule()->string(std::make_shared<StringLiteral>(TokenRef()))
	});

	data->factor = Parser::rule()-> or 
	({
		Parser::rule(std::make_shared<NegativeExpr>(tempRef))->sep({"-"})->ast(data->primary),
		data->primary
	});

	data->expr = data->expr0->expression(std::make_shared<BinaryExpr>(tempRef), data->factor, data->operators);

	data->statement0 = Parser::rule();

	data->block = Parser::rule(std::make_shared<BlockStmnt>(tempRef))->sep({ "{" })->option(data->statement0)->repeat
	(
		Parser::rule()->sep({ ";",Token::eol })->sep({ "else" })->ast(data->block)->sep({"}"})
	);

	data->simple = Parser::rule(std::make_shared<PrimaryExpr>(tempRef))->ast(data->expr);

	data->statement = data->statement0-> or
	({
		Parser::rule(std::make_shared<IfStmnt>(tempRef))->sep({"if"})->ast(data->expr)->ast(data->block)->option(Parser::rule()->sep({"else"})->ast(data->block)),
		Parser::rule(std::make_shared<WhileStmnt>(tempRef))->sep({"while"})->ast(data->expr)->ast(data->block),
		data->simple
	});

	data->program = Parser::rule()-> or
	({
		data->statement,
		Parser::rule(std::make_shared<NullStmnt>(tempRef))->sep({";",Token::eol})
	});

	data->reserved.insert(";");
	data->reserved.insert("}");
	data->reserved.insert(Token::eol);

	data->operators.add("=",1,false);
	data->operators.add("==", 2, true);
	data->operators.add(">", 2, true);
	data->operators.add("<", 2, true);
	data->operators.add("+", 3, true);
	data->operators.add("-", 3, true);
	data->operators.add("*", 4, true);
	data->operators.add("/", 4, true);
	data->operators.add("%", 4, true);

}

BasicParser::~BasicParser()
{
}

ASTreeRef BasicParser::parse(Lexer & lexer)
{
	return data->program->parse(lexer);
}
