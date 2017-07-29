#include "ASTree.h"
#include "Token.h"

struct ASTLeaf::leafData
{
	TokenRef token;
};

struct ASTList::listData
{
	std::vector<ASTreeRef> children;
};

ASTree::ASTree()
{
}

ASTree::~ASTree()
{
}

Iterator ASTree::iterator()
{
	return children();
}

ASTLeaf::ASTLeaf(TokenRef & t)
{
	data = std::make_unique<leafData>();
	data->token = t;
}

ASTLeaf::~ASTLeaf()
{
}

ASTreeRef ASTLeaf::child(int index)
{
	/*
	todo，抛出一个异常
	*/
	return ASTreeRef();
}

int ASTLeaf::numChildren()
{
	return 0;
}

Iterator ASTLeaf::children()
{
	return Iterator();
}

std::string ASTLeaf::location()
{
	return std::string("at line")+std::to_string(data->token->getLineNumber());
}

std::string ASTLeaf::toString()
{
	return data->token->getText();
}

TokenRef ASTLeaf::token()
{
	return data->token;
}

ASTList::ASTList(std::vector<ASTreeRef>& list)
{
	data = std::make_unique<listData>();
	data->children = std::move(list);
}

ASTList::~ASTList()
{
}

ASTreeRef ASTList::child(int index)
{
	return data->children[index];
}

int ASTList::numChildren()
{
	return data->children.size();
}

Iterator ASTList::children()
{
	return data->children.begin();
}

std::string ASTList::location()
{
	std::string temp;
	for (ASTreeRef &ref : data->children)
	{
		temp = ref->location();
		if (temp.length())
		{
			break;
		}
	}

	return temp;
}

std::string ASTList::toString()
{
	std::string temp = "(";
	std::string sep = "";

	for (ASTreeRef &ref : data->children)
	{
		temp.append(sep);
		sep = " ";
		temp.append(ref->toString());
	}

	temp += ")";

	return temp;

}

NumberLiteral::NumberLiteral(TokenRef & t):ASTLeaf(t)
{
}

NumberLiteral::~NumberLiteral()
{
}

int NumberLiteral::value()
{
	return token()->getNumber();
}

Name::Name(TokenRef & t) :ASTLeaf(t)
{
}

Name::~Name()
{
}

std::string Name::name()
{
	return token()->getText();
}

BinaryExpr::BinaryExpr(std::vector<ASTreeRef>& list):ASTList(list)
{
}

BinaryExpr::~BinaryExpr()
{
}

ASTreeRef BinaryExpr::left()
{
	return child(0);
}

std::string BinaryExpr::op()
{
	return dynamic_cast<ASTLeaf *>(child(1).get())->token()->getText();
}

ASTreeRef BinaryExpr::right()
{
	return child(2);
}

PrimaryExpr::PrimaryExpr(std::vector<ASTreeRef>& ref):ASTList(ref)
{
}

NegativeExpr::NegativeExpr(std::vector<ASTreeRef>& ref) : ASTList(ref)
{
}

ASTreeRef NegativeExpr::operand()
{
	return data->children[0];
}

std::string NegativeExpr::toString()
{
	return "-"+operand()->toString();
}

BlockStmnt::BlockStmnt(std::vector<ASTreeRef>& ref) :ASTList(ref)
{
}

IfStmnt::IfStmnt(std::vector<ASTreeRef>& ref) : ASTList(ref)
{
}

ASTreeRef IfStmnt::condition()
{
	return data->children[0];
}

ASTreeRef IfStmnt::thenBlock()
{
	return data->children[1];
}

ASTreeRef IfStmnt::elseBlock()
{
	return numChildren()>2?data->children[2]:ASTreeRef();
}

std::string IfStmnt::toString()
{
	return "(if " + condition()->toString() + " " + thenBlock()->toString() + " else " + elseBlock()->toString() + ")";
}

WhileStmnt::WhileStmnt(std::vector<ASTreeRef>& ref) :ASTList(ref)
{
}

ASTreeRef WhileStmnt::condition()
{
	return data->children[0];
}

ASTreeRef WhileStmnt::body()
{
	return data->children[1];
}

std::string WhileStmnt::toString()
{
	return "(while " + condition()->toString() + " " + body()->toString() + ")";
}

NullStmnt::NullStmnt(std::vector<ASTreeRef>& ref) :ASTList(ref)
{
}

StringLiteral::StringLiteral(TokenRef t):ASTLeaf(t)
{
}

std::string StringLiteral::value()
{
	return data->token->getText();
}
