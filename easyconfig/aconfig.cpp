#include "aconfig.h"
#include <sstream>

String AconfigKindToString(ACONFIG_KIND kind)
{
	switch (kind)
	{
		case ackBool: return "bool field";
		case ackInt:  return "int field";
		case ackReal: return "real field";
		case ackText: return "text field";
		case ackNone: 
		default:		return "undefined";
	}
}

ACONFIG_KIND KindFromString(String s)
{
	switch (s[0])
	{
		case 'b': return ackBool;
		case 'c': return ackComp;
		case 'i': return ackInt;
		case 'r': return ackReal;
		case 't': return ackText;
	}
	return ACONFIG_KIND();
}


void BOOL_FIELD::Store()
{
	s.beginGroup(name);
		s.setValue("kind", "b");
		s.setValue("value", _value);
		s.setValue("default", _defVal);
	s.endGroup();
}

void BOOL_FIELD::Retrieve()
{
	s.beginGroup(name);
		if (s.value("kind","x").toString() != "b")
			throw "bool-field expected";
		_defVal = s.value("default", false).toBool();
		_value = s.value("value", _defVal).toBool();
	s.endGroup();
}


void INT_FIELD::Store()
{
	s.beginGroup(name);
		s.setValue("kind", "i");
		s.setValue("value", _value);
		s.setValue("default", _defVal);
	s.endGroup();
}

void INT_FIELD::Retrieve()
{
	s.beginGroup(name);
	if (s.value("kind", "x").toString() != "i")
		throw "int-field expected";
	 _defVal = s.value("default", 0).toInt();
	 _value = s.value("value", _defVal).toInt();
	s.endGroup();

}

void REAL_FIELD::Store()
{
		s.beginGroup(name);
			s.setValue("kind", "r");
			s.setValue("value", _value);
			s.setValue("default", _defVal);
		s.endGroup();
}

void REAL_FIELD::Retrieve()
{
	s.beginGroup(name);
	if (s.value("kind", "x").toString() != "r")
		throw "real-field expected";
	 _defVal = s.value("default", 0.0).toDouble();
	 _value = s.value("value", _defVal).toDouble();
	s.endGroup();
}

void TEXT_FIELD::Store()
{
	s.beginGroup(name);
		s.setValue("kind", "t");
		s.setValue("value", _value);
		s.setValue("default", _defVal);
	s.endGroup();
}

void TEXT_FIELD::Retrieve()
{
	s.beginGroup(name);
		if (s.value("kind", "x").toString() != "t")
			throw "text-field expected";
		_defVal = s.value("default", false).toString();
		_value = s.value("value", _defVal).toString();
	s.endGroup();
}

void COMPOUND_FIELD::Store()
{
	if (!name.isEmpty())
	{
		s.beginGroup(name);
			s.setValue("kind", "c");
			s.setValue("value", _value);
			s.setValue("default", _defVal);
	}
	for (auto pf : _fields)
	{
		if (pf.second->kind != ackNone)
			pf.second->Store();
	}
	if (!name.isEmpty())
		s.endGroup();
}

void COMPOUND_FIELD::Retrieve()
{
	s.beginGroup(name);
		if (s.value("kind", "x").toString() != "b")
			throw "compound-field expected";
		 _defVal = s.value("default", false).toString();
		 _value = s.value("value", _defVal).toString();
	s.endGroup();
}

BOOL_FIELD& BOOL_FIELD::operator=(bool newVal)
{
	if (newVal != _value)
	{
		_value = newVal;
		if (parent)
			parent->SetChanged(true);
	}
	return *this;
}

INT_FIELD& INT_FIELD::operator=(int newVal)
{
	if (newVal != _value)
	{
		_value = newVal;
		if (parent)
			parent->SetChanged(true);
	}
	return *this;
}
REAL_FIELD& REAL_FIELD::operator=(double newVal)
{
	if (newVal != _value)
	{
		 _value = newVal;
		if (parent)
			parent->SetChanged(true);
	}
	return *this;
}
//-------------------------------------
TEXT_FIELD& TEXT_FIELD::operator=(String newVal)
{
	if (newVal != _value)
	{
		 _value = newVal;
		if (parent)
			parent->SetChanged(true);
	}
	return *this;
}

size_t COMPOUND_FIELD::Size(ACONFIG_KIND kind) const
{
	switch (kind)
	{
		case ackBool:	return _boolList.size();
		case ackInt:	return _intList.size();
		case ackReal:	return _realList.size();
		case ackText:	return _textList.size();
		default:		return _fields.size();
	}
}

void COMPOUND_FIELD::AddBoolField(Settings &s, String name, bool _defVal, bool val)
{
	BOOL_FIELD intf(s, name, _defVal, val, parent);
	_boolList.push_back(intf);
	if (_fields.count(name) && _fields[name]->kind != ackBool)
		throw "bool field expected";
	_fields[name] = &_boolList.back();
}
void COMPOUND_FIELD::AddIntField(Settings &s, String name, int _defVal, int val)
{
	INT_FIELD intf(s, name, _defVal, val, parent);
	_intList.push_back(intf);
	if (_fields.count(name) && _fields[name]->kind != ackInt)
		throw "int field expected";
	_fields[name] = &_intList.back();
}
void COMPOUND_FIELD::AddRealField(Settings &s, String name, double _defVal, double val)
{
	REAL_FIELD intf(s, name, _defVal, val, parent);
	_realList.push_back(intf);
	if (_fields.count(name) && _fields[name]->kind != ackReal)
		throw "real field expected";
	_fields[name] = &_realList.back();
}
void COMPOUND_FIELD::AddTextField(Settings &s, String name, String _defVal, String val)
{
	TEXT_FIELD textf(s, name, _defVal, val, parent);
	size_t n = _textList.size();
	_textList.push_back(textf);
	if (_fields.count(name) && _fields[name]->kind != ackText)
		throw "text field expected";
	_fields[name] = &_textList.back();
}

COMPOUND_FIELD *COMPOUND_FIELD::AddCompField(Settings &s, String name, String _defVal, String val)
{
	COMPOUND_FIELD compf(s, name, _defVal, val, parent);
	size_t n = _textList.size();
	_compList.push_back(compf);
	if (_fields.count(name) && _fields[name]->kind != ackComp)
		throw "compound field expected";
	_fields[name] = &_compList.back();
	return &_compList.back();
}

FIELD_BASE *COMPOUND_FIELD::operator[](String fieldn)
{

	if (_fields.count(fieldn))
		return _fields[fieldn];
	return nullptr;
}

void COMPOUND_FIELD::_CopyLists(const COMPOUND_FIELD &other)
{
	_intList = other._intList;
	_boolList = other._boolList;
	_realList = other._realList;
	_textList = other._textList;
	_compList = other._compList;

	_fields.clear();
	// set up new _fields
	for (auto a : _boolList)
		_fields[a.name] = &a;
	for (auto a : _intList)
		_fields[a.name] = &a;
	for (auto a : _realList)
		_fields[a.name] = &a;
	for (auto a : _textList)
		_fields[a.name] = &a;
	for (auto a : _compList)
		_fields[a.name] = &a;
}

COMPOUND_FIELD &COMPOUND_FIELD::operator=(const COMPOUND_FIELD &other)
{
	TEXT_FIELD::operator=(other);
	_CopyLists(other);

	return *this;
}

void ACONFIG::Load(String fname)	// from ini file
{
	Clear();		// all data

	_settings.Load(fname);	   // reads into 's'

	StringList sl = _settings.allKeys();	// create config. variables
	size_t pos;
	String name;
	
	for (auto key : sl)
	{
		pos = key.indexOf(SETTINGS_DELIMITER);
		if (pos != NPOS)
		{
			if (key.left(pos) == name)
				continue;
			name = key.left(pos);
			_AddFieldFromSettings(_settings, name);
		}
	}

	_changed = false;
}

void ACONFIG::_Store(FIELD_BASE *pf)
{
		switch (pf->kind)		// no variant type on std
		{
			case ackBool: static_cast<BOOL_FIELD*>(pf)->Store();
						break;
			case ackInt:  static_cast<INT_FIELD*>(pf)->Store();
						break;
			case ackReal: static_cast<REAL_FIELD*>(pf)->Store();
						break;
			case ackText: static_cast<TEXT_FIELD*>(pf)->Store();
						break;
			case ackComp: static_cast<COMPOUND_FIELD*>(pf)->Store();
						break;
			default: break;
		}
}

void ACONFIG::_AddFieldFromSettings(Settings &s, String name)
{
	if(!name.isEmpty())
		s.beginGroup(name); 

	ACONFIG_KIND kind = KindFromString(s.value("kind", "u").toString());

	if (!name.isEmpty())
		s.endGroup();
	switch (kind)
	{
		case ackBool:	s.beginGroup(name);
							AddBoolField(name, s.value("default", false).toBool(), s.value("value", false).toBool());
						s.endGroup();
			break;
		case ackInt:	s.beginGroup(name);
							AddIntField(name, s.value("default", 0).toInt(), s.value("value", 0).toInt());
						s.endGroup();
						break;
		case ackReal:	s.beginGroup(name);
							AddRealField(name, s.value("default", 0.0).toDouble(), s.value("value", 0.0).toDouble());
						s.endGroup();
						break;
		case ackText:	s.beginGroup(name);
							AddTextField(name, s.value("default", "").toString(), s.value("value", "").toString());
						s.endGroup();
						break;
		case ackComp:	s.beginGroup(name);
							AddCompField(name, s.value("default", "").toString(), s.value("value", "").toString());
						s.endGroup();
						break;
			default: break;
	}
}

void ACONFIG::Store()		// write to disk after calling this or in ~Settings()
{
	_pFields->Store();
	_changed = false;
}

// DEBUG
//--------------------------------------------------------------
// writes field into open file in format
// <name>,<kind (one LC letter)>,<default>,<actual _value>
void COMPOUND_FIELD::_DumpFields(std::ostream &ofs, ACONFIG_KIND kind)
{
	FIELD_BASE *pf;
	for (auto a : _fields)
	{
		pf = nullptr;
		switch (kind)
		{
			case ackNone: pf = a.second; break;
			case ackBool: if (a.second->kind == ackBool) pf = a.second; break;
			case ackInt:  if (a.second->kind == ackInt) pf = a.second; break;
			case ackReal: if (a.second->kind == ackReal) pf = a.second; break;
			case ackText: if (a.second->kind == ackText) pf = a.second; break;
			case ackComp: if (a.second->kind == ackComp) pf = a.second; break;
		}
		std::cout << "name: " << pf->name << ", kind: " << AconfigKindToString(pf->kind)
			<< ", default: ";
		switch (pf->kind)
		{
			case ackBool: std::cout << static_cast<BOOL_FIELD *>(pf)->Default() << ", value: " << static_cast<BOOL_FIELD *>(pf)->Value() << "\n"; break;
			case ackInt:  std::cout << static_cast<INT_FIELD *> (pf)->Default() << ", value: " << static_cast<INT_FIELD *>(pf)->Value() << "\n"; break;
			case ackReal: std::cout << static_cast<REAL_FIELD *>(pf)->Default() << ", value: " << static_cast<REAL_FIELD *> (pf)->Value() << "\n"; break;
			case ackText: std::cout << static_cast<TEXT_FIELD *>(pf)->Default() << ", value: " << static_cast<TEXT_FIELD *>(pf)->Value() << "\n"; break;
			case ackComp: {
							std::cout << static_cast<COMPOUND_FIELD *>(pf)->Default() << ", value: " 
										<< static_cast<COMPOUND_FIELD *>(pf)->Value() << "\n";
							static_cast<COMPOUND_FIELD *>(pf)->_DumpFields(std::cout, kind);
						  }
						  break;
		}
	}
}
void COMPOUND_FIELD::DumpFields(ACONFIG_KIND kind, String file)
{
	std::ofstream ofs;
	auto coutbuf = std::cout.rdbuf();
	bool redirected = false;

	if (!file.empty())			// then redirect cout
	{
		ofs.open(file);
		if (ofs.is_open())
		{
			std::cout.rdbuf(ofs.rdbuf());
			redirected = true;
		}
	}

	_DumpFields(ofs, kind);

	if (redirected)
		std::cout.rdbuf(coutbuf);
}

//void ACONFIG::Load(String fname)
//{
//		// load each field from file as text and store it 
//		// file contains lines for each field
//		//	<name>,<type>,<value>,<defval>
//
//}
//void ACONFIG::Store(String fname)
//{
//
//}
