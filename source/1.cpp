#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>

using std::cout;
using std::endl;

std::map<std::string, int> vars;

enum OPERATOR {
	LBRACKET, RBRACKET,
	ASSIGN,
	PLUS, MINUS,
	MULTIPLY
};

std::string OPERTEXT[] {
	"(", ")",
	"=",
	"+", "-",
	"*"
};

enum LEXEMTYPE {
	NUM, OPER, VAR
};

int PRIORITY[] = {
	-1, -1,
	0,
	1, 1,
	2
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

class Variable: public Lexem {
	std::string name;
public:
	Variable(const std::string &name);
	std::string getName();
	int getValue();
	void setValue(int value);
};

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

class Number: public Lexem {
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
	return 0;
}

int Oper::getValue(Lexem *left, Lexem *right) {
	if (opertype == ASSIGN) {
		if (left->getLexemtype() == VAR && right->getLexemtype() == VAR) {
			vars[static_cast<Variable *>(left)->getName()] = static_cast<Variable *>(right)->getValue();
			return static_cast<Variable *>(right)->getValue();
		} 
		if (left->getLexemtype() == VAR && right->getLexemtype() == NUM) {
			vars[static_cast<Variable *>(left)->getName()] = static_cast<Number *>(right)->getValue();
			return static_cast<Number *>(right)->getValue();
		}
		if (left->getLexemtype() == NUM && right->getLexemtype() == VAR) {
			return static_cast<Variable *>(right)->getValue();			
		}
		if (left->getLexemtype() == NUM && right->getLexemtype() == NUM) {
			return static_cast<Number *>(right)->getValue();
		}
	}
	if (left->getLexemtype() == VAR && right->getLexemtype() == VAR) 
		return evalNum(static_cast<Variable *>(left)->getValue(), static_cast<Variable *>(right)->getValue());
	if (left->getLexemtype() == VAR && right->getLexemtype() == NUM) {
		return evalNum(static_cast<Variable *>(left)->getValue(), static_cast<Number *>(right)->getValue());
	}
	if (left->getLexemtype() == NUM && right->getLexemtype() == VAR) {
		return evalNum(static_cast<Number *>(left)->getValue(), static_cast<Variable *>(right)->getValue());
	}
	return evalNum(static_cast<Number *>(left)->getValue(), static_cast<Number *>(right)->getValue());
}

Lexem *checkNum(std::string codeline, int &i) {
	if (isdigit(codeline[i])) {
			int number = 0;
			while (isdigit(codeline[i])) {
				number = number * 10 + codeline[i] - '0';
				i++;
			}
			i--;
			return new Number(number);
		}
	return nullptr;
}

Lexem *checkOper(std::string codeline, int &i) {
	size_t size = sizeof(OPERTEXT) / sizeof(std::string); 
	for (int j = 0; j < size; j++) {
		std::string subcod = codeline.substr(i, OPERTEXT[j].size());
		if (OPERTEXT[j] == subcod) {
			i += OPERTEXT[j].size() - 1;
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
		i--;
		return new Variable(tmp);
	}
	return nullptr;
}

std::vector<Lexem *> parseLexem(std::string codeline) {
	std::vector<Lexem *> infix;
	int i;
	Lexem *ptr;
	for (i = 0; i < codeline.size(); i++) {
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
	}
	return infix;
}

void print(std::vector<Lexem *> vec) {
	for (int i = 0; i < vec.size(); i++) {
		if ((int)(vec[i]->getLexemtype())) {
			cout << "oper is: " << OPERTEXT[(int)(((Oper *)vec[i])->getType())] << endl;
		}
		else cout << "number is: " << ((Number *)vec[i])->getValue() << endl;
	}
}

std::vector<Lexem *> buildPoliz(std::vector<Lexem *> infix) {
	int i;
	std::vector<Lexem *> postfix;
	std::stack<Oper *> stack;
	for (i = 0; i < infix.size(); i++) {
		if (infix[i]->getLexemtype() == NUM) {
			postfix.push_back(infix[i]);
		}
		if (infix[i]->getLexemtype() == VAR) {
			postfix.push_back(infix[i]);
			continue;
		}
		if (infix[i]->getLexemtype() == OPER) {
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
			if (stack.empty()) {
				stack.push(static_cast<Oper *>(infix[i]));
				continue;
			}
			Oper* x = stack.top();
			if (static_cast<Oper *>(infix[i])->getPriority() <= x->getPriority()) {
				postfix.push_back(x);
				stack.pop();
				stack.push(static_cast<Oper *>(infix[i]));
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
	return postfix;
}


int evaluatePoliz(std::vector<Lexem *> postfix) {
	std::stack<Lexem *> stack;
	for (int i = 0; i < postfix.size(); i++) {
		if (postfix[i]->getLexemtype() == NUM || postfix[i]->getLexemtype() == VAR) {
			stack.push(postfix[i]);
		}
		else {
			Lexem *right = stack.top();
			stack.pop();
			Lexem *left = stack.top(); 
			stack.pop();
			Oper *ptr = static_cast<Oper *>(postfix[i]);
			int n = ptr->getValue(left, right);
			stack.push(new Number(n));
		}
	}
	Number *ans = static_cast<Number *>(stack.top());
	stack.pop();
	return ans->getValue();
}

int main() {
	std::string codeline;
	std::vector<Lexem *> infix;
	std::vector<Lexem *> postfix;
	int value;

	while (std::getline(std::cin, codeline)) {
		infix = parseLexem(codeline);
		postfix = buildPoliz(infix);
		value = evaluatePoliz(postfix);
		std::cout << value << std::endl;

	}
	return 0;
}

