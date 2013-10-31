#include "syntax.h"

#include <memory>
#include <stdexcept>
#include <string>

#include "data.h"
#include "tokens.h"

using namespace std;

LispData AnalyzeSyntax(Tokens* tokens)
{
	if (!tokens || tokens->empty()) {
		throw logic_error("Unexpected EOF while reading");
	}

	string token = tokens->front();
	tokens->pop_front();

	if (token == "(") {
		List lst;
		while (tokens->front() != ")") {
			lst.push_back(make_shared<LispData>(AnalyzeSyntax(tokens)));
		}
		tokens->pop_front();
		return LispData(lst);
	}
	else if (token == ")") {
		throw runtime_error("Unexpected ')'");
	}
	else {
		return LispData(token);
	}
}
