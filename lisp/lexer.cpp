#include "lexer.h"

#include <algorithm>
#include <iterator>
#include <sstream>

using namespace std;

std::pair<Tokens, int> Tokenize(const string &s)
{
	Tokens res;
	int brackets = 0;
	stringstream str(s);
	bool newliteral = true;

	istreambuf_iterator<char> from(str.rdbuf());
	istreambuf_iterator<char> to;

	while (from != to) {
		switch (*from) {
		case '(':
			++brackets;
			res.push_back("(");
			newliteral = true;
			break;
		case ')':
			--brackets;
			res.push_back(")");
			newliteral = true;
			break;
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			newliteral = true;
			break;
		default:
			if (newliteral) {
				res.push_back("");
				newliteral = false;
			}
			res.back().push_back(*from);
		}
		++from;
	}

	return make_pair(res, brackets);
}
