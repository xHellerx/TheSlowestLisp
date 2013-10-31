#include "evaluator.h"

#include "data.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>

using namespace std;

LispData Eval(const LispData& lst, EnvPtr env)
{
	if (lst.GetType() == symbol_type) {
		Env* e = env->FindEnv(lst.GetSymbol());
		auto s = e->FindSymbol(lst.GetSymbol());
		if (s == e->EndIter()) {
			throw runtime_error("Accessing not initialized symbol");
		}
		return s->second;
	}
	else if (lst.GetType() != list_type) {
		return lst;
	}
	
	auto args = lst.GetListRef();
	Symbol command = args->front()->GetSymbol();

	if (command == "if") {
		bool test = Eval(*args->at(1), env).GetBool();
		return Eval(*args->at(test ? 2 : 3), env);
	}
	else if (command == "define") {
		Symbol symbol = args->at(1)->GetSymbol();
		(*env)[symbol] = Eval(*args->at(2), env);
		return LispData();
	}
	else if (command == "lambda") {
		VariablesList vars(*args->at(1)->GetListRef());
		LispData exp = *args->at(2);
		return LispData([exp, env, vars] (const List &d) {
			return Eval(exp, make_shared<Env>(vars, LispData(d), env));
		});
	}
	else if (command == "set!") {
		Symbol symbol = args->at(1)->GetSymbol();
		Env* e = env->FindEnv(symbol);
		if (!e) {
			string message("Cannot find variable ");
			message += symbol;
			throw runtime_error(message);
		}
		e->FindSymbol(symbol)->second = Eval(*args->at(2), env);
		return LispData();
	}
	else if (command == "begin") {
		LispData res;
		auto from = args->begin() + 1;
		auto to = args->end();
		while (from < to) {
			res = Eval(**from, env);
			++from;
		}
		return res;
	}
	else {
		List res;
		res.reserve(args->size());
		auto evaluator = [env] (DataPtr p) {
			return make_shared<LispData>(Eval(*p, env));
		};
		transform(args->begin(), args->end(), back_inserter(res), evaluator);
		Procedure proc = res.front()->GetProcedure();
		List newargs(res.begin() + 1, res.end());
		return proc(newargs);
	}
}
