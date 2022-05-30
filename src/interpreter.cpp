#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include "interpreter.h"

using std::cout;
using std::endl;

std::map<std::string, int> vars;
std::map<std::string, int> labels;
std::map<std::string, std::vector<int> > ArrayTable;



std::string OPERTEXT[] {
	"if", "then",
	"else", "endif",
	"while", "endwhile",
	"goto", "=", ":",
	"array",
	"(", ")",
	"[", "]",
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

int PRIORITY[] = {
	-2, -2,
	-2, -2,
	-2, -2,
	-2, 0, -2,
	-2,
	-1, -1,
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



void Lexem::setLexemType(LEXEMTYPE type) {
	this->type = type;
}

LEXEMTYPE Lexem::getLexemtype() {
	return type;
}

Lexem::Lexem() {
	// cout << "Lexem" << endl;
}

bool Variable::exists() {
	return labels.find(name) != labels.end();
}

ArrayElem::ArrayElem(std::string name, int index) {
	this->name =  name;
	this->index = index;
}

int ArrayElem::getValue() {
	return ArrayTable[name][index];
}

void ArrayElem::setValue(int value) {
	ArrayTable[name][index] = value;
}



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

Goto::Goto(OPERATOR opertype): Oper(opertype) {
	row = UNDEFINED;
}		

void Goto::setRow(int row) {
	Goto::row = row;
}

int Goto::getRow() {
	return row;
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

Lexem *Oper::getValue(Lexem *left, Lexem *right) {
	if (opertype == ASSIGN) {
		static_cast<Variable *>(left)->setValue(static_cast<Item *>(right)->getValue());
		return right;
	} 
	if (opertype == RQBRACKET) {
		std::string name = static_cast<Variable *>(left)->getName();
		int index = static_cast<Number *>(right)->getValue();
		return new ArrayElem(name, index);
		
	}
	int leftArg = static_cast<Item *>(left)->getValue();
	int rightArg = static_cast<Item *>(right)->getValue();
	return new Number(evalNum(leftArg, rightArg));
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
			if (static_cast<Oper *>(infix[i])->getType() == LBRACKET || static_cast<Oper *>(infix[i])->getType() == LQBRACKET) {
				stack.push(static_cast<Oper*>(infix[i]));
				continue;
			}
			if (static_cast<Oper *>(infix[i])->getType() == RBRACKET || static_cast<Oper *>(infix[i])->getType() == RQBRACKET) {
				while (stack.top()->getType() != LBRACKET && stack.top()->getType() != LQBRACKET) {
					postfix.push_back(stack.top());
					stack.pop();
				}
				if (static_cast<Oper *>(infix[i])->getType() == RQBRACKET) {
					postfix.push_back(static_cast<Oper *>(infix[i]));
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
				while (!stack.empty() && x->getType() != LBRACKET && x->getType() != LQBRACKET 
					&& static_cast<Oper *>(infix[i])->getPriority() < x->getPriority()) {
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
	std::vector<Number *> del;
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
				return static_cast<Goto *>(postfix[i])->getRow();
			}
			if (static_cast<Oper *>(postfix[i])->getType() == IF
					|| static_cast<Oper *>(postfix[i])->getType() == WHILE) {
				Goto *lexemgoto = static_cast<Goto *>(postfix[i]);
				int r = static_cast<Number *>(stack.top())->getValue();
				stack.pop();
				for (int i = 0; i < del.size(); i++) {
						cout << static_cast<Number *>(del[i])->getValue() << ": ";
						delete del[i];
						cout << "del" << endl;
					}
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
			if (static_cast<Oper *>(postfix[i])->getType() == ARRAY) {
				std::string name = static_cast<Variable *>(left)->getName();
				int index = static_cast<Number *>(right)->getValue();
				ArrayTable[name] = std::vector<int>(index);

				for (int i = 0; i < del.size(); i++) {
						cout << static_cast<Number *>(del[i])->getValue() << ": ";
						delete del[i];
						cout << "del" << endl;
					}
				return row + 1;
			}

			Lexem *ptr = static_cast<Oper *>(postfix[i])->getValue(left, right);
			stack.push(ptr);
			cout << "NEW: " << static_cast<Number *>(stack.top())->getValue() << endl;
			del.push_back(static_cast<Number *>(stack.top()));
			continue;
		}

	}

	if (stack.size()) { 
		Number *ans = static_cast<Number *>(stack.top());
		stack.pop();
		cout <<  "answer is: " << ans->getValue() << endl;
	}
	while (stack.size() != 0) {
		stack.pop();
	}
	for (int i = 0; i < del.size(); i++) {
		cout << static_cast<Number *>(del[i])->getValue() << ": ";
		delete del[i];
		cout << "del" << endl;
	}
	return row + 1;
}

void initLabels(std::vector<Lexem *> &infix, int &row) {
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



