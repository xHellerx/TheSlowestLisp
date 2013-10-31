#include "data.h"

#include <algorithm>
#include <stdexcept>

using namespace std;


LispData::LispData()
{
	type_ = void_type;
}


LispData::LispData(const string &s)
{
	auto i = s.find_first_not_of("0123456789");
	if (i == string::npos) {
		type_ = integer_type;
		data_.i = stoi(s);
		return;
	}
	else if (s[i] == '.') {
		auto j = s.find_first_not_of("0123456789", i + 1);
		if (j == string::npos) {
			type_ = float_type;
			data_.f = stod(s);
			return;
		}
	}
	type_ = symbol_type;
	data_.s = new string(s);
}


LispData::LispData(const List& lst)
{
	type_ = list_type;
	data_.l = new List(lst);
}


LispData::LispData(const Procedure& p)
{
	type_ = procedure_type;
	data_.p = new Procedure(p);
}


LispData::LispData(Integer x)
{
	type_ = integer_type;
	data_.i = x;
}


LispData::LispData(Float x)
{
	type_ = float_type;
	data_.f = x;
}


LispData::LispData(const LispData& x)
{
	type_ = x.type_;
	switch (type_) {
	case integer_type:
		data_.i = x.data_.i;
		break;
	case float_type:
		data_.f = x.data_.f;
		break;
	case symbol_type:
		data_.s = new Symbol(*x.data_.s);
		break;
	case list_type:
		data_.l = new List(*x.data_.l);
		break;
	case procedure_type:
		data_.p = new Procedure(*x.data_.p);
		break;
	}
}



LispData::~LispData()
{
	Clear();
}


void LispData::Append(const LispData& data)
{
	if (type_ != list_type) {
		throw logic_error("Trying to append to non-list datatype.");
	}
	
	data_.l->push_back(make_shared<LispData>(data));
}


const List* LispData::GetListRef() const
{
	if (type_ != list_type) {
		throw logic_error("GetListRef called on non-list datatype");
	}

	return data_.l;
}


void LispData::Clear()
{
	switch (type_) {
	case symbol_type:
		delete data_.s;
		data_.s = nullptr;
		break;
	case list_type:
		delete data_.l;
		data_.l = nullptr;
		break;
	case procedure_type:
		delete data_.p;
		data_.p = nullptr;
		break;
	}
}


Integer LispData::GetInt() const
{
	if (type_ != integer_type) {
		throw runtime_error("Trying to apply integer operation to non-integer type");
	}
	return data_.i;
}


Float LispData::GetFloat() const
{
	switch (type_) {
	case integer_type:
		return data_.i;
	case float_type:
		return data_.f;
	default:
		throw runtime_error("Trying to apply float operation to non-float type");
	}
}


Symbol LispData::GetSymbol() const
{
	if (type_ != symbol_type) {
		throw runtime_error("Trying to evaluate non-symbolic type as symbol");
	}
	return *data_.s;
}


bool LispData::GetBool() const
{
	switch (type_) {
	case void_type:
		return false;
	case integer_type:
		return data_.i != 0;
	case float_type:
		return fabs(data_.f) > 1e-15;
	case procedure_type:
	case symbol_type:
		return true;
	case list_type:
		return data_.l->empty();
	default:
		throw logic_error("GetBool() called on unknown type");
	}
}


Procedure LispData::GetProcedure() const
{
	if (type_ != procedure_type) {
		throw runtime_error("Trying to evaluate not a procedure type");
	}

	return *data_.p;
}


DataType LispData::GetType() const
{
	return type_;
}


LispData& LispData::operator= (const LispData& x)
{
	Clear();

	type_ = x.type_;
	switch (type_) {
	case integer_type:
		data_.i = x.data_.i;
		break;
	case float_type:
		data_.f = x.data_.f;
		break;
	case symbol_type:
		data_.s = new Symbol(*x.data_.s);
		break;
	case list_type:
		data_.l = new List(*x.data_.l);
		break;
	case procedure_type:
		data_.p = new Procedure(*x.data_.p);
		break;
	}

	return *this;
}


ostream& operator<< (ostream& s, const LispData &x)
{
	switch (x.type_) {
	case integer_type:
		s<<x.data_.i;
		break;
	case float_type:
		s<<x.data_.f;
		break;
	case symbol_type:
		s<<*x.data_.s;
		break;
	case list_type:
		{
			s<<"(";
			if (!x.data_.l->empty()) {
				s<<*x.data_.l->front();
			}
			auto printer = [&s] (DataPtr p) mutable {
				s<<", ";
				s<<*p;
			};
			for_each(x.data_.l->begin() + 1, x.data_.l->end(), printer);
			s<<")";
		}
		break;
	case procedure_type:
		s<<"<procedure>";
		break;
	}
	return s;
}
