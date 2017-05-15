#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include <string>
#include <iostream>

using namespace std;

class Attribute
{
public:
	int static const FLOAT = -1;
	int static const INT = 0;

	Attribute()
	{
		this->name = "";
		this->type = 0;
		this->unique = false;
	}

	Attribute(string name, int type, bool unique)
	{
		this->name = name;
		this->type = type;
		this->unique = unique;
	}
	virtual ~Attribute() {};
	//Get & Set
	//string getName();
	//void setName(string name);
	//int getType();
	//void setType(int type);
	//bool isUnique();
	//void setUnique(int unique);
	//void setIndexName(string indexName);
	//string getIndexName();
	////Print attribute
	void printAttribute()
	{
		string typeName;
		switch (type)
		{
		case -1: typeName = "Float"; break;
		case 0: typeName = "Int"; break;
		default: typeName = "Char(n)";
			break;
		}
		cout << "Attribute name: " << name <<
			"; Type: " << typeName << "; Unique: "
			<< unique << "; Index name:" << indexName
			<< endl;
	}
	string name;
	int type;//-1-float, 0-int, n-char(n)
	bool unique;
	string indexName;
};


//string Attribute::getName()
//{
//	return this->name;
//}
//
//void Attribute::setName(string name)
//{
//	this->name = name;
//}
//
//int Attribute::getType()
//{
//	return this->type;
//}
//
//void Attribute::setType(int type)
//{
//	this->type = type;
//}
//
//bool Attribute::isUnique()
//{
//	return this->unique;
//}
//
//void Attribute::setUnique(int unique)
//{
//	this->unique = unique;
//}
//
//void Attribute::setIndexName(string indexName)
//{
//	this->indexName = indexName;
//}
//
//string Attribute::getIndexName()
//{
//	return this->indexName;
//}

#endif