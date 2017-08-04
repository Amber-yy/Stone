#include "ASTree.h"
#include "Token.h"

#include <iostream>

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

Object ASTLeaf::eval(Enviroment & env)
{
	//toto,抛出一个运行时错误
	return Object();
}

TokenRef ASTLeaf::token()
{
	return data->token;
}

ASTList::ASTList(std::vector<ASTreeRef>& list)
{
	data = std::make_unique<listData>();
	data->children = list;
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

Object ASTList::eval(Enviroment & env)
{
	//todo,抛出一个语法异常
	return Object();
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

Object NumberLiteral::eval(Enviroment & env)
{
	Object t;
	t.num = value();
	return t;
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

Object Name::eval(Enviroment & env)
{
	auto it = env.get(name());
	return it->second;
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

Object BinaryExpr::eval(Enviroment & env)
{
	auto o = op();
	if (o == "=")
	{
		Object r = right()->eval(env);
		return computeAssign(env, r);
	}
	else
	{
		Object l = left()->eval(env);
		Object r = right()->eval(env);
		return computeOp(l, o,r);
	}

	return Object();
}

Object BinaryExpr::computeAssign(Enviroment & env, Object &r)
{
	auto l = left();

	if (l->toString() == "outstream")
	{
		if (r.isNum)
		{
			if ((int)r.num == r.num)
			{
				std::cout << std::to_string((int)r.num);
			}
			else
			{
				std::cout << std::to_string(r.num);
			}
		}
		else
		{
			std::cout << r.text;
		}

		return r;
	}

	if (dynamic_cast<Name *>(l.get()))
	{
		env.put(dynamic_cast<Name *>(l.get())->name(), r);
		return r;
	}
	else
	{
		//todo,为非变量赋值
	}

	return Object();
}

Object BinaryExpr::computeOp(Object & l, std::string &op, Object & r)
{
	if (l.isNum&&r.isNum)
	{
		return computeNumber(l, op, r);
	}
	else if(op=="+")
	{
		Object t;
		t.isNum = false;

		std::string t1 = l.isNum ? ((int)l.num==l.num? std::to_string((int)l.num) :std::to_string(l.num) ): l.text;
		std::string t2 = r.isNum ? ((int)r.num == r.num ? std::to_string((int)r.num) : std::to_string(r.num)) : r.text;

		t.text = t1 + t2;

		return t;
	}
	else if(op=="==")
	{
		Object t;
		t.num = (l == r);

		return t;
	}
	else
	{
		//todo,运行时语法错误
	}

	return Object();
}

Object BinaryExpr::computeNumber(Object & l, std::string & op, Object & r)
{
	double a = l.num;
	double b = r.num;

	Object t;

	if (op == "+")
	{
		t.num = a + b;
	}
	else if (op == "-")
	{
		t.num = a - b;
	}
	else if (op == "*")
	{
		t.num = a * b;
	}
	else if (op == "/")
	{
		t.num = a/b;//todo，除数不能为0
	}
	else if (op == "%")
	{
		t.num = (int)a % (int)b;
	}
	else if (op == "==")
	{
		t.num = (a == b);
	}
	else if (op == ">")
	{
		t.num = (a > b);
	}
	else if (op == "<")
	{
		t.num = (a < b);
	}
	else
	{
		//todo,运行时错误
	}

	return t;
}

PrimaryExpr::PrimaryExpr(std::vector<ASTreeRef>& ref):ASTList(ref)
{
}

Object PrimaryExpr::eval(Enviroment & env)
{
	return Object();
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

Object NegativeExpr::eval(Enviroment & env)
{
	Object result=operand()->eval(env);

	if (result.isNum)
	{
		result.num=-result.num;
	}

	return result;
}

BlockStmnt::BlockStmnt(std::vector<ASTreeRef>& ref) :ASTList(ref)
{
}

Object BlockStmnt::eval(Enviroment & env)
{
	Object result;
	for (auto t : data->children)
	{
		if (!dynamic_cast<NullStmnt *>(t.get()))
		{
			result = t->eval(env);
		}
	}

	return result;
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
	std::string result = "(if " + condition()->toString() + " " + thenBlock()->toString();
	if (elseBlock().get() != nullptr)
	{
		result+= " else " + elseBlock()->toString() ;
	}

	result += ")";

	return result;
}

Object IfStmnt::eval(Enviroment & env)
{
	Object c = condition()->eval(env);
	if (c.isNum&&c.num != 0)
	{
		return thenBlock()->eval(env);
	}
	else
	{
		auto b = elseBlock();
		if (b.get() == nullptr)
		{
			return Object();
		}
		
		return b->eval(env);
	}

	return Object();
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
	return  "(while " + condition()->toString() + " " + body()->toString() + ")";
}

Object WhileStmnt::eval(Enviroment & env)
{
	Object t;

	auto s = condition()->toString();

	for (;;)
	{
		Object c = condition()->eval(env);
		if (c.isNum&&c.num == 0)
		{
			return t;
		}
		else
		{
			t = body()->eval(env);
		}
	}

	return t;
}

NullStmnt::NullStmnt(std::vector<ASTreeRef>& ref) :ASTList(ref)
{
}

Object NullStmnt::eval(Enviroment & env)
{
	return Object();
}

StringLiteral::StringLiteral(TokenRef t):ASTLeaf(t)
{
}

std::string StringLiteral::value()
{
	return data->token->getText();
}

Object StringLiteral::eval(Enviroment & env)
{
	Object t;
	t.isNum = false;
	t.text = value();
	return t;
}

Enviroment::Enviroment()
{
}

void Enviroment::put(std::string& name, Object &value)
{
	values[name] = value;
}

std::map<std::string, Object>::iterator Enviroment::get(std::string &name)
{
	auto result= values.find(name);
	if (result == values.end())
	{
		//todo
	}
	return result;
}
bool Object::operator==(const Object & o)
{
	return (isNum==o.isNum)&&(num==o.num)&&(text==o.text);
}
