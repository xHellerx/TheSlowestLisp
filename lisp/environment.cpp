#include "environment.h"

#include <algorithm>
#include <iterator>
#include <numeric>

#include "data.h"

using namespace std;

EnvPtr Env::global = InitializeGlobalEnv();


class WrongArity : public runtime_error
{
public:
	WrongArity(const Symbol& s) : runtime_error(s)
	{}

	virtual const char* what() const override
	{
		static char buff[256];
		const char* w = exception::what();
		int len = strlen(w);
		string message("Wrong number of arguments in '");
		message.insert(message.end(), w, len < 100 ? w + len : w + 100);
		message += "' procedure.";
		strcpy(buff, message.c_str());
		return buff;
	}
};


VariablesList::VariablesList(const List& lst)
{
	auto symbol_extractor = [] (DataPtr p) {
		return p->GetSymbol();
	};

	names_.resize(lst.size());
	transform(lst.begin(), lst.end(), names_.begin(), symbol_extractor);
}


VariablesList::NamesList::const_iterator VariablesList::begin() const
{
	return names_.begin();
}


VariablesList::NamesList::const_iterator VariablesList::end() const
{
	return names_.end();
}


Env::Env() {}

Env::Env(const VariablesList& vars, const LispData& data, EnvPtr outer)
	: outer_(outer)
{
	auto v_it = vars.begin();
	auto d_it = begin(*data.GetListRef());
	auto last = vars.end();
	for (;v_it != last; ++v_it, ++d_it) {
		data_.insert(make_pair(*v_it, **d_it));
	}
}


EnvPtr Env::InitializeGlobalEnv()
{
	EnvPtr global = make_shared<Env>();

	global->AddProcedure("+", [] (const List& lst) -> LispData {
		auto summator = [] (const LispData& a, const DataPtr b) -> LispData {
			if (a.GetType() == integer_type && b->GetType() == integer_type) {
				return LispData(a.GetInt() + b->GetInt());
			}
			else {
				return LispData(a.GetFloat() + b->GetFloat());
			}
		};
		return accumulate(begin(lst), end(lst), LispData(0L), summator);
	});

	global->AddProcedure("-", [] (const List& lst) -> LispData {
		if (lst.size() != 2) {
			throw WrongArity("-");
		}
		LispData a = *lst.front();
		LispData b = *lst.back();
		if (a.GetType() == integer_type && b.GetType() == integer_type) {
			return LispData(a.GetInt() - b.GetInt());
		}
		else {
			return LispData(a.GetFloat() - b.GetFloat());
		}
	});

	global->AddProcedure("*", [] (const List& lst) -> LispData {
		auto multiplicator = [] (const LispData& a, const DataPtr b) -> LispData {
			if (a.GetType() == integer_type && b->GetType() == integer_type) {
				return LispData(a.GetInt() * b->GetInt());
			}
			else {
				return LispData(a.GetFloat() * b->GetFloat());
			}
		};
		return accumulate(begin(lst), end(lst), LispData(1), multiplicator);
	});


	global->AddProcedure("/", [] (const List& lst) -> LispData {
		if (lst.size() != 2) {
			throw WrongArity("/");
		}
		LispData a = *lst.front();
		LispData b = *lst.back();
		if (fabs(b.GetFloat()) < 1e-15) {
			throw runtime_error("Zero division");
		}
		if (a.GetType() == integer_type && b.GetType() == integer_type) {
			return LispData(a.GetInt() / b.GetInt());
		}
		else {
			return LispData(a.GetFloat() / b.GetFloat());
		}
	});


	global->AddProcedure("=", [] (const List& lst) -> LispData {
		if (lst.size() != 2) {
			throw WrongArity("=");
		}
		LispData a = *lst.front();
		LispData b = *lst.back();
		return LispData(a.GetFloat() == b.GetFloat());
	});


	global->AddProcedure("cons", [] (const List& lst) -> LispData {
		if (lst.size() != 2) {
			throw WrongArity("cons");
		}
		return LispData(lst);
	});


	global->AddProcedure("car", [] (const List& lst) -> LispData {
		if (lst.size() != 1) {
			throw WrongArity("car");
		}
		if (lst.front()->GetType() != list_type) {
			throw runtime_error("Not pair type given to 'car'.");
		}
		return LispData(*lst.front()->GetListRef()->front());
	});


	global->AddProcedure("cdr", [] (const List& lst) -> LispData {
		if (lst.size() != 1) {
			throw WrongArity("cdr");
		}
		auto pair = lst.front();
		if (pair->GetType() != list_type || pair->GetListRef()->size() != 2) {
			throw runtime_error("Not pair type given to 'cdr'.");
		}
		return LispData(*pair->GetListRef()->back());
	});


	global->AddProcedure("null?", [] (const List& lst) -> LispData {
		if (lst.size() != 1) {
			throw WrongArity("null?");
		}
		auto pair = lst.front();
		if (pair->GetType() != list_type) {
			throw runtime_error("Not pair type given to 'null?'.");
		}
		return LispData(pair->GetListRef()->empty());
	});


	(*global)["nil"] = LispData(List());


	global->AddProcedure("exit", [] (const List&) -> LispData {
		exit(0);
	});

	return global;
}


void Env::Update(const InitializeList& args)
{
	auto it = begin(args);
	auto last = end(args);
	auto data_end = end(data_);
	for (;it != last; ++it) {
		auto pos = data_.find(it->first);
		if (pos == data_end) {
			data_.insert(*it);
		}
		else {
			pos->second = it->second;
		}
	}
}


Env* Env::FindEnv(const Symbol& x)
{
	if (data_.find(x) == data_.end()) {
		if (outer_) {
			return outer_->FindEnv(x);
		}
		else {
			return nullptr;
		}
	}
	else {
		return this;
	}
}


Env::Variables::iterator Env::FindSymbol(const Symbol& x)
{
	return data_.find(x);
}


LispData& Env::operator[] (const Symbol& x)
{
	return data_[x];
}


Env::Variables::iterator Env::EndIter()
{
	return data_.end();
}


void Env::AddProcedure(const Symbol &name, const Procedure &p)
{
	data_[name] = LispData(p);
}

