#pragma once

#include <functional>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

class LispData;

typedef int Integer;
typedef double Float;
typedef std::string Symbol;
typedef std::shared_ptr<LispData> DataPtr;
typedef std::vector<DataPtr> List;
typedef std::function<LispData(const List&)> Procedure;


enum DataType {void_type, integer_type, float_type, symbol_type, list_type, procedure_type};


class LispData
{
public:
	LispData();
	explicit LispData(const std::string&);
	explicit LispData(const List&);
	explicit LispData(const Procedure&);
	explicit LispData(Integer);
	explicit LispData(Float);
	LispData(const LispData&);
	~LispData();

	void Append(const LispData&);
	const List* GetListRef() const;
	void Clear();
	Integer GetInt() const;
	Float GetFloat() const;
	Symbol GetSymbol() const;
	bool GetBool() const;
	Procedure GetProcedure() const;
	DataType GetType() const;

	LispData& operator= (const LispData&);

	friend std::ostream& operator<<(std::ostream&, const LispData&);

private:
	union {
		Integer i;
		Float f;
		Symbol *s;
		List *l;
		Procedure *p;
	} data_;


	DataType type_;
};

