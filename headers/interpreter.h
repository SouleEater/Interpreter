#ifndef INTLIB_HPP
#define INTLIB_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>


class Number;
class Oper;
class Item;

enum OPERATOR {
	IF, THEN,
	ELSE, ENDIF,
	WHILE, ENDWHILE,
	GOTO, ASSIGN, COLON,
	ARRAY,
	LBRACKET, RBRACKET,
	LQBRACKET, RQBRACKET,
	OR,
	AND,
	BITOR,
	XOR,
	BITAND,
	EQ, NEQ,
	SHL, SHR,
	LEQ, LT, GEQ, GT,
	PLUS, MINUS,
	MULTIPLY, DIV, MOD
};

enum LEXEMTYPE {
	NUM, OPER, VAR
};

class Lexem {
	LEXEMTYPE type;
public:
	Lexem();
	void setLexemType(LEXEMTYPE type);
	LEXEMTYPE getLexemtype();
	virtual ~Lexem() { /*cout << "~Lexem" << endl;*/ };
};

class Oper: public Lexem {
	OPERATOR opertype;
public:
	Oper(OPERATOR op);
	OPERATOR getType();
	int getPriority();
	int evalNum(int left, int right);
	Lexem *getValue(Lexem* left, Lexem* right);
	void print(OPERATOR op);
};

class Item: public Lexem {
public:
	Item() { /*cout << "Item" << endl; */};
	virtual int getValue() = 0;
	virtual ~Item() { /*cout << "~Item" << endl;*/ };
};

class ArrayElem: public Lexem {
	std::string name;
	int index;
public:
	ArrayElem(std::string, int);
	int getValue();
	void setValue(int value);
};

class Variable: public Item {
	std::string name;
public:
	Variable(const std::string &name);
	std::string getName();
	int getValue();
	void setValue(int value);
	int getRow();
	bool exists();
};

class Number: public Item {
	int value;
public:
	Number() {/*cout << "Number" << endl;*/ };
	Number(int value);
	void print(int value);
	int getValue();
	~Number() { /*cout << "~Number" << endl;*/ }
};

class Goto: public Oper {
	int row;
public:
	enum { UNDEFINED = -INT32_MAX };
	Goto(OPERATOR opertype);
	void setRow(int row);
	int getRow();
	// void print() {
	// 	cout << "[<row " << row << ">" << OPERTEXT[opertype] << "] ";
	// }
};

Lexem *checkNum(std::string codeline, int &i);

Lexem *checkOper(std::string codeline, int &i);

Lexem *checkVars(std::string codeline, int &i);

std::vector<Lexem *> parseLexem(std::string codeline);

void print(std::vector<Lexem *> vec);

std::vector<Lexem *> buildPoliz(std::vector<Lexem *> infix);

int evaluatePoliz(std::vector<Lexem *> postfix, int &row);

void initLabels(std::vector<Lexem *> &infix, int &row);

void initJumps(std::vector< std::vector <Lexem *> > infixes);

#endif
