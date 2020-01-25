#include "aconfig.h"

std::string AconfigKindToString(ACONFIG_KIND kind)
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


void BOOL_FIELD::Store() { parent->WriteBool(this); }
void INT_FIELD::Store() { parent->WriteInt(this); }
void REAL_FIELD::Store() { parent->WriteReal(this); }
void TEXT_FIELD::Store() { parent->WriteText(this); }

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
TEXT_FIELD& TEXT_FIELD::operator=(std::string newVal)
{
	if (changed = (newVal != value))
	{
		value = newVal;
		if (parent)
			parent->SetChanged(true);
	}
	return *this;
}

void ACONFIG::AddBoolField(std::string name, bool defVal, bool val)
{
	BOOL_FIELD intf(name, defVal, val, this);
	_boolList.push_back(intf);
	if (_fields.count(name) && _fields[name]->kind != ackBool)
		throw "type mismatch";
	_fields[name] = &_boolList.back();
}
void ACONFIG::AddIntField(std::string name, int defVal, int val)
{
	INT_FIELD intf(name, defVal, val, this);
	_intList.push_back(intf);
	if (_fields.count(name) && _fields[name]->kind != ackInt)
		throw "type mismatch";
	_fields[name] = &_intList.back();
}
void ACONFIG::AddRealField(std::string name, double defVal, double val)
{
	REAL_FIELD intf(name, defVal, val, this);
	_realList.push_back(intf);
	if (_fields.count(name) && _fields[name]->kind != ackReal)
		throw "type mismatch";
	_fields[name] = &_realList.back();
}
void ACONFIG::AddTextField(std::string name, std::string defVal, std::string val)
{
	TEXT_FIELD textf(name, defVal, val, this);
	size_t n = _textList.size();
	_textList.push_back(textf);
	if (_fields.count(name) && _fields[name]->kind != ackText)
		throw "type mismatch";
	_fields[name] = &_textList.back();
}

void ACONFIG::Load(std::string fname)	// from file
{
	_ifs.open(fname);
	if (!_ifs.is_open())
		throw "can't open";
	std::vector < std::string> sl;
	splitstring s;
	while (!_ifs.eof())
	{
		_ifs >> s;
		sl = s.split(',', 1);
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

void ACONFIG::Store(std::string fname)
{
	_ofs.open(fname.c_str());
	if (!_ofs.is_open())
		throw "can't open";

	for (auto p : _fields)
		p.second->Store();
	_ofs.close();
}
// DEBUG
FIELD_BASE *ACONFIG::operator[](std::string fieldn)
{

	if (_fields.count(fieldn))
		return _fields[fieldn];
	return nullptr;
}
ACONFIG &ACONFIG::CopyFrom(const ACONFIG &other)
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

//--------------------------------------------------------------
// writes field into open file in format
// <name>,<kind (one LC letter)>,<default>,<actual value>

void ACONFIG::WriteBool(BOOL_FIELD *pf)
{
	_ofs << pf->name << ",b," << pf->defVal << ","<< pf->value << "\n";
	pf->changed = false;
}

void ACONFIG::WriteInt(INT_FIELD *pf)
{
	_ofs << pf->name << ",i," << pf->defVal << "," << pf->value << "\n";
	pf->changed = false;
}

void ACONFIG::WriteReal(REAL_FIELD *pf)
{
	_ofs << pf->name << ",r," << pf->defVal << "," << pf->value << "\n";
	pf->changed = false;
}

void ACONFIG::WriteText(TEXT_FIELD *pf)
{
	_ofs << pf->name << ",t," << pf->defVal << "," << pf->value << "\n";
	pf->changed = false;
}

void ACONFIG::DumpFields(ACONFIG_KIND kind, std::string file)
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

//void ACONFIG::Load(std::string fname)
//{
//		// load each field from file as text and store it 
//		// file contains lines for each field
//		//	<name>,<type>,<value>,<defval>
//
//}
//void ACONFIG::Store(std::string fname)
//{
//
//}
