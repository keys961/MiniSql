#pragma once
#include "stdafx.h"
#include "CatalogManager.h"
using namespace std;
class API
{
public:
	API();
	bool createTable(string tableName, vector<Attribute>* attriList, string pKeyName, int pKeyPos);
	bool createIndex(string indexName, string tableName, string attriName);
	bool dropTable(string tableName);
	bool dropIndex(string indexName);
	bool select(string tableName, vector<Condition> *conditionList);
	bool insert(string tableName, string *);
	bool deleteFromTable(string tableName, vector<Condition> *conditionList);
	~API();
};

