#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <fstream>

//******************************************
// splittable string http://www.cplusplus.com/articles/1UqpX9L8/

class splitstring : public std::string 
{
	std::vector<std::string> _flds;
public:
	splitstring(char *s) : std::string(s) { };
	splitstring():std::string()  { };
	// split: receives a char delimiter; returns a vector of strings
	// By default ignores repeated delimiters, unless argument rep == 1.
	std::vector<std::string>& split(char delim, int rep)
	{
		if (!_flds.empty()) _flds.clear();  // empty vector if necessary
		std::string work = data();
		std::string buf = "";
		size_t i = 0;
		while (i < work.length()) {
			if (work[i] != delim)
				buf += work[i];
			else if (rep == 1) {
				_flds.push_back(buf);
				buf = "";
			}
			else if (buf.length() > 0) {
				_flds.push_back(buf);
				buf = "";
			}
			i++;
		}
		if (!buf.empty())
			_flds.push_back(buf);
		return _flds;
	}
	const std::string &operator[] (size_t index) const
	{
		static std::string dummy;

		if (index < 0 || index > _flds.size())
			return dummy;
		return _flds[index];
	}
	size_t Size() const { return _flds.size(); }
};

//**************************************

class ACONFIG;

enum ACONFIG_KIND{ ackNone, ackBool, ackInt, ackReal, ackText };

std::string AconfigKindToString(ACONFIG_KIND);

struct FIELD_BASE
{
	std::string name;
	ACONFIG_KIND kind;
	ACONFIG *parent;
	bool changed = false;
	virtual void Store(std::ostream &_ofs) = 0;	 // - " -

	FIELD_BASE(std::string name, ACONFIG_KIND kind = ackNone, ACONFIG*parent = nullptr) : name(name), kind(kind), parent(parent) { 	}
};

struct BOOL_FIELD : public FIELD_BASE
{
	bool value;
	bool defVal;
	BOOL_FIELD(std::string aname, bool aDefVal =false, bool val=false, ACONFIG* aparent = nullptr) :
		FIELD_BASE(aname, ackBool, aparent), value(val), defVal(aDefVal) {}
	void Store(std::ostream &_ofs);
	std::string ToString() const { return value ? "TRUE" : "FALSE"; }
	std::string DefToString() const { return defVal ? "TRUE" : "FALSE"; }
	BOOL_FIELD& operator=(bool newVal);
};

struct INT_FIELD : public FIELD_BASE
{
	int value = 0;
	int defVal = 0;		// if Load() is unsuccesfull use this
	INT_FIELD(std::string aname, int defVal = 0, int val = 0, ACONFIG* aparent = nullptr) : 
		FIELD_BASE(aname, ackInt, aparent), value(val), defVal(defVal) {}
	void Store(std::ostream &_ofs);
	std::string ToString() const { return std::to_string(value); }
	std::string DefToString() const { return std::to_string(defVal); }
	INT_FIELD& operator=(int newVal);
};

struct REAL_FIELD : public FIELD_BASE
{
	double value = 0;
	double defVal = 0;		// if Load() is unsuccesfull use this
	REAL_FIELD(std::string aname, double defVal = 0.0, double val = 0.0, ACONFIG* aparent = nullptr) :
		FIELD_BASE(aname, ackReal, aparent), value(val), defVal(defVal) {}
	void Store(std::ostream &_ofs);
	std::string ToString() const { return std::to_string(value); }
	std::string DefToString() const { return std::to_string(defVal); }
	REAL_FIELD& operator=(double newVal);
};

struct TEXT_FIELD : public FIELD_BASE
{
	std::string value;
	std::string defVal;		// if Load() is unsuccesfull use this
	TEXT_FIELD(std::string aname, std::string aDefVal = std::string(), std::string val = std::string() , ACONFIG* aparent = nullptr) :
		FIELD_BASE(aname, ackText, aparent), defVal(aDefVal), value(val) {}
	void Store(std::ostream &_ofs);
	std::string ToString() const { return value; }
	std::string DefToString() const { return defVal; }
	TEXT_FIELD& operator=(std::string newVal);
};

class ACONFIG
{
	std::ifstream _ifs;
	std::ofstream _ofs;
	bool _changed;

	std::map<std::string, FIELD_BASE*> _fields;
	std::list<BOOL_FIELD> _boolList;
	std::list<INT_FIELD> _intList;
	std::list<REAL_FIELD> _realList;
	std::list<TEXT_FIELD> _textList;

	void _Write(FIELD_BASE *pf);
public:
	ACONFIG() {}

	void SetChanged(bool changed) { _changed = changed; }
	void Clear() { _boolList.clear(); _intList.clear(); _realList.clear(); _textList.clear(); _fields.clear(); _changed = false; }
	size_t Size(ACONFIG_KIND kind = ackNone) const;

	void AddBoolField(std::string name, bool defVal = false, bool val = false);
	void AddIntField(std::string name, int defVal = 0, int val = 0);
	void AddRealField(std::string name, double defVal = 0.0, double val = 0.0);
	void AddTextField(std::string name, std::string defVal = std::string(), std::string val = std::string());

	void Load(std::string fname);	// from file
	void Store(std::string fname);
		// DEBUG
	void DumpFields(ACONFIG_KIND kind = ackNone, std::string file=std::string());	// print all
		// operators
	FIELD_BASE *operator[](std::string fieldn);
	ACONFIG &CopyFrom(const ACONFIG &other);
};
