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
	std::vector<std::string> flds;
public:
	splitstring(char *s) : std::string(s) { };
	splitstring():std::string()  { };
	// split: receives a char delimiter; returns a vector of strings
	// By default ignores repeated delimiters, unless argument rep == 1.
	std::vector<std::string>& split(char delim, int rep)
	{
		if (!flds.empty()) flds.clear();  // empty vector if necessary
		std::string work = data();
		std::string buf = "";
		size_t i = 0;
		while (i < work.length()) {
			if (work[i] != delim)
				buf += work[i];
			else if (rep == 1) {
				flds.push_back(buf);
				buf = "";
			}
			else if (buf.length() > 0) {
				flds.push_back(buf);
				buf = "";
			}
			i++;
		}
		if (!buf.empty())
			flds.push_back(buf);
		return flds;
	}
};

//**************************************

class ACONFIG;

enum ACONFIG_KIND{ ackNone, ackBool, ackInt, ackReal, ackText };

std::string AconfigKindToString(ACONFIG_KIND);

struct FIELD_BASE
{
	std::string name;
	ACONFIG_KIND kind = ackNone;
	ACONFIG *parent;
	bool changed;
	virtual void Store() = 0;	 // - " -
};

struct BOOL_FIELD : public FIELD_BASE
{
	bool value;
	bool defVal;
	BOOL_FIELD(std::string aname, bool aDefVal =false, bool val=false, ACONFIG* aparent = nullptr) :
		value(val), defVal(aDefVal) {	name = aname; parent = aparent; kind = ackBool;	}
	void Store();
	std::string ToString() const { return value ? "TRUE" : "FALSE"; }
	std::string DefToString() const { return defVal ? "TRUE" : "FALSE"; }
	BOOL_FIELD& operator=(bool newVal);
};

struct INT_FIELD : public FIELD_BASE
{
	int value = 0;
	int defVal = 0;		// if Load() is unsuccesfull use this
	INT_FIELD(std::string aname, int defVal = 0, int val = 0, ACONFIG* aparent = nullptr) : 
		value(val), defVal(defVal) { name = aname; parent = aparent; kind = ackInt;	}
	void Store();
	std::string ToString() const { return std::to_string(value); }
	std::string DefToString() const { return std::to_string(defVal); }
	INT_FIELD& operator=(int newVal);
};

struct REAL_FIELD : public FIELD_BASE
{
	double value = 0;
	double defVal = 0;		// if Load() is unsuccesfull use this
	REAL_FIELD(std::string aname, double defVal = 0.0, double val = 0.0, ACONFIG* aparent = nullptr) :
		value(val), defVal(defVal) {
		name = aname; parent = aparent; kind = ackReal;
	}
	void Store();
	std::string ToString() const { return std::to_string(value); }
	std::string DefToString() const { return std::to_string(defVal); }
	REAL_FIELD& operator=(double newVal);
};

struct TEXT_FIELD : public FIELD_BASE
{
	std::string value;
	std::string defVal;		// if Load() is unsuccesfull use this
	TEXT_FIELD(std::string aname, std::string aDefVal = std::string(), std::string val = std::string() , ACONFIG* aparent = nullptr) :
		value(val), defVal(aDefVal) { name = aname; parent = aparent; kind = ackText;	}
	void Store();
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
public:
	ACONFIG() {}

	void SetChanged(bool changed) { _changed = changed; }

	void AddBoolField(std::string name, bool defVal=false, bool val = false)
	{
		BOOL_FIELD intf(name, defVal, val, this);
		_boolList.push_back(intf);
		if (_fields.count(name) && _fields[name]->kind != ackBool)
			throw "type mismatch";
		_fields[name] = &_boolList.back();
	}
	void AddIntField(std::string name, int defVal=0, int val=0)
	{
		INT_FIELD intf(name, defVal, val, this);
		_intList.push_back(intf);
		if (_fields.count(name) && _fields[name]->kind != ackInt)
			throw "type mismatch";
		_fields[name] = &_intList.back();
	}
	void AddRealField(std::string name, double defVal = 0.0, double val = 0.0)
	{
		REAL_FIELD intf(name, defVal, val, this);
		_realList.push_back(intf);
		if (_fields.count(name) && _fields[name]->kind != ackReal)
			throw "type mismatch";
		_fields[name] = &_realList.back();
	}
	void AddTextField(std::string name, std::string defVal=std::string(), std::string val = std::string())
	{
		TEXT_FIELD textf(name, defVal, val, this);
		size_t n = _textList.size();
		_textList.push_back(textf);
		if (_fields.count(name) && _fields[name]->kind != ackText)
			throw "type mismatch";
		_fields[name] = &_textList.back();
	}

	void WriteBool(BOOL_FIELD *pintf);
	void WriteInt(INT_FIELD *pintf);
	void WriteReal(REAL_FIELD *pintf);
	void WriteText(TEXT_FIELD *ptextf);

	void Load(std::string fname)	// from file
	{
		_ifs.open(fname);
		if (!_ifs.is_open())
			throw "can't open";
		std::vector < std::string> sl;
		splitstring s;
		while (! _ifs.eof())
		{
			_ifs >> s;
			sl = s.split(',',1);
			if (sl.size() != 4) // <name>,<type>,<value>,<default>
				throw "bad string";
			switch (sl[1][0])
			{
				case 'b': AddBoolField(sl[0], stoi(sl[2]), stoi(sl[3])); break;
				case 'i': AddIntField(sl[0], stoi(sl[2]), stoi(sl[3])); break;
				case 'r': AddRealField(sl[0], stod(sl[2]), stod(sl[3])); break;
				case 't': AddTextField(sl[0], sl[2], sl[3]);
				default: break;
			}
		}
		_ifs.close();
	}

	void Store(std::string fname)
	{
		_ofs.open(fname.c_str());
		if (!_ofs.is_open())
			throw "can't open";

		for (auto p : _fields)
			p.second->Store();
		_ofs.close();
	}
		// DEBUG
	void DumpFields(ACONFIG_KIND kind = ackNone, std::string file=std::string());	// print all
		// operators
	FIELD_BASE *operator[](std::string fieldn)
	{

		if (_fields.count(fieldn))
			return _fields[fieldn];
		return nullptr;
	}
	ACONFIG &CopyFrom(const ACONFIG &other)
	{
		if (_ifs.is_open())
			_ifs.close();
		if (_ofs.is_open())
			_ofs.close();
		_changed = other._changed;
		_intList = other._intList;
		_boolList = other._boolList;
		_realList = other._realList;
		_textList = other._textList;

		_fields.clear();
		for (auto a : _intList)
			_fields[a.name] = &a;
		for (auto a : _textList)
			_fields[a.name] = &a;

		return *this;
	}

};
