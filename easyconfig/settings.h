#pragma once
#include <string>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <fstream>

//******************************************
// Settings class: stores/retrieves data in/from files

using String = std::string;
using StringList = std::list<String>;
using StringMap = std::map<String, String>;
using ValuePair = std::pair<String, String>;

class VelueVector 
{
	std::vector<ValuePair> _values;
	int _nVpIndex;	// in _values;

	int _Find(String name)	//0: not found, other *** index+1 *** of element
	{
		_nVpIndex = 0;
		for (auto p : _values)
			if (p.first == name)
				return _nVpIndex;
			else
				++_nVpIndex;
		return _nVpIndex;
	}
public:
	String &operator[](String name)
	{
		static String _dummy;
		int i = _Find(name);
		if (i)	// found
			return &_values[--i].second;
		return _dummy;
	}
};

class SettingsGroup 
{
	String sGroupName;

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


