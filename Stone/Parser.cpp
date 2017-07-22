#include "Parser.h"
#include "Token.h"

#include <functional>

struct Factory::factoryData
{
	std::function<ASTreeRef(TokenRef &, std::vector<ASTreeRef> &)> maker;
};

struct Tree::treeData
{
	ParserRef parser;
};

struct OrTree::orTreeData
{
	std::vector<ParserRef> parsers;
};

struct AToken::tokenData
{
	tokenData(ASTreeRef ref) :factory(ref){}
	Factory factory;
};

struct IDToken::idData
{
	std::set<std::string> reserved;
};

struct Repeat::repeatData
{
	ParserRef parser;
	bool onlyOnce;
};

struct Leaf::leafData
{
	std::vector<std::string> tokens;
};

struct Precedence::preData
{
	int value;
	bool left;
};

struct Expr::exprData
{
	exprData(ASTreeRef ref):factory(ref){}
	Factory factory;
	Operator op;
	ParserRef factor;
};

struct Parser::parserData
{
	std::vector<ElementRef> elements;
	FactoryRef factory;
};

Parser::Parser()
{
}

Parser::Parser(const Parser & p)
{
	data = std::make_unique<parserData>();
	data->elements = p.data->elements;
	data->factory = p.data->factory;
}

Parser::Parser(ASTreeRef ref)
{
	reset(ref);
}

ASTreeRef Parser::parse(Lexer & lexer)
{
	std::vector<ASTreeRef> results;

	for (auto &e : data->elements)
	{
		e->parse(lexer, results);
	}

	return data->factory->make(TokenRef(), results);
}

bool Parser::match(Lexer & lexer)
{
	if (data->elements.size() == 0)
	{
		return true;
	}

	ElementRef e = data->elements[0];
	
	return e->match(lexer);
}

Parser::~Parser()
{
}

ParserP Parser::reset()
{
	return reset(ASTreeRef());
}

ParserP Parser::reset(ASTreeRef ref)
{
	data->elements.clear();
	data->factory = std::make_shared<Factory>(ref);
	return this;
}

ParserP Parser::number(ASTreeRef ref)
{
	data->elements.push_back(std::make_shared<NUMToken>(ref));
	return this;
}

ParserP Parser::identifier(std::set<std::string>& reserved)
{
	return identifier(ASTreeRef(), reserved);
}

ParserP Parser::identifier(ASTreeRef ref, std::set<std::string>& reserved)
{
	data->elements.push_back(std::make_shared<IDToken>(ref, reserved));
	return this;
}

ParserP Parser::string()
{
	return string(ASTreeRef());
}

ParserP Parser::string(ASTreeRef ref)
{
	data->elements.push_back(std::make_shared<STRToken>(ref));
	return this;
}

ParserP Parser::token(const std::vector<std::string>& pat)
{
	data->elements.push_back(std::make_shared<Leaf>(pat));
	return this;
}

ParserP Parser::sep(const std::vector<std::string>& pat)
{
	data->elements.push_back(std::make_shared<Skip>(pat));
	return this;
}

ParserP Parser::ast(ParserRef ref)
{
	data->elements.push_back(std::make_shared<Tree>(ref));
	return this;
}

ParserP Parser:: or (const std::vector<ParserRef>& p)
{
	data->elements.push_back(std::make_shared<OrTree>(p));
	return this;
}

ParserP Parser::maybe(ParserRef ref)
{
	ParserRef p2 = std::make_shared<Parser>(*ref.get());
	data->elements.push_back(std::make_shared<OrTree>(std::vector<ParserRef>({ ref,p2 })));
	return this;
}

ParserP Parser::option(ParserRef ref)
{
	data->elements.push_back(std::make_shared<Repeat>(ref,true));
	return this;
}

ParserP Parser::repeat(ParserRef ref)
{
	data->elements.push_back(std::make_shared<Repeat>(ref, false));
	return this;
}

ParserP Parser::expression(ParserRef subexp, Operator & operators)
{
	return expression(ASTreeRef(),subexp,operators);
}

ParserP Parser::expression(ASTreeRef ref, ParserRef subexp, Operator & operators)
{
	data->elements.push_back(std::make_shared<Expr>(ref, subexp, operators));
	return this;
}

ParserP Parser::insertChoice(ParserRef p)
{
	ElementRef e = data->elements[0];
	if (dynamic_cast<OrTree *>(e.get()))
	{
		dynamic_cast<OrTree *>(e.get())->insert(p);
	}
	else
	{
		ParserRef other=std::make_shared<Parser>(*this);
		reset();
		or (std::vector<ParserRef>({ p, other }));
	}

	return this;
}

ParserP Parser::rule()
{
	return new Parser(ASTreeRef());
}

ParserP Parser::rule(ASTreeRef ref)
{
	return new Parser(ref);
}

Tree::Tree(ParserRef ref)
{
	data = std::make_unique<treeData>();
	data->parser = ref;
}

Tree::~Tree()
{
}

void Tree::parse(Lexer & lexer, std::vector<ASTreeRef>& res)
{
	res.push_back(data->parser->parse(lexer));
}

bool Tree::match(Lexer & lexer)
{
	return data->parser->match(lexer);
}

OrTree::OrTree(const std::vector<ParserRef>& ref)
{
	data = std::make_unique<orTreeData>();
	data->parsers = ref;
}

OrTree::~OrTree()
{
}

void OrTree::parse(Lexer & lexer, std::vector<ASTreeRef>& res)
{
	ParserRef p = choose(lexer);
	if (p.get() == nullptr)
	{
		//todo:抛出一个语法分析错误
	}
	else
	{
		res.push_back(p->parse(lexer));
	}
}

bool OrTree::match(Lexer & lexer)
{
	return choose(lexer).get()==nullptr;
}

ParserRef OrTree::choose(Lexer & lexer)
{
	for (ParserRef p : data->parsers)
	{
		if (p->match(lexer))
		{
			return p;
		}
	}

	return ParserRef();
}

void OrTree::insert(ParserRef p)
{
	data->parsers.insert(data->parsers.begin(), p);
}

Repeat::Repeat(ParserRef p, bool once)
{
	data = std::make_unique<repeatData>();
	data->parser = p;
	data->onlyOnce = once;
}

void Repeat::parse(Lexer & lexer, std::vector<ASTreeRef>& res)
{
	while (data->parser->match(lexer))
	{
		ASTreeRef t = data->parser->parse(lexer);

		if (dynamic_cast<ASTList *>(t.get()) == nullptr&&t->numChildren() > 0)
		{
			res.push_back(t);
		}
		
		if (data->onlyOnce)
		{
			break;
		}

	}
}

bool Repeat::match(Lexer & lexer)
{
	return data->parser->match(lexer);
}

Factory::Factory(ASTreeRef &t)
{
	data = std::make_unique<factoryData>();

	ASTree *ptr = t.get();
	if (dynamic_cast<BinaryExpr *>(ptr))
	{
		data->maker = [](TokenRef &token, std::vector<ASTreeRef>&ref) {return std::make_shared<BinaryExpr>(std::move(ref)); };
	}
	else if (dynamic_cast<Name *>(ptr))
	{
		data->maker = [](TokenRef &token, std::vector<ASTreeRef>&ref) {return std::make_shared<Name>(token); };
	}
	else if (dynamic_cast<NumberLiteral *>(ptr))
	{
		data->maker = [](TokenRef &token, std::vector<ASTreeRef>&ref) {return std::make_shared<NumberLiteral>(token); };
	}
	else if (dynamic_cast<ASTList *>(ptr))
	{
		data->maker = [](TokenRef &token, std::vector<ASTreeRef>&ref) {return std::make_shared<ASTList>(std::move(ref)); };
	}
	else
	{
		data->maker = [](TokenRef &token, std::vector<ASTreeRef>&ref) {return std::make_shared<ASTLeaf>(token); };
	}
}

ASTreeRef Factory::make(TokenRef &token, std::vector<ASTreeRef>&ref)
{
	return data->maker(token,ref);
}

AToken::AToken(ASTreeRef ref)
{
	data = std::make_unique<tokenData>(ref);
}

void AToken::parse(Lexer & lexer, std::vector<ASTreeRef>& res)
{
	TokenRef t = lexer.read();
	if (test(t))
	{
		ASTreeRef leaf = data->factory.make(t, std::vector<ASTreeRef>());
		res.push_back(leaf);
	}
	else
	{
		//todo:语法解析错误
	}
}

bool AToken::match(Lexer & lexer)
{
	return test(lexer.peek(0));
}

IDToken::IDToken(ASTreeRef ref, const std::set<std::string>& s):AToken(ref)
{
	data = std::make_unique<idData>();
	data->reserved = s;
}

bool IDToken::test(TokenRef t)
{
	return t->isIdentifier()&&(data->reserved.find(t->getText())==data->reserved.end());
}

NUMToken::NUMToken(ASTreeRef ref):AToken(ref)
{
}

bool NUMToken::test(TokenRef t)
{
	return t->isNumber();
}

STRToken::STRToken(ASTreeRef ref):AToken(ref)
{
}

bool STRToken::test(TokenRef t)
{
	return false;
}

Leaf::Leaf(const std::vector<std::string>& pat)
{
	data = std::make_unique<leafData>();
	data->tokens = pat;
}

void Leaf::parse(Lexer & lexer, std::vector<ASTreeRef>& res)
{
	TokenRef t = lexer.read();
	if (t->isIdentifier())
	{
		for (auto &token : data->tokens)
		{
			if (token == t->getText())
			{
				find(res, t);
				return;
			}
		}
	}

	if (data->tokens.size() > 0)
	{
		//data->tokens[0] expected t
	}
	else
	{
		//todo，语法分析错误
	}

}

bool Leaf::match(Lexer & lexer)
{
	TokenRef t = lexer.peek(0);
	if (t->isIdentifier())
	{
		for (auto &token : data->tokens)
		{
			if (token == t->getText())
			{
				return false;
			}
		}
	}
	return false;
}

void Leaf::find(std::vector<ASTreeRef>& ref, TokenRef t)
{
	ref.push_back(std::make_shared<ASTLeaf>(t));
}

Skip::Skip(const std::vector<std::string>& pat):Leaf(pat)
{
}

void Skip::find(std::vector<ASTreeRef>& ref, TokenRef t)
{
}

Precedence::Precedence()
{
}

Precedence::Precedence(const Precedence & prec)
{
	if (prec.data.get() != nullptr)
	{
		data = std::make_unique<preData>();
		*data = *prec.data;
	}
}

Precedence::Precedence(int v, bool a)
{
	data = std::make_unique<preData>();
	data->value = v;
	data->left = a;
}

Precedence & Precedence::operator=(Precedence && pre)
{
	data.swap(pre.data);
	return *this;
}

bool Precedence::left()
{
	return data->left;
}

bool Precedence::ok()
{
	return data.get()!=nullptr;
}

int Precedence::value()
{
	return data->value;
}

void Operator::add(std::string name, int prec, bool left)
{
	operator[](name) = Precedence(prec, left);
}

Expr::Expr(ASTreeRef ref, ParserRef exp, Operator & op)
{
	data = std::make_unique<exprData>(ref);
	data->op = std::move(op);
	data->factor = exp;
}

void Expr::parse(Lexer & lexer, std::vector<ASTreeRef>& res)
{
	ASTreeRef right = data->factor->parse(lexer);
	Precedence prec;

	while ((prec = nextOperator(lexer)).ok())
	{
		right = doShift(lexer, right, prec.value());
	}

	res.push_back(right);
}

bool Expr::match(Lexer & lexer)
{
	return data->factor->match(lexer);
}

ASTreeRef Expr::doShift(Lexer & lexer, ASTreeRef left, int prec)
{
	std::vector<ASTreeRef> list;

	list.push_back(left);
	list.push_back(std::make_shared<ASTLeaf>(lexer.read()));

	ASTreeRef right = data->factor->parse(lexer);

	Precedence next;

	while ((next = nextOperator(lexer)).ok()&&rightIsExpr(prec,next))
	{
		right = doShift(lexer, right, next.value());
	}

	list.push_back(right);

	return data->factory.make(TokenRef(), list);
}

Precedence Expr::nextOperator(Lexer & lexer)
{
	TokenRef t = lexer.peek(0);
	if (t->isIdentifier())
	{
		return data->op.find(t->getText())->second;
	}

	return Precedence();
}

bool Expr::rightIsExpr(int prec, Precedence &nextPrec)
{
	if (nextPrec.left())
	{
		return prec < nextPrec.value();
	}

	return prec <= nextPrec.value();
}
