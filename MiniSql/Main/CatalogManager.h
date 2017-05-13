#pragma once
#include <string>
#include <vector>
#include "Attribute.h"
#include "BufferManager.h"
#include "Index.h"

using namespace std;

class CatalogManager
{
	/*class Table
	{
	public:
		string tableName;
		vector<Attribute> attriList;
		Attribute primaryKey;
	};*/
public:

	const static int UNKNOWN_FILE = -1;
	const static int TABLE_FILE = 1;
	const static int INDEX_FILE = 2;

	BufferManager bufferManager;
	CatalogManager();
	~CatalogManager();
	bool addTable(string tableName, vector<Attribute>* attriList, string pKeyName, int pKeyPos);
	bool findTable(string tableName);
	bool dropTable(string tableName);
};

