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

class VelueVector 
{
	std::vector<ValuePair> _values; 
	int _nVpIndex;	// in _values;
	String _group;	// actual group

	bool _SetRootGroup(char *buf)
	{
		if (buf[0] == '[')	// root section header
		{
			char *p = strchr(buf, ']');
			if (!p)
				throw "bad section head";
			_group = String(buf + 1).left((p - buf) - 1);
			return true;
		}
		return false;
	}
	
	int _Find(String name)	// 0: not found, other *** index+1 *** of element
	{						// name: full path name
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
		String sKey = String(Trim(buf));
		if (!_group.isEmpty())
			sKey = _group + "\\" + sKey;
		_values.push_back( std::make_pair( sKey, String(Trim(p))) );

		return _values.size();
	}
	size_t _AddPair(String s) // format : [<white space>]<text>[<white space>]=[<white space>]<text>[<whitespace>][#<comment>]
	{
		char buf[1024];
		int len = s.length();
		if (len > 1023)
			len = 1023;
		strncpy(buf, s.c_str(), len);
		buf[len] = 0;
		return _AddPair(buf);
	}

public:
	bool Contains(String name)
	{
		return _Find(name);
	}

	String &operator[](String name)
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

	int Read(String name)
	{
		std::ifstream ifs(name);
		char buf[1024];

		if (!ifs.bad())
		{
			while (ifs.getline(buf, 1024, '\n'))
			{
				if(!_SetRootGroup(buf))
					_AddPair(buf);
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

			auto n = v.first.find_first_of('\\');
			if (n != String::npos)
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
};

/*========================================================
 * Settings class
 * REMARKS: - settings are stored in ini format files, BUT
 *			- keys are case sensitive!
 *			- top level group in []
 *			- other groups start with a backslash
 *-------------------------------------------------------*/
class Settings
{
	String	_sIniName,		// name of input/output files
			_sOName;		// special name for e.g. organization

	bool _changed = false;
	std::vector<ValuePair> _values;	   // < <full path name>, <value> >
	void _Read()
	{
		std::ifstream ifs(_sIniName);
		if (!ifs.is_open())
			return;


	}
public:
	struct Value
	{
		String name, defVal;		// set at  each query
		ValuePair vp;;			// (name, value) pairs set up on constucting Settings from file

		String toString() const { return  name.empty() ? defVal : name; }
		int toInt() const { return std::stoi(toString()); }
		int toBool() const { return std::stoi(toString()); }
		double toDouble() { return std::stod(toString()); }

	};


	Settings() {};
	Settings(String iniName) :_sIniName(iniName) { _Read(); }
	Settings(String oName, String iniName) : _sIniName(iniName), _sOName(oName) { _Read(); }
	~Settings()
	{

		if (!_changed)
			return;

		std::ofstream ofs(_sIniName);
		if (!ofs.is_open())
			return;

	}

	void SetNames(String oName, String iniName) { _sName = iniName; _sOName = oName; _Read(); }

	// Getters
	Value& value(String name, String defVal)
	{
		_value.name = name; _value.defVal = defVal;
		return _value;
	}
};


