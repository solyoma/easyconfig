#pragma once
#include "settings.h"

//******************************************
// splittable string http://www.cplusplus.com/articles/1UqpX9L8/
class splitstring : public String 
{
	std::vector<String> _flds;
public:
	splitstring(char *s) : String(s) { };
	splitstring():String()  { };
	// split: receives a char delimiter; returns a vector of strings
	// By default ignores repeated delimiters, unless argument rep == 1.
	std::vector<String>& split(char delim, int rep)
	{
		if (!_flds.empty()) _flds.clear();  // empty vector if necessary
		String work = data();
		String buf = "";
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
	const String &operator[] (size_t index) const
	{
		static String dummy;

		if (index < 0 || index > _flds.size())
			return dummy;
		return _flds[index];
	}
	size_t Size() const { return _flds.size(); }
};

//**************************************

class COMPOUND_FIELD;

enum ACONFIG_KIND{ ackNone, ackBool, ackInt, ackReal, ackText, ackComp };

String AconfigKindToString(ACONFIG_KIND);

struct FIELD_BASE
{
	String name;
	ACONFIG_KIND kind;
	COMPOUND_FIELD *parent;

	FIELD_BASE(String name, ACONFIG_KIND kind = ackNone, COMPOUND_FIELD *parent = nullptr) : name(name), kind(kind), parent(parent) { 	}
	FIELD_BASE(const FIELD_BASE &fb) : FIELD_BASE(fb.name, fb.kind, fb.parent) {};
	FIELD_BASE &operator=(const FIELD_BASE &other)
	{
		name = other.name;
		kind = other.kind;
		parent = other.parent;
		changed = changed;

		return *this;
	}

	bool changed = false;
	virtual void Store(Settings &s) = 0;	 // - " -
	virtual void Retrieve(Settings &ofs) = 0;
	virtual String ToString() const = 0;
	virtual	String DefToString() const = 0;
	virtual bool NotDefault() const = 0;
	void SetChanged(bool changed) { changed = changed; }

};

struct BOOL_FIELD : public FIELD_BASE
{
	bool value;
	bool defVal;
	BOOL_FIELD(String aname, bool aDefVal =false, bool val=false, COMPOUND_FIELD *aparent = nullptr) :
		FIELD_BASE(aname, ackBool, aparent), value(val), defVal(aDefVal) {}
	BOOL_FIELD(const BOOL_FIELD &bf) : BOOL_FIELD(bf.name, bf.defVal, bf.value, bf.parent) {}
	void Store(Settings &s);
	virtual void Retrieve(Settings &ofs);
	String ToString() const { return value ? "TRUE" : "FALSE"; }
	String DefToString() const { return defVal ? "TRUE" : "FALSE"; }
	virtual bool NotDefault() const { return value != defVal; }
	BOOL_FIELD& operator=(bool newVal);
	BOOL_FIELD &operator=(const BOOL_FIELD &other) { FIELD_BASE::operator=(other); value = other.value; defVal = other.defVal; ; return *this; }
};

struct INT_FIELD : public FIELD_BASE
{
	int value = 0;
	int defVal = 0;		// if Load() is unsuccesfull use this
	INT_FIELD(String aname, int defVal = 0, int val = 0, COMPOUND_FIELD *aparent = nullptr) : 
		FIELD_BASE(aname, ackInt, aparent), value(val), defVal(defVal) {}
	INT_FIELD(const INT_FIELD &bf) : INT_FIELD(bf.name, bf.defVal, bf.value, bf.parent) {}

	String ToString(int digits) const
	{
		String s = std::to_string(value);
		s = String(digits, '0') + s;
		return digits ? s.substr(s.length() - digits) : s;
	}
	String ToHexString(int digits = 0, bool prefix = false) const
	{
		char buf[32];
		sprintf(buf, "%*x", digits, value);
		String s = buf;
		if (digits > 0)
			s = s.substr(s.length() - digits);

		return prefix ? "0x" + s : s;
	}

	void Store(Settings &s);
	virtual void Retrieve(Settings &ofs);
	String ToString() const { return std::to_string(value); }
	String DefToString() const { return std::to_string(defVal); }
	virtual bool NotDefault() const { return value != defVal; }
	INT_FIELD& operator=(int newVal);
	INT_FIELD &operator=(const INT_FIELD &other) { FIELD_BASE::operator=(other); value = other.value; defVal = other.defVal; return *this; }
};

struct REAL_FIELD : public FIELD_BASE
{
	double value = 0;
	double defVal = 0;		// if Load() is unsuccesfull use this
	REAL_FIELD(String aname, double defVal = 0.0, double val = 0.0, COMPOUND_FIELD *aparent = nullptr) :
		FIELD_BASE(aname, ackReal, aparent), value(val), defVal(defVal) {}
	REAL_FIELD(const REAL_FIELD &bf) : REAL_FIELD(bf.name, bf.defVal, bf.value, bf.parent) {}

	void Store(Settings &s);
	virtual void Retrieve(Settings &ofs);

	String ToString() const { return std::to_string(value); }
	String DefToString() const { return std::to_string(defVal); }
	virtual bool NotDefault() const { return value != defVal; }
	REAL_FIELD& operator=(double newVal);
	REAL_FIELD &operator=(const REAL_FIELD &other) { FIELD_BASE::operator=(other); value = other.value; defVal = other.defVal; ; return *this; }
};

struct TEXT_FIELD : public FIELD_BASE
{
	String value;
	String defVal;		// if Load() is unsuccesfull use this
	TEXT_FIELD(String aname, String aDefVal = String(), String val = String() , COMPOUND_FIELD *aparent = nullptr) :
		FIELD_BASE(aname, ackText, aparent), defVal(aDefVal), value(val) {}
	TEXT_FIELD(const TEXT_FIELD &bf) : TEXT_FIELD(bf.name, bf.defVal, bf.value, bf.parent) {}

	void Store(Settings &s);
	virtual void Retrieve(Settings &ofs);

	String ToString() const { return value; }
	String DefToString() const { return defVal; }
	virtual bool NotDefault() const { return value != defVal; }
	TEXT_FIELD& operator=(String newVal);
};

class COMPOUND_FIELD : public TEXT_FIELD
{
protected:
	std::map<String, FIELD_BASE*> _fields;
	std::list<BOOL_FIELD> _boolList;
	std::list<INT_FIELD> _intList;
	std::list<REAL_FIELD> _realList;
	std::list<TEXT_FIELD> _textList;
	std::list<TEXT_FIELD> _compList;

	void _CopyLists(const COMPOUND_FIELD &other);
public:
	COMPOUND_FIELD(String aname, String aDefVal = String(), String val = String(), COMPOUND_FIELD* aparent = nullptr) :
		TEXT_FIELD(aname, aDefVal, val, aparent) {}
	COMPOUND_FIELD(const COMPOUND_FIELD &bf) : COMPOUND_FIELD(bf.name, bf.defVal, bf.value, bf.parent) {}

	COMPOUND_FIELD &operator=(const COMPOUND_FIELD &other);
	FIELD_BASE *operator[](String fieldn);

	void Clear() { _boolList.clear(); _intList.clear(); _realList.clear(); _textList.clear(); _fields.clear(); changed = false; }
	size_t Size(ACONFIG_KIND kind = ackNone) const;

	void AddBoolField(String name, bool defVal = false, bool val = false);
	void AddIntField(String name, int defVal = 0, int val = 0);
	void AddRealField(String name, double defVal = 0.0, double val = 0.0);
	void AddTextField(String name, String defVal = String(), String val = String());

	void Store(Settings &s);
	virtual void Retrieve(Settings &ofs);
};

class ACONFIG : public COMPOUND_FIELD
{
	void _Store(Settings s, FIELD_BASE *pf);
public:
	ACONFIG() : COMPOUND_FIELD("\\") {}

	void Load(String fname);	// from ini file
	void Store(String fname);
		// DEBUG
	void DumpFields(ACONFIG_KIND kind = ackNone, String file=String());	// print all
	ACONFIG &operator=(const ACONFIG &other) { COMPOUND_FIELD::operator=(other);  return *this;  }
};
