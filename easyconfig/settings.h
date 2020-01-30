#pragma once
#include <string>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <fstream>

#include "trim.h"
//******************************************
// Settings class: stores/retrieves data in/from files

#if 0
class String : public std::string
{
public:
	String	left(size_t n) const { return 	static_cast< String>(substr(0, n)); }
	String	mid(size_t from, size_t n=0xffff) const  { return 	static_cast<String>(substr(from, n)); }
	String	right(size_t n) const { return static_cast<String>(substr(s.length() - n)); }

	String	isEmpty()	const { return empty(); }
	String  lTrim() const { return ltrim_copy(*this); }
	String  rTrim() const { return rtrim_copy(*this); }
	String  trim() const { return ltrim_copy(rTrim() ); }

	int indexOf(const String &str, int from = 0) const
	{
		return (int)
	}
};
#else
	#ifdef QSTRING_H
		using String = QString;
		const NPOS = -1;
	#else
		using String = std::string;

		#define left(n)	substr(0,n)
		#define  right(n,s) substr(s.length() - n)
		#define  mid	substr
		#define indexOf	find_first_of
		#define lastIndexOf	find_last_of
		#define isEmpty()	empty()
		#define NPOS	std::string::npos
	#endif
#endif
using StringList = std::list<String>;
using StringMap = std::map<String, String>;

// pairs: ( <full name>, <value> )	<= <full name> = <value>
// Example 
//      pair								in ini file: 				using Settings(s)

//	(Fruit, apple)							Fruit=apple					s.setValue("fruit","apple");

//  (fruit\apple,red)						[fruit]						s.beginGroup("fruit"); s.setValue("apple","red");; s.endGroup();
//											apple=red

// (fruit\apple\color, blue)				[fruit]						s.beginGroup("fruit"); 
//											apple\color=blue			  s.beginGroup(apple);
//																			s.setValue("color","blue"); 
//																		  s.endGroup(); 
//																		s.endGroup();
//                                                             OR:
//																		s.setValue("fruit\\apple\\color","blue");


using ValuePair = std::pair<String, String>;
class ValueItem 
{
	String _val;
public:
	ValueItem() {}
	ValueItem(String &val) :_val(val) {}
	String toString() const { return  _val; }
	int toInt() const { return std::stoi (_val); }
	int toBool() const { return std::stoi(_val); }
	double toDouble() { return std::stod (_val); }
};

class ValueVector 
{
protected:
	std::vector<ValuePair> _values; 
	bool _changed = false;
	int _nVpIndex;	// in _values;
	String _group;	// actual group

	bool _SetGroupRootFrom(char *buf)
	{
		if (buf[0] == '[')	// root section header
		{
			char *p = strchr(buf, ']');
			if (!p)
				throw "bad section head";
			_group = String(buf + 1).left((p - buf) - 1) + "\\";
			return true;
		}
		return false;
	}
	
	int _Find(String name)	// 0: not found, other *** index+1 *** of element
	{						// name: full path name, e.g. s\b\c
		_nVpIndex = 0;
		for (auto p : _values)
			if (p.first == name)
				return _nVpIndex;
			else
				++_nVpIndex;
		return _nVpIndex;
	}

	size_t _AddPair(char *buf) // format : [<white space>]<text>[<white space>]=[<white space>]<text>[<whitespace>][#<comment>]
	{
		char *p = strchr(buf, '#');
		if (p)
			*p = 0;
		if ( (p = strchr(buf, '=')) == 0)
			throw "bad line";

		*p++ = 0;
		String	sKey = String(Trim(buf)),
				sValue = String(Trim(p));

		if (!_group.isEmpty())
			sKey = _group + sKey;

		int i = _Find(sKey);
		if (i)
		{
			_values.push_back(std::make_pair(sKey, sValue) );
			_changed = true;
		}
		else
		{
			_values[i - 1].second = sValue;
			_changed = true;
		}

		return _values.size();
	}

	size_t _AddPair(String s) // format : [<white space>]<text>[<white space>]=[<white space>]<text>[<whitespace>][#<comment>]
	{
		char buf[1024];
		size_t len = s.length();
		if (len > 1023)
			len = 1023;
		strncpy(buf, s.c_str(), len);
		buf[len] = 0;
		return _AddPair(buf);
	}

public:
	bool Contains(String name)		// name full path from root, e.g. a\b\c
	{
		return _Find(name);
	}

	String &operator[](String name)	   // if name is not present adds new pair with empty value
	{
		int i = _Find(name);
		if (i)	// found
			return _values[--i].second;

		_values.push_back(std::make_pair(name, String())); 
		return _values[_values.size() - 1].second;
		
	}
	String GetSection(String session_name) // all sub-strings for name
	{
		String s;
		size_t slen = session_name.length();
		for (auto p : _values)
			if (p.first.substr(0, slen) == session_name)
			{
				while (p.first.substr(0, slen) == session_name)
					s += p.second + "\n";
				return s;
			}
		return s;
	}

	void beginGroup(String s)
	{
		_group += s + "\\";
	}

	void endGroup()
	{
		if (_group.isEmpty())
			return;
		_group = _group.left(_group.length() - 1);	// cut ending '\'
		size_t ind = _group.lastIndexOf('\\');		// get start of last group
		if (ind != NPOS)
			_group = _group.left(ind + 1);			// delete last group, leaving the '\'
		else
			_group.clear();							// no more '\' at end: last group
	}

	int Read(String name)
	{
		std::ifstream ifs(name);
		char buf[1024];

		if (!ifs.bad())
		{
			_values.clear();

			while (ifs.getline(buf, 1024, '\n'))
			{
				if(!_SetGroupRootFrom(buf))		// if [group name] => set _groups
					_AddPair(buf);				// else add lines to actual group
			}
			return (int)_values.size();
		}
		return 0;
	}

	void Write(String name)
	{
		std::ofstream ofs(name);
		if (ofs.bad())
			throw "can't write";
		_group.clear();
		String s;
		size_t len = 0;

		for (auto v : _values)
		{
			auto n = v.first.indexOf('\\');
			if (n != NPOS)
			{
				s = v.first.left(n);		// first group ?
				if (_group != s)
				{
					_group = s;
					len = s.length();
					ofs << "[" << s << "]\n";
				}
				ofs << s.mid(len) << "=" << v.second << "\n";
			}
			else
				ofs << v.first << v.second << "\n";
		}
	}

	void setValue(String name, bool value)	// name relative to _group
	{
		setValue(name, std::to_string(value));
	}
	void setValue(String name, int value)	// name relative to _group
	{
		setValue(name, std::to_string(value));
	}
	void setValue(String name, double value)	// name relative to _group
	{
		setValue(name, std::to_string(value));
	}
	void setValue(String name, String value)	// name relative to _group
	{
		if(_group.length())
			name = name.mid(_group.length());
		_AddPair(name + "=" + value);
	}

	ValueItem value(String name, String defVal)
	{
		name = _group + name;
		if (!_Find(name))
			return ValueItem(defVal);
		return ValueItem(operator[](name));
	}
	ValueItem value(String name, bool defVal)
	{
		return value(name, std::to_string(defVal));
	}
	ValueItem value(String name, int defVal)
	{
		return value(name, std::to_string(defVal));
	}
	ValueItem value(String name, double defVal)
	{
		return value(name, std::to_string(defVal));
	}
};

/*========================================================
 * Settings class
 * REMARKS: - settings are stored in ini format files, BUT
 *			- keys are case sensitive!
 *			- top level group in []
 *			- other groups start with a backslash
 *-------------------------------------------------------*/
class Settings	: public ValueVector
{
	String	_sIniName,		// name of input/output files
			_sOName;		// special name for e.g. organization

public:
	Settings() {};
	Settings(String iniName) { if( ! _sIniName.isEmpty() && _sIniName != iniName) Read(iniName); }
	Settings(String oName, String iniName) : _sIniName(iniName), _sOName(oName) { Read(iniName); }
	void Store() { Write(_sIniName); }
	~Settings()
	{

		if (!_changed)
			return;
		Write(_sIniName);
	}

	void SetNames(String oName, String iniName) { _sIniName = iniName; _sOName = oName; Read(iniName); }
};

