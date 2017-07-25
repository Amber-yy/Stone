#pragma once

#include <memory>
#include <vector>
#include <string>

class Token;
class ASTree;
using ASTreeRef = std::shared_ptr<ASTree>;
using Iterator = std::vector<ASTreeRef>::iterator;
using TokenRef = std::shared_ptr <Token>;

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
};

class Name :public ASTLeaf
{
public:
	Name(TokenRef &t);
	virtual ~Name();
	virtual std::string name();
};

class BinaryExpr :public ASTList
{
public:
	BinaryExpr(std::vector<ASTreeRef> &list);
	virtual ~BinaryExpr();
	virtual ASTreeRef left();
	virtual std::string op();
	virtual ASTreeRef right();
};

class PrimaryExpr :public ASTList
{
public:
	PrimaryExpr(std::vector<ASTreeRef> &ref);
};

class NegativeExpr :public ASTList
{
public:
	NegativeExpr(std::vector<ASTreeRef> &ref);
	ASTreeRef operand();
	virtual std::string toString()override;
};

class BlockStmnt :public ASTList
{
public:
	BlockStmnt(std::vector<ASTreeRef> &ref);
};

class IfStmnt :public ASTList
{
public:
	IfStmnt(std::vector<ASTreeRef> &ref);
	virtual ASTreeRef condition();
	virtual ASTreeRef thenBlock();
	virtual ASTreeRef elseBlock();
	virtual std::string toString()override;
};

class WhileStmnt :public ASTList
{
public:
	WhileStmnt(std::vector<ASTreeRef> &ref);
	virtual ASTreeRef condition();
	virtual ASTreeRef body();
	virtual std::string toString()override;
};

class NullStmnt :public ASTList
{
public:
	NullStmnt(std::vector<ASTreeRef> &ref);
};

class StringLiteral :public ASTLeaf
{
public:
	StringLiteral(TokenRef t);
	virtual std::string value();
};