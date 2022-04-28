#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>

using std::cout;
using std::endl;

std::map<std::string, int> vars;
std::map<std::string, int> labels;

enum OPERATOR {
	IF, THEN,
	ELSE, ENDIF,
	WHILE, ENDWHILE,
	GOTO, ASSIGN, COLON,
	LBRACKET, RBRACKET,
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

std::string OPERTEXT[] {
	"if", "then",
	"else", "endif",
	"while", "endwhile",
	"goto", "=", ":",
	"(", ")",
	"or",
	"and",
	"|",
	"^",
	"&",
	"==", "!=",
	"<<", ">>",
	"<=", "<", ">=", ">",
	"+", "-",
	"*", "/", "%"
};

enum LEXEMTYPE {
	NUM, OPER, VAR
};

int PRIORITY[] = {
	-2, -2,
	-2, -2,
	-2, -2,
	-2, 0, -2,
	-1, -1,
	1,
	2,
	3,
	4,
	5,
	6, 6,
	8, 8,
	7, 7, 7, 7,
	9, 9,
	10, 10, 10
};

class Lexem {
	LEXEMTYPE type;
public:
	Lexem();
	void setLexemType(LEXEMTYPE type);
	LEXEMTYPE getLexemtype();
	virtual ~Lexem() {};
};

void Lexem::setLexemType(LEXEMTYPE type) {
	this->type = type;
}

LEXEMTYPE Lexem::getLexemtype() {
	return type;
}

Lexem::Lexem() {

}

class Number;
class Oper;
class Item;

class Oper: public Lexem {
	OPERATOR opertype;
public:
	Oper(OPERATOR op);
	OPERATOR getType();
	int getPriority();
	int evalNum(int left, int right);
	int getValue(Lexem* left, Lexem* right);
	void print(OPERATOR op);
};

class Item: public Lexem {
public:
	virtual int getValue() = 0;
	virtual ~Item() {};
};

class Variable: public Item {
	std::string name;
public:
	Variable(const std::string &name);
	std::string getName();
	int getValue();
	void setValue(int value);
	int getRow();
	bool exists() {
		return labels.find(name) != labels.end();
	}
};

int Variable::getRow() {
	return labels[name];
}

Variable::Variable(const std::string &name) {
	this->name = name;
	setLexemType(VAR);
	//vars[name] = 0;
}

std::string Variable::getName() {
	return name;
}

int Variable::getValue() {
	return vars[name];
}

void Variable::setValue(int value) {
	vars[name] = value;
}

Oper::Oper(OPERATOR op) {
	setLexemType(OPER);
	opertype = op;
}

OPERATOR Oper::getType() {
	return opertype;
}

void Oper::print(OPERATOR op) {
	cout << op << endl;
}

int Oper::getPriority() {
	return PRIORITY[opertype];
}

class Number: public Item {
	int value;
public:
	Number(int value);
	void print(int value);
	int getValue();
};

Number::Number(int val) {
	setLexemType(NUM);
	value = val;
}

int Number::getValue() {
	return value;
}

void Number::print(int value) {
	std::cout << value << std::endl; 
}

class Goto: public Oper {
	int row;
public:
	enum { UNDEFINED = -INT32_MAX };
	Goto(OPERATOR opertype): Oper(opertype) {
		row = UNDEFINED;
	}
	void setRow(int row) {
		Goto::row = row;
	}
	int getRow() {
		return row;
	}
	// void print() {
	// 	cout << "[<row " << row << ">" << OPERTEXT[opertype] << "] ";
	// }
};

int Oper::evalNum(int left, int right) {
	if (opertype == MULTIPLY) {
		return left * right;
	}	
	if (opertype == MINUS) {
		return left - right;
	}
	if (opertype == PLUS) {
		return left + right;
	}
	if (opertype == MOD) {
		return left % right;
	}
	if (opertype == DIV) {
		return left / right;
	}
	if (opertype == OR) {
		return left or right;
	}
	if (opertype == AND) {
		return left and right;
	} 
	if (opertype == BITOR) {
		return left | right;
	}
	if (opertype == XOR) {
		return left ^ right;
	}
	if (opertype == BITAND) {
		return left & right;
	}
	if (opertype == NEQ) {
		return left != right;
	}
	if (opertype == LEQ) {
		return left <= right;
	}
	if (opertype == EQ) {
		return left == right;
	}
	if (opertype == LT) {
		return left < right;
	}
	if (opertype == GEQ) {
		return left >= right;
	}
	if (opertype == GT) {
		return left > right;
	}
	if (opertype == SHL) {
		return left << right;
	}
	if (opertype == SHR) {
		return left >> right;
	}
	return 0;
}

int Oper::getValue(Lexem *left, Lexem *right) {
	if (opertype == ASSIGN) {
		static_cast<Variable *>(left)->setValue(static_cast<Item *>(right)->getValue());
		return static_cast<Item *>(right)->getValue();
	} 
	int leftArg = static_cast<Item *>(left)->getValue();
	int rightArg = static_cast<Item *>(right)->getValue();
	return evalNum(leftArg, rightArg);
}

Lexem *checkNum(std::string codeline, int &i) {
	if (isdigit(codeline[i])) {
			int number = 0;
			while (isdigit(codeline[i])) {
				number = number * 10 + codeline[i] - '0';
				i++;
			}
			return new Number(number);
		}
	return nullptr;
}

Lexem *checkOper(std::string codeline, int &i) {
	size_t size = sizeof(OPERTEXT) / sizeof(std::string); 
	for (int j = 0; j < size; j++) {
		std::string subcod = codeline.substr(i, OPERTEXT[j].size());
		if (OPERTEXT[j] == subcod) {
			i += OPERTEXT[j].size();
			if (j == GOTO || j == IF || j == ELSE || j == WHILE || j == ENDWHILE || j == ENDIF)
				return new Goto(OPERATOR(j));
			return new Oper(OPERATOR(j));
		}
	}
	return nullptr;
}


Lexem *checkVars(std::string codeline, int &i) {
	if (isalpha(codeline[i])) {
		std::string tmp;
		while (isalpha(codeline[i])) {
			tmp.push_back(codeline[i]);
			i++;
		}
		return new Variable(tmp);
	}
	return nullptr;
}

std::vector<Lexem *> parseLexem(std::string codeline) {
	std::vector<Lexem *> infix;
	int i;
	Lexem *ptr;
	for (i = 0; i < codeline.size(); ) {
		if (ptr = checkNum(codeline, i)) {
			infix.push_back(ptr);
			continue;
		}
		if (ptr = checkOper(codeline, i)) {
			infix.push_back(ptr);
			continue;
		}
		if (ptr = checkVars(codeline, i)) {
			infix.push_back(ptr);
			continue;
		}
		i++;
	}
	return infix;
}


void print(std::vector<Lexem *> vec) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i]->getLexemtype() == NUM) {
			cout << "[" << static_cast<Number *>(vec[i])->getValue() << "] ";
		}
		if (vec[i]->getLexemtype() == OPER) {
			Oper *lexemop = static_cast<Oper *>(vec[i]);
			cout << "[" << OPERTEXT[(int)(lexemop->getType())] << "]"; 
		}
		if (vec[i]->getLexemtype() == VAR) {
			cout << "[" << static_cast<Variable *>(vec[i])->getName() <<  "(" << static_cast<Variable *>(vec[i])->getRow() << ")" << "] ";
		}
	}
	cout << endl;
}

std::vector<Lexem *> buildPoliz(std::vector<Lexem *> infix) {
	int i;
	std::vector<Lexem *> postfix;
	std::stack<Oper *> stack;
	for (i = 0; i < infix.size(); i++) {
		if (infix[i] == nullptr) {
			continue;
		}
		if (infix[i]->getLexemtype() == NUM) {
			postfix.push_back(infix[i]);
		}
		if (infix[i]->getLexemtype() == VAR) {
			postfix.push_back(infix[i]);
			continue;
		}
		if (infix[i]->getLexemtype() == OPER) {
			if (stack.empty()) {
				stack.push(static_cast<Oper *>(infix[i]));
				continue;
			}
			if (static_cast<Oper *>(infix[i])->getType() == LBRACKET) {
				stack.push(static_cast<Oper*>(infix[i]));
				continue;
			}
			if (static_cast<Oper *>(infix[i])->getType() == RBRACKET) {
				while (stack.top()->getType() != LBRACKET) {
					postfix.push_back(stack.top());
					stack.pop();
				}
				stack.pop();
				continue;
			}
			OPERATOR type = static_cast<Oper *>(infix[i])->getType();
			if (type == GOTO || type == WHILE || type == IF || type == ELSE || type == ENDWHILE) {
				if (type == GOTO) {
					static_cast<Goto *>(infix[i])->setRow(labels[static_cast<Variable *>(infix[i + 1])->getName()]);
				}
				stack.push(static_cast<Oper *>(infix[i]));
				continue;
			}
			if (type == ENDIF || type == THEN) {
				continue;
			}
			Oper* x = stack.top();
			if (static_cast<Oper *>(infix[i])->getPriority() < x->getPriority()) {
				while (!stack.empty() && x->getType() != LBRACKET) {
					x = stack.top();
					postfix.push_back(x);
					stack.pop();
				}
				stack.push(static_cast<Oper *>(infix[i]));
				continue;
			}
			if (static_cast<Oper *>(infix[i])->getPriority() == x->getPriority()) {
				postfix.push_back(x);
				stack.pop();
				stack.push(static_cast<Oper *>(infix[i]));
				continue;
			}
			if (static_cast<Oper *>(infix[i])->getPriority() > x->getPriority()) {
				stack.push(static_cast<Oper *>(infix[i]));
			}
		}
	}
	if (stack.size() != 0) {
		while (stack.size() != 0) {
			postfix.push_back(stack.top());
			stack.pop();
		}
	}
	//print(postfix);
	return postfix;
}


int evaluatePoliz(std::vector<Lexem *> postfix, int &row) {
	std::stack<Lexem *> stack;
	for (int i = 0; i < postfix.size(); i++) {
		if (postfix[i] == nullptr) {
			continue;
		}
		if (postfix[i]->getLexemtype() == NUM || postfix[i]->getLexemtype() == VAR) {
			stack.push(postfix[i]);
		}
		else {
			if (static_cast<Oper *>(postfix[i])->getType() == GOTO 
					|| static_cast<Oper *>(postfix[i])->getType() == ELSE 
						|| static_cast<Oper *>(postfix[i])->getType() == ENDWHILE) {
				return static_cast<Goto *>(postfix[i - 1])->getRow();
			}
			if (static_cast<Oper *>(postfix[i])->getType() == IF
					|| static_cast<Oper *>(postfix[i])->getType() == WHILE) {
				Goto *lexemgoto = static_cast<Goto *>(postfix[i]);
				int r = static_cast<Number *>(stack.top())->getValue();
				stack.pop();
				if (!r) {
					return lexemgoto->getRow();
				}
				else {
					return row + 1;
				}
			}
			Lexem *right = stack.top();
			stack.pop();
			Lexem *left = stack.top(); 
			stack.pop();
			Oper *ptr = static_cast<Oper *>(postfix[i]);
			int n = ptr->getValue(left, right);
			stack.push(new Number(n));
		}
	}
	if (stack.size()) { 
		Number *ans = static_cast<Number *>(stack.top());
		stack.pop();
		cout <<  "answer is: " << ans->getValue() << endl;
	}
	return row + 1;
}

void initLabels(std::vector<Lexem *> &infix, int row) {
	for (int i = 1; i < (int)infix.size(); i++) {
		if (infix[i - 1]->getLexemtype() == VAR && infix[i]->getLexemtype() == OPER) {
			Variable *lexemvar = static_cast<Variable *>(infix[i-1]);
			Oper *lexemop = static_cast<Oper *>(infix[i]);
			if (lexemop->getType() == COLON) {
				cout << "T" << endl;
				if (!lexemvar->exists()) {
					labels.insert(std::pair<std::string, int>(lexemvar->getName(), row));
				}
				labels[lexemvar->getName()] = row;
				delete infix[i - 1];
				delete infix[i];
				infix[i - 1] = nullptr;
				infix[i] = nullptr;
				i++;
			}
		} 
	} 
}

void initJumps(std::vector< std::vector <Lexem *> > infixes) {
	std::stack<Goto *> stackIfElse;
	std::stack<Goto *> stackWhile;
	for (int row = 0; row < (int)infixes.size(); row++) {
		for (int i = 0; i < (int)infixes[row].size(); i++) {
			if (infixes[row][i] == nullptr)
				continue;
			if (infixes[row][i]->getLexemtype() == OPER) {
				Oper *lexemop = (Oper *)infixes[row][i];
				
				if (lexemop->getType() == WHILE) {
					Goto *lexemgoto = (Goto *)lexemop;
					lexemgoto->setRow(row);
					stackWhile.push(lexemgoto);
				}

				if (lexemop->getType() == ENDWHILE) {
					Goto *lexemgoto = (Goto *)lexemop;
					lexemgoto->setRow(stackWhile.top()->getRow());
					stackWhile.top()->setRow(row + 1);
					stackWhile.pop();
				}

				if (lexemop->getType() == IF)
					stackIfElse.push((Goto *)lexemop);
				
				if (lexemop->getType() == ELSE) {
					stackIfElse.top()->setRow(row + 1);
					stackIfElse.pop();
					stackIfElse.push((Goto *)lexemop);
				}

				if (lexemop->getType() == ENDIF) {
					stackIfElse.top()->setRow(row + 1);
					stackIfElse.pop();
				}
			}
		}
	}
}

int main() {
	std::string codeline;
	std::vector<std::vector<Lexem *>> infixLines, postfixLines;

	while (std::getline(std::cin, codeline)) {
		infixLines.push_back(parseLexem(codeline));
	}	

	for (int i = 0; i < infixLines.size(); i++) {		
 		print(infixLines[i]);
	}

	initJumps(infixLines);

	for (int row = 0; row < infixLines.size(); row++) {
		initLabels(infixLines[row], row);
	}

	for (const auto &infix: infixLines) {
	 	postfixLines.push_back(buildPoliz(infix));
	}


	int row = 0;
	while (0 <= row && row < postfixLines.size()) {
		row = evaluatePoliz(postfixLines[row], row);
	}

	return 0;
}

