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

//--------------------------------------------------------------

void ACONFIG::WriteBool(BOOL_FIELD *pintf)
{
	_ofs << pintf->name << ",b," << pintf->value << ","<<pintf->defVal << "\n";
}

void ACONFIG::WriteInt(INT_FIELD *pintf)
{
	_ofs << pintf->name << ",i," << pintf->value << "," << pintf->defVal << "\n";
}

void ACONFIG::WriteReal(REAL_FIELD *pintf)
{
	_ofs << pintf->name << ",r," << pintf->value << "," << pintf->defVal << "\n";
}

void ACONFIG::WriteText(TEXT_FIELD *ptextf)
{
	_ofs << ptextf->name << ",t," << ptextf->value << "," << ptextf->defVal << "\n";
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
