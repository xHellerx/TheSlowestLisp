// lisp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <functional>
#include <iostream>
#include <string>
#include <tuple>

#include "data.h"
#include "lexer.h"
#include "syntax.h"
#include "evaluator.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	while (true) {
		try {
			int brackets = 0;
			Tokens tokens;
			do {
				string input;
				getline(cin, input);
				Tokens t;
				int br;
				tie(t, br) = Tokenize(input);
				brackets += br;
				if (brackets < 0) {
					throw runtime_error("Unexpected ')'");
				}
				tokens.insert(tokens.end(), t.begin(), t.end());
			} while (brackets);

			if (!tokens.empty()) {
				cout<<Eval(AnalyzeSyntax(&tokens))<<endl;
			}
		}
		catch (exception &ex) {
			cout<<ex.what()<<endl;
		}
	}
	return 0;
}

