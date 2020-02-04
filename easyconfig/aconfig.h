#pragma once
#include "settings.h"
#include <stack>
#include <memory>

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

class ACONFIG;

enum ACONFIG_KIND{ ackNone, ackBool, ackInt, ackReal, ackText, ackComp };

String AconfigKindToString(ACONFIG_KIND);
ACONFIG_KIND KindFromString(String s);

struct FIELD_BASE
{
	String name;
	ACONFIG_KIND kind;
	Settings &s;
	ACONFIG *parent;

	FIELD_BASE(Settings &s, String name, ACONFIG_KIND kind = ackNone, ACONFIG *parent = nullptr) : 
				s(s),name(name), kind(kind), parent(parent) { 	}
	FIELD_BASE(Settings &s, const FIELD_BASE &fb) : FIELD_BASE(s, fb.name, fb.kind, fb.parent) {};
	FIELD_BASE &operator=(const FIELD_BASE &other)
	{
		name = other.name;
		kind = other.kind;
		parent = other.parent;
		s = other.s;

		return *this;
	}

	virtual void Store() = 0;	 // - " -
	virtual void Retrieve() = 0;
	virtual String ToString() const = 0;
	virtual	String DefToString() const = 0;
	virtual bool NotDefault() const = 0;
};

class BOOL_FIELD : public FIELD_BASE
{
	bool _value;
	bool _defVal;
public:
	BOOL_FIELD(Settings &s, String aname, bool a_defVal =false, bool val=false, ACONFIG *aparent = nullptr) :
		FIELD_BASE(s, aname, ackBool, aparent), _value(val), _defVal(a_defVal) {}
	BOOL_FIELD(Settings &s, const BOOL_FIELD &bf) : BOOL_FIELD(s, bf.name, bf._defVal, bf._value, bf.parent) {}

	virtual void SetValue(bool val)
	{
		_value = val;
		s.beginGroup(name);
			s.setValue("value", val);
		s.endGroup();
	}
	virtual bool Value() const { return _value; }
	virtual void SetDefault(bool def)
	{
		_defVal = def;
		s.beginGroup(name);
			s.setValue("default", def);
		s.endGroup();
	}
	virtual bool Default() const { return _defVal; }

	void Store();
	virtual void Retrieve();
	String ToString() const { return _value ? "TRUE" : "FALSE"; }
	String DefToString() const { return _defVal ? "TRUE" : "FALSE"; }
	virtual bool NotDefault() const { return _value != _defVal; }
	BOOL_FIELD& operator=(bool newVal);
	BOOL_FIELD &operator=(const BOOL_FIELD &other) { FIELD_BASE::operator=(other); _value = other._value; _defVal = other._defVal; ; return *this; }
};

struct INT_FIELD : public FIELD_BASE
{
	int _value = 0;
	int _defVal = 0;		// if Load() is unsuccesfull use this
public:
	INT_FIELD(Settings &s,String aname, int _defVal = 0, int val = 0, ACONFIG *aparent = nullptr) :
		FIELD_BASE(s, aname, ackInt, aparent), _value(val), _defVal(_defVal) {}
	INT_FIELD(Settings &s, const INT_FIELD &bf) : INT_FIELD(s,bf.name, bf._defVal, bf._value, bf.parent) {}

	virtual void SetValue(int val)
	{
		_value = val;
		s.beginGroup(name);
		s.setValue("value", val);
		s.endGroup();
	}
	virtual int Value() const { return _value; }
	virtual void SetDefault(int def)
	{
		_defVal = def;
		s.beginGroup(name);
		s.setValue("default", def);
		s.endGroup();
	}
	virtual bool Default() const { return _defVal; }

	String ToString(int digits) const
	{
		String s = std::to_string(_value);
		s = String(digits, '0') + s;
		return digits ? s.substr(s.length() - digits) : s;
	}
	String ToHexString(int digits = 0, bool prefix = false) const
	{
		char buf[32];
		sprintf(buf, "%*x", digits, _value);
		String s = buf;
		if (digits > 0)
			s = s.substr(s.length() - digits);

		return prefix ? "0x" + s : s;
	}

	void Store();
	virtual void Retrieve();
	String ToString() const { return std::to_string(_value); }
	String DefToString() const { return std::to_string(_defVal); }
	virtual bool NotDefault() const { return _value != _defVal; }
	INT_FIELD& operator=(int newVal);
	INT_FIELD &operator=(const INT_FIELD &other) { FIELD_BASE::operator=(other); _value = other._value; _defVal = other._defVal; return *this; }
};

struct REAL_FIELD : public FIELD_BASE
{
	double _value = 0;
	double _defVal = 0;		// if Load() is unsuccesfull use this
public:
	REAL_FIELD(Settings &s, String aname, double _defVal = 0.0, double val = 0.0, ACONFIG *aparent = nullptr) :
		FIELD_BASE(s, aname, ackReal, aparent), _value(val), _defVal(_defVal) {}
	REAL_FIELD(Settings &s, const REAL_FIELD &bf) : REAL_FIELD(s, bf.name, bf._defVal, bf._value, bf.parent) {}

	virtual void SetValue(double val)
	{
		_value = val;
		s.beginGroup(name);
		s.setValue("value", val);
		s.endGroup();
	}
	virtual double Value() const { return _value; }
	virtual void SetDefault(double def)
	{
		_defVal = def;
		s.beginGroup(name);
		s.setValue("default", def);
		s.endGroup();
	}
	virtual double Default() const { return _defVal; }

	void Store();
	virtual void Retrieve();

	String ToString() const { return std::to_string(_value); }
	String DefToString() const { return std::to_string(_defVal); }
	virtual bool NotDefault() const { return _value != _defVal; }
	REAL_FIELD& operator=(double newVal);
	REAL_FIELD &operator=(const REAL_FIELD &other) { FIELD_BASE::operator=(other); _value = other._value; _defVal = other._defVal; ; return *this; }
};

struct TEXT_FIELD : public FIELD_BASE
{
	String _value;
	String _defVal;		// if Load() is unsuccesfull use this
public:
	TEXT_FIELD(Settings &s, String aname, String a_defVal = String(), String val = String() , ACONFIG *aparent = nullptr) :
		FIELD_BASE(s, aname, ackText, aparent), _defVal(a_defVal), _value(val) {}
	TEXT_FIELD(Settings &s, const TEXT_FIELD &bf) : TEXT_FIELD(s, bf.name, bf._defVal, bf._value, bf.parent) {}

	virtual void SetValue(String val)
	{
		_value = val;
		s.beginGroup(name);
		s.setValue("value", val);
		s.endGroup();
	}
	virtual String Value() const { return _value; }
	virtual void SetDefault(String def)
	{
		_defVal = def;
		s.beginGroup(name);
		s.setValue("default", def);
		s.endGroup();
	}
	virtual String Default() const { return _defVal; }

	void Store();
	virtual void Retrieve();

	String ToString() const { return _value; }
	String DefToString() const { return _defVal; }
	virtual bool NotDefault() const { return _value != _defVal; }
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
	std::list<COMPOUND_FIELD> _compList;

	void _CopyLists(const COMPOUND_FIELD &other);
	// DEBUG
	void _DumpFields(std::ostream &ofs, ACONFIG_KIND kind = ackNone);
public:
	COMPOUND_FIELD(Settings &s, String aname, String aDefVal = String(), String val = String(), ACONFIG* aparent = nullptr) :
		TEXT_FIELD(s, aname, aDefVal, val, aparent) {}
	COMPOUND_FIELD(Settings &s, const COMPOUND_FIELD &bf) : COMPOUND_FIELD(s, bf.name, bf._defVal, bf._value, bf.parent) {}

	COMPOUND_FIELD &operator=(const COMPOUND_FIELD &other);
	FIELD_BASE *operator[](String fieldn);

	void Clear() 
	{
		_boolList.clear(); _intList.clear(); _realList.clear(); _textList.clear(); _compList.clear();
		_fields.clear();
		s.clear();
	}
	size_t Size(ACONFIG_KIND kind = ackNone) const;

	void AddBoolField(Settings &s, String name, bool _defVal = false, bool val = false);
	void AddIntField (Settings &s, String name, int _defVal = 0, int val = 0);
	void AddRealField(Settings &s, String name, double _defVal = 0.0, double val = 0.0);
	void AddTextField(Settings &s, String name, String _defVal = String(), String val = String());
	COMPOUND_FIELD * AddCompField(Settings &s, String name, String _defVal = String(), String val = String());

	void Store();
	virtual void Retrieve();
	// DEBUG
	void DumpFields(ACONFIG_KIND kind = ackNone, String file=String());	// print all
};

class ACONFIG 
{
	Settings _settings;		// changed ini is automatically written in destructor of Settings
	COMPOUND_FIELD *_pFields;
							// but can be saved any time using 'Save()'
	bool _changed = false;

	void _Store(FIELD_BASE *pf);
	ACONFIG_KIND _AddFieldFromSettings(Settings &s, String name);
	COMPOUND_FIELD *_pc = nullptr;		// used for embedded compound fields, set when using AddCompField(), reset at EndCompField()
	std::stack<COMPOUND_FIELD *, std::vector<COMPOUND_FIELD*> > _compStack;
public:
	ACONFIG() 
	{ 
		_pFields = new COMPOUND_FIELD(_settings, String()); 
		_compStack.push(_pFields);
		_pc = _pFields;
	}
	ACONFIG(String iniName) :ACONFIG()
	{ 
		_settings.SetName(iniName); 
	}
	~ACONFIG() {}

	void SetName(String iniName) { _settings.SetName(iniName); }

	ACONFIG &operator=(const ACONFIG &other) 
	{ 
		*_pFields = *other._pFields;

		_settings = other._settings;
		_changed = other._changed;
		return *this;  
	}

	FIELD_BASE *operator[](String fieldn)
	{
		return _pFields->operator[](fieldn);
	}

	void Clear()
	{
		_pFields->Clear();
		while (_compStack.size() != 1)
			_compStack.pop();
	}

	void AddBoolField(String name, bool defVal = false, bool val = false) 
	{ 
		_pc->AddBoolField(_settings, name, defVal, val); 
		_changed = true;
	}

	void AddIntField(String name, int defVal = 0, int val = 0) 
	{
		_pc->AddIntField(_settings, name, defVal, val);
		_changed = true;
	}

	void AddRealField(String name, double defVal = 0.0, double val = 0.0) 
	{
		_pc->AddRealField(_settings, name, defVal, val);
		_changed = true;
	}

	void AddTextField(String name, String defVal = String(), String val = String()) 
	{ 
		_pc->AddTextField(_settings, name, defVal, val);
		_changed = true;
	}
		   // add sub fields after this and finish with 'EndCompField()'
	COMPOUND_FIELD* AddCompField(String name, String defVal = String(), String val = String()) 
	{ 
		_changed = true;
		COMPOUND_FIELD *pc = _pc->AddCompField(_settings, name, defVal, val);
		_compStack.push(pc);
		return _pc = pc;
	}

	void EndCompField()			// call after AddCompField
	{ 
		if (!_compStack.empty()) 
		{
			_compStack.pop(); 
			_pc = _compStack.top(); 
		} 
	}



	void Store();				// in _settings
	void Load(String iniName);	// from ini file
	void Save() {	_settings.Save();	}				// into ini file it was Load()-ed from
	void Save(String iniName) { _settings.Save(iniName); }	// to ini file 
	void SetChanged(bool set) { _changed = set; }
	// DEBUG
	void DumpFields(ACONFIG_KIND kind = ackNone, String file = String()) { _pFields->DumpFields(kind, file); }

};
