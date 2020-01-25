// autoconfig.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "aconfig.h"

ACONFIG config, config1;

using std::cout;


void ShowElem(std::ostream &ofs, FIELD_BASE *pf)
{
	if (!pf)
		cout << "- does not exist\n";
	else
	{
		cout << "field: " << pf->name << ": ";
		switch (pf->kind)
		{
		case ackBool: {
						BOOL_FIELD *pif = static_cast<BOOL_FIELD*>(pf);
						cout << AconfigKindToString(ackBool) << " with default " << pif->DefToString()
							<< " value " << pif->ToString() << "\n";
					  }
			break;
		case ackInt: {
						INT_FIELD *pif = static_cast<INT_FIELD*>(pf);
						cout << AconfigKindToString(ackInt) << " with default " << pif->defVal
							<< " value " << pif->value << "\n";
					 }
			break;

		case ackReal: {
						REAL_FIELD *pif = static_cast<REAL_FIELD*>(pf);
						cout << AconfigKindToString(ackReal) << " with default " << pif->defVal
							<< " value " << pif->value << "\n";
					  }
			break;
		case ackText: {
						TEXT_FIELD *pif = static_cast<TEXT_FIELD*>(pf);
						cout << AconfigKindToString(ackText) << " with default: " << pif->defVal
							<< ", value: " << pif->value << "\n";
					 }
			break;
		}
	}

}

int main()
{
	for (int i = 0; i < 20; ++i)
	{
		config.AddBoolField(std::string("bool_field_") + std::to_string(i), false, true);
		config.AddIntField (std::string("int_field_") + std::to_string(i), 2*i, i);
		config.AddRealField(std::string("real_field_") + std::to_string(i), 2.718281828 * i, 3.1415926539*i);
		config.AddTextField(std::string("text_field_") + std::to_string(i), std::string("tfield")+ std::to_string(i), std::string("no-default"));
	}
	cout << "Dumping into 'dump1.txt'\n";
	config.DumpFields(ackNone, "dump1.txt");
	cout << "copied \n";
	config1.CopyFrom(config);
	cout << "Dumping copy into 'dump1_copied.txt'\n";
	config.DumpFields(ackNone, "dump1_copied.txt");

	cout << "Storing config1 into file 'aconfig.ini'\n";
	config.Store("aconfig.ini");
	cout << "Reading back from 'aconfig.ini'\n";
	config.Load("aconfig.ini");
	cout << "dumping into dump2.txt";
	config.DumpFields(ackNone, "dump2.txt");
	cout << "\nIndexing test:\n";

	ShowElem(cout, config["real_field_7"]);
	ShowElem(cout, config["bool_field_4"]);
	ShowElem(cout, config["int_field_10"]);
	ShowElem(cout, config["text_field_3"]);

	cout << "\nReady.\n";
}
