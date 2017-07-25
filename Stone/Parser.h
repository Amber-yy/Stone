#pragma once

#include "Lexer.h"
#include "ASTree.h"

#include <set>
#include <map>

class Parser;
using ParserRef = std::shared_ptr<Parser>;

class Operator;

class Factory final
{
public:
	Factory(ASTreeRef &t);
	ASTreeRef make(TokenRef &token, std::vector<ASTreeRef>&ref);
private:
	struct factoryData;
	std::unique_ptr<factoryData> data;
};

class Parser
{
public:
	Parser();
	Parser(const Parser &p);
	Parser(ASTreeRef ref);
	virtual ASTreeRef parse(Lexer &lexer);
	virtual bool match(Lexer &lexer);
	virtual ~Parser();
	virtual ParserRef reset();
	virtual ParserRef reset(ASTreeRef ref);
	virtual ParserRef number(ASTreeRef ref);
	virtual ParserRef identifier(std::set<std::string>& reserved);
	virtual ParserRef identifier(ASTreeRef ref, std::set<std::string>& reserved);
	virtual ParserRef string();
	virtual ParserRef string(ASTreeRef ref);
	virtual ParserRef token(const std::vector<std::string>& pat);
	virtual ParserRef sep(const std::vector<std::string> &pat);
	virtual ParserRef ast(ParserRef ref);
	virtual ParserRef or (const std::vector<ParserRef>& p);
	virtual ParserRef maybe(ParserRef ref);
	virtual ParserRef option(ParserRef ref);
	virtual ParserRef repeat(ParserRef ref);
	virtual ParserRef expression(ParserRef subexp, Operator &operators);
	virtual ParserRef expression(ASTreeRef ref, ParserRef subexp, Operator &operators);
	virtual ParserRef insertChoice(ParserRef p);
	static ParserRef rule();
	static ParserRef rule(ASTreeRef ref);
protected:
	struct parserData;
	std::unique_ptr<parserData> data;
};

class Element
{
public:
	virtual void parse(Lexer &lexer,std::vector<ASTreeRef> &res) = 0;
	virtual bool match(Lexer &lexer) = 0;
};

class Tree :public Element
{
public:
	Tree(ParserRef ref);
	virtual~Tree();
	virtual void parse(Lexer &lexer, std::vector<ASTreeRef> &res)override;
	virtual bool match(Lexer &lexer)override;
protected:
	struct treeData;
	std::unique_ptr<treeData> data;
};

class OrTree :public Element
{
public:
	OrTree(const std::vector<ParserRef> &ref);
	virtual~OrTree();
	virtual void parse(Lexer &lexer, std::vector<ASTreeRef> &res)override;
	virtual bool match(Lexer &lexer)override;
	virtual ParserRef choose(Lexer &lexer);
	virtual void insert(ParserRef p);
protected:
	struct orTreeData;
	std::unique_ptr<orTreeData> data;
};

class Repeat :public Element
{
public:
	Repeat(ParserRef p, bool once);
	virtual void parse(Lexer &lexer, std::vector<ASTreeRef> &res)override;
	virtual bool match(Lexer &lexer) override;
protected:
	struct repeatData;
	std::unique_ptr<repeatData> data;
};

class AToken :public Element
{
public:
	AToken(ASTreeRef ref);
	virtual void parse(Lexer &lexer, std::vector<ASTreeRef> &res)override;
	virtual bool match(Lexer &lexer) override;
	virtual bool test(TokenRef t) = 0;
protected:
	struct tokenData;
	std::unique_ptr<tokenData> data;
};

class IDToken :public AToken
{
public:
	IDToken(ASTreeRef ref, const std::set<std::string> &s);
	virtual bool test(TokenRef t)override;
protected:
	struct idData;
	std::unique_ptr<idData> data;
};

class NUMToken :public AToken
{
public:
	NUMToken(ASTreeRef ref);
	virtual bool test(TokenRef t)override;
};

class STRToken :public AToken
{
public:
	STRToken(ASTreeRef ref);
	virtual bool test(TokenRef t)override;
};

class Leaf :public Element
{
public:
	Leaf(const std::vector<std::string> &pat);
	virtual void parse(Lexer &lexer, std::vector<ASTreeRef> &res)override;
	virtual bool match(Lexer &lexer)override;
	virtual void find(std::vector<ASTreeRef>&ref, TokenRef t);
protected:
	struct leafData;
	std::unique_ptr<leafData> data;
};

class Skip :public Leaf
{
public:
	Skip(const std::vector<std::string> &pat);
	virtual void find(std::vector<ASTreeRef>&ref, TokenRef t)override;
};

class Precedence
{
public:
	Precedence();
	Precedence(const Precedence &prec);
	Precedence(int v, bool a);
	Precedence& operator=(Precedence &&pre);
	bool left();
	bool ok();
	int value();
protected:
	struct preData;
	std::unique_ptr<preData> data;
};

struct Precedence::preData
{
	int value;
	bool left;
};

class Operator :public std::map<std::string, Precedence>
{
public:
	void add(std::string name, int prec, bool left);
};

class Expr :public Element
{
public:
	Expr(ASTreeRef ref, ParserRef exp, Operator &op);
	virtual void parse(Lexer &lexer, std::vector<ASTreeRef> &res)override;
	virtual bool match(Lexer &lexer)override;
	virtual ASTreeRef doShift(Lexer &lexer,ASTreeRef left,int prec);
	virtual Precedence nextOperator(Lexer &lexer);
	static bool rightIsExpr(int prec, Precedence &nextPrec);
protected:
	struct exprData;
	std::unique_ptr<exprData> data;
};

using ElementRef = std::shared_ptr<Element>;
using FactoryRef = std::shared_ptr<Factory>;
