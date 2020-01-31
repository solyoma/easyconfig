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


void BOOL_FIELD::Store(Settings &s)
{
	if (value != defVal)
	{
		s.beginGroup(name);
			s.setValue("kind", "b");
			s.setValue("value", value);
			s.setValue("default", defVal);
		s.endGroup();
	}
	changed = false;
}

void BOOL_FIELD::Retrieve(Settings & s)
{
	s.beginGroup(name);
		if (s.value("kind","x").toString() != "b")
			throw "bool-field type mismatch";
		defVal = s.value("default", false).toBool();
		value = s.value("value", defVal).toBool();
	s.endGroup();

	changed = false;
}


void INT_FIELD::Store(Settings &s)
{
	if (value != defVal)
	{
		s.beginGroup(name);
			s.setValue("kind", "i");
			s.setValue("value", value);
			s.setValue("default", defVal);
		s.endGroup();
	}
	changed = false;
}

void INT_FIELD::Retrieve(Settings & s)
{
	s.beginGroup(name);
	if (s.value("kind", "x").toString() != "i")
		throw "int-field type mismatch";
	defVal = s.value("default", 0).toInt();
	value = s.value("value", defVal).toInt();
	s.endGroup();

	changed = false;
}

void REAL_FIELD::Store(Settings &s)
{
	if (value != defVal)
	{
		s.beginGroup(name);
			s.setValue("kind", "r");
			s.setValue("value", value);
			s.setValue("default:", defVal);
		s.endGroup();
	}
	changed = false;
}

void REAL_FIELD::Retrieve(Settings & s)
{
	s.beginGroup(name);
	if (s.value("kind", "x").toString() != "r")
		throw "real-field type mismatch";
	defVal = s.value("default", 0.0).toDouble();
	value = s.value("value", defVal).toDouble();
	s.endGroup();

	changed = false;
}

void TEXT_FIELD::Store(Settings &s)
{
	if (value != defVal)
	{
		s.beginGroup(name);
			s.setValue("kind", "t");
			s.setValue("value", value);
			s.setValue("default:", defVal);
		s.endGroup();
	}
	changed = false;
}

void TEXT_FIELD::Retrieve(Settings & s)
{
	s.beginGroup(name);
		if (s.value("kind", "x").toString() != "t")
			throw "text-field type mismatch";
		defVal = s.value("default", false).toString();
		value = s.value("value", defVal).toString();
	s.endGroup();

	changed = false;
}

void COMPOUND_FIELD::Store(Settings &s)
{
	s.beginGroup(name);
		s.setValue("kind", "c");
		s.setValue("value", value);
		s.setValue("default:", defVal);
	for (auto pf : _fields)
	{
		if (pf.second->kind != ackNone)
			pf.second->Store(s);
	}
	s.endGroup();

	changed = false;
}

void COMPOUND_FIELD::Retrieve(Settings & s)
{
	s.beginGroup(name);
		if (s.value("kind", "x").toString() != "b")
			throw "compound-field type mismatch";
		defVal = s.value("default", false).toString();
		value = s.value("value", defVal).toString();
	s.endGroup();

	changed = false;
}

BOOL_FIELD& BOOL_FIELD::operator=(bool newVal)
{
	if (changed = (newVal != value))
	{
		value = newVal;
		if (parent)
			parent->SetChanged(true);
	}
	return *this;
}

INT_FIELD& INT_FIELD::operator=(int newVal)
{
	if (changed = (newVal != value))
	{
		value = newVal;
		if (parent)
			parent->SetChanged(true);
	}
	return *this;
}
REAL_FIELD& REAL_FIELD::operator=(double newVal)
{
	if (changed = (newVal != value))
	{
		value = newVal;
		if (parent)
			parent->SetChanged(true);
	}
	return *this;
}
//-------------------------------------
TEXT_FIELD& TEXT_FIELD::operator=(String newVal)
{
	if (changed = (newVal != value))
	{
		value = newVal;
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

void COMPOUND_FIELD::AddBoolField(String name, bool defVal, bool val)
{
	BOOL_FIELD intf(name, defVal, val, this);
	_boolList.push_back(intf);
	if (_fields.count(name) && _fields[name]->kind != ackBool)
		throw "type mismatch";
	_fields[name] = &_boolList.back();
}
void COMPOUND_FIELD::AddIntField(String name, int defVal, int val)
{
	INT_FIELD intf(name, defVal, val, this);
	_intList.push_back(intf);
	if (_fields.count(name) && _fields[name]->kind != ackInt)
		throw "type mismatch";
	_fields[name] = &_intList.back();
}
void COMPOUND_FIELD::AddRealField(String name, double defVal, double val)
{
	REAL_FIELD intf(name, defVal, val, this);
	_realList.push_back(intf);
	if (_fields.count(name) && _fields[name]->kind != ackReal)
		throw "type mismatch";
	_fields[name] = &_realList.back();
}
void COMPOUND_FIELD::AddTextField(String name, String defVal, String val)
{
	TEXT_FIELD textf(name, defVal, val, this);
	size_t n = _textList.size();
	_textList.push_back(textf);
	if (_fields.count(name) && _fields[name]->kind != ackText)
		throw "type mismatch";
	_fields[name] = &_textList.back();
}

FIELD_BASE *COMPOUND_FIELD::operator[](String fieldn)
{

	if (_fields.count(fieldn))
		return _fields[fieldn];
	return nullptr;
}

void COMPOUND_FIELD::_CopyLists(const COMPOUND_FIELD &other)
{
	changed = other.changed;
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
	Settings s(fname);

	changed = false;
}

void ACONFIG::_Store(Settings &s, FIELD_BASE *pf)
{
//	s.beginGroup(pf->name);
//		s.setValue("kind", pf->kind);
		switch (pf->kind)		// no variant type on std
		{
			case ackBool: s.setValue("value", static_cast<BOOL_FIELD*>(pf)->value);
						s.setValue("default", static_cast<BOOL_FIELD*>(pf)->defVal);
						break;
			case ackInt:  s.setValue("value", static_cast<INT_FIELD*>(pf)->value);
						s.setValue("default", static_cast<INT_FIELD*>(pf)->defVal);
						break;
			case ackReal: s.setValue("value", static_cast<REAL_FIELD*>(pf)->value);
						s.setValue("default", static_cast<REAL_FIELD*>(pf)->defVal);
						break;
			case ackText: s.setValue("value", static_cast<TEXT_FIELD*>(pf)->value);
						s.setValue("default", static_cast<TEXT_FIELD*>(pf)->defVal);
						break;
			case ackComp: s.setValue("value", static_cast<COMPOUND_FIELD*>(pf)->value);
						s.setValue("default", static_cast<COMPOUND_FIELD*>(pf)->defVal);
						for (auto pfs : static_cast<ACONFIG*>(pf)->_fields)
							_Store(s, pfs.second);
						break;
			default: break;
		}
//	s.endGroup();
}

void ACONFIG::Store(String fname)
{
	Settings s(fname);

	for (auto pfs : _fields)
	{
		auto pf = pfs.second;
		_Store(s, pf);

	}

	changed = false;
}

// DEBUG
//--------------------------------------------------------------
// writes field into open file in format
// <name>,<kind (one LC letter)>,<default>,<actual value>

void ACONFIG::DumpFields(ACONFIG_KIND kind, String file)
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

	FIELD_BASE *pf;
	for (auto a : _fields)
	{
		pf = nullptr;
		switch (kind)
		{
			case ackNone: pf = a.second; break;
			case ackBool:  if (a.second->kind == ackBool) pf = a.second; break;
			case ackInt:  if(a.second->kind == ackInt) pf = a.second; break;
			case ackReal:  if (a.second->kind == ackReal) pf = a.second; break;
			case ackText: if (a.second->kind == ackText) pf = a.second; break;
		}
		std::cout << "name: " << pf->name << ", kind: " << AconfigKindToString(pf->kind)
			<< ", changed: " << (pf->changed ? "yes" : "no") << ", default: ";
		switch (pf->kind)
		{
			case ackBool: std::cout << static_cast<BOOL_FIELD *>(pf)->defVal << ", value: " << static_cast<BOOL_FIELD *>(pf)->value << "\n"; break;
			case ackInt:  std::cout << static_cast<INT_FIELD *> (pf)->defVal << ", value: " << static_cast<INT_FIELD *>(pf)->value << "\n"; break;
			case ackReal: std::cout << static_cast<REAL_FIELD *>(pf)->defVal << ", value: " << static_cast<INT_FIELD *>(pf)-> value << "\n"; break;
			case ackText: std::cout << static_cast<TEXT_FIELD *>(pf)->defVal << ", value: " << static_cast<TEXT_FIELD *>(pf)->value << "\n"; break;
		}
	}
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
