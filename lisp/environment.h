#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "data.h"

class Env;

typedef std::vector<std::pair<Symbol, LispData> > InitializeList;
typedef std::shared_ptr<Env> EnvPtr;


class VariablesList
{
public:
	typedef std::vector<Symbol> NamesList;

	VariablesList(const List&);

	NamesList::const_iterator begin() const;
	NamesList::const_iterator end() const;


private:
	NamesList names_;
};


class Env
{
public:
	typedef std::map<Symbol, LispData> Variables;

	Env();
	Env(const VariablesList&, const LispData&, EnvPtr);

	static EnvPtr InitializeGlobalEnv();
	void Update(const InitializeList&);
	Env* FindEnv(const Symbol&);
	Variables::iterator FindSymbol(const Symbol&);
	LispData& operator[] (const Symbol&);
	Variables::iterator EndIter();
	void AddProcedure(const Symbol&, const Procedure&);

	static EnvPtr global;

private:
	Env& operator= (const Env&);

	Variables data_;
	EnvPtr outer_;
};
