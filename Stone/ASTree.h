#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

class Token;
class ASTree;
using ASTreeRef = std::shared_ptr<ASTree>;
using Iterator = std::vector<ASTreeRef>::iterator;
using TokenRef = std::shared_ptr <Token>;

struct Object
{
	Object():isNum(true),num(0){}
	bool operator==(const Object &o);
	bool isNum;
	double num;
	std::string text;
};

class Enviroment
{
public:
	Enviroment();
	void put(std::string &name, Object &value);
	std::map<std::string, Object>::iterator get(std::string &name);
protected:
	std::map<std::string, Object> values;
};

class ASTree
{
public:
	ASTree();
	virtual ~ASTree();
	virtual ASTreeRef child(int index) = 0;
	virtual int numChildren()=0;
	virtual Iterator children() = 0;
	virtual std::string location() = 0;
	virtual std::string toString() = 0;
	virtual Object eval(Enviroment &env) = 0;
	virtual Iterator iterator();
};

class ASTLeaf :public ASTree
{
public:
	ASTLeaf(TokenRef &t);
	~ASTLeaf();
	virtual ASTreeRef child(int index) override;
	virtual int numChildren()override;
	virtual Iterator children()override;
	virtual std::string location()override;
	virtual std::string toString()override;
	virtual Object eval(Enviroment &env) override;
	virtual TokenRef token();
protected:
	struct leafData;
	std::unique_ptr<leafData> data;
};

class ASTList :public ASTree
{
public:
	ASTList(std::vector<ASTreeRef> &list);
	virtual ~ASTList();
	virtual ASTreeRef child(int index) override;
	virtual int numChildren()override;
	virtual Iterator children()override;
	virtual std::string location()override;
	virtual std::string toString()override;
	virtual Object eval(Enviroment &env) override;
protected:
	struct listData;
	std::unique_ptr<listData> data;
};

class NumberLiteral :public ASTLeaf
{
public:
	NumberLiteral(TokenRef &t);
	virtual ~NumberLiteral();
	virtual int value();
	virtual Object eval(Enviroment &env) override;
};

class Name :public ASTLeaf
{
public:
	Name(TokenRef &t);
	virtual ~Name();
	virtual std::string name();
	virtual Object eval(Enviroment &env) override;
};

class BinaryExpr :public ASTList
{
public:
	BinaryExpr(std::vector<ASTreeRef> &list);
	virtual ~BinaryExpr();
	virtual ASTreeRef left();
	virtual std::string op();
	virtual ASTreeRef right();
	virtual Object eval(Enviroment &env) override;
	virtual Object computeAssign(Enviroment &env, Object &r);
	virtual Object computeOp(Object &l, std::string &op, Object &r);
	virtual Object computeNumber(Object &l, std::string &op, Object &r);
};

class PrimaryExpr :public ASTList
{
public:
	PrimaryExpr(std::vector<ASTreeRef> &ref);
	virtual Object eval(Enviroment &env) override;
};

class NegativeExpr :public ASTList
{
public:
	NegativeExpr(std::vector<ASTreeRef> &ref);
	ASTreeRef operand();
	virtual std::string toString()override;
	virtual Object eval(Enviroment &env) override;
};

class BlockStmnt :public ASTList
{
public:
	BlockStmnt(std::vector<ASTreeRef> &ref);
	virtual Object eval(Enviroment &env) override;
};

class IfStmnt :public ASTList
{
public:
	IfStmnt(std::vector<ASTreeRef> &ref);
	virtual ASTreeRef condition();
	virtual ASTreeRef thenBlock();
	virtual ASTreeRef elseBlock();
	virtual std::string toString()override;
	virtual Object eval(Enviroment &env) override;
};

class WhileStmnt :public ASTList
{
public:
	WhileStmnt(std::vector<ASTreeRef> &ref);
	virtual ASTreeRef condition();
	virtual ASTreeRef body();
	virtual std::string toString()override;
	virtual Object eval(Enviroment &env) override;
};

class NullStmnt :public ASTList
{
public:
	NullStmnt(std::vector<ASTreeRef> &ref);
	virtual Object eval(Enviroment &env) override;
};

class StringLiteral :public ASTLeaf
{
public:
	StringLiteral(TokenRef t);
	virtual std::string value();
	virtual Object eval(Enviroment &env) override;
};