#include "interpreter.h"
//#include <vector>

int main() {
	std::string codeline;
	std::vector<std::vector<Lexem *>> infixLines, postfixLines;

	while (std::getline(std::cin, codeline)) {
		infixLines.push_back(parseLexem(codeline));
	}	
	

	initJumps(infixLines);

	for (int row = 0; row < infixLines.size(); row++) {
		initLabels(infixLines[row], row);
	}

	for (const auto &infix: infixLines) {
	 	postfixLines.push_back(buildPoliz(infix));
	}

	// for (int i = 0; i < postfixLines.size(); i++) {		
 	// 		print(postfixLines[i]);
	// }

	int row = 0;
	while (0 <= row && row < postfixLines.size()) {
		row = evaluatePoliz(postfixLines[row], row);
	}
	for (int i = 0; i < infixLines.size(); i++) {
		for (int j = 0; j < infixLines[i].size(); j++)
			delete infixLines[i][j];
	}

	return 0;
}