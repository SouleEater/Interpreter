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
};

class Variable: public Item {
	std::string name;
public:
	Variable(const std::string &name);
	std::string getName();
	int getValue();
	void setValue(int value);
	int getRow();
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
			return new Oper((OPERATOR)j);
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

/*void print(std::vector<Lexem *> vec) {
	for (int i = 0; i < vec.size(); i++) {
		if ((int)(vec[i]->getLexemtype())) {
			cout << "oper is: " << OPERTEXT[(int)(((Oper *)vec[i])->getType())] << endl;
		}
		else cout << "number is: " << ((Number *)vec[i])->getValue() << endl;
	}
}*/

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
			if (static_cast<Oper *>(postfix[i])->getType() == GOTO) {
				return static_cast<Variable *>(postfix[i - 1])->getRow();
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

void initLabels(std::vector<Lexem *> &infix, int &row) {
	for (int i = 1; i < (int)infix.size(); i++) {
		if (infix[i - 1]->getLexemtype() == VAR && infix[i]->getLexemtype() == OPER) {
			Variable *lexemvar = static_cast<Variable *>(infix[i-1]);
			Oper *lexemop = static_cast<Oper *>(infix[i]);
			if (lexemop->getType() == COLON) {
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

int main() {
	std::string codeline;
	std::vector<std::vector<Lexem *>> infixLines, postfixLines;

	while (std::getline(std::cin, codeline)) {
		infixLines.push_back(parseLexem(codeline));
	}	
	for (int row = 0; row < infixLines.size(); row++) {
		initLabels(infixLines[row], row);
	}
	for (const auto &infix: infixLines) {
	 	postfixLines.push_back(buildPoliz(infix));
	}
	for (int i = 0; i < postfixLines.size(); i++) {		
 		print(postfixLines[i]);
	}
	int row = 0;
	while (0 <= row && row < postfixLines.size()) {
		row = evaluatePoliz(postfixLines[row], row);
	}
	return 0;
}

