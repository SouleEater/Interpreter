#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>

using std::cout;
using std::endl;

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
	NUM, OPER
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
	int getValue(Number* left, Number* right);
	void print(OPERATOR op);
};

class Variable: public Lexem {
	std::string name;
public:
	Variable(const std::string &name);
	int getValue();
	void setValue(int value);
};

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

int Oper::getValue(Number* left_ptr, Number* right_ptr) {
	int left = left_ptr->getValue();
	int right = right_ptr->getValue();
	if (opertype == MULTIPLY) {
		return left * right;
	}	
	if (opertype == MINUS) {
		return left - right;
	}
	if (opertype == PLUS) {
		return left + right;
	}

	return 1;
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
	std::stack<Number *> stack;
	for (int i = 0; i < postfix.size(); i++) {
		if (postfix[i]->getLexemtype() == NUM) {
			stack.push(static_cast<Number *>(postfix[i]));
		}
		else {
			Number* right = stack.top();
			stack.pop();
			Number* left = stack.top(); 
			stack.pop();
			Oper* ptr = static_cast<Oper *>(postfix[i]);
			int n = ptr->getValue(left, right);
			stack.push(new Number(n));
		}
	}
	Number* ans = stack.top();
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
		//cout << infix.size() << endl;
		 postfix = buildPoliz(infix);
		 print(postfix);
		value = evaluatePoliz(postfix);
		std::cout << value << std::endl;
	}
	return 0;
}

