#pragma once
#include <string>
#include <vector>
#include "Attribute.h"
#include "BufferManager.h"
#include "Index.h"

using namespace std;

class CatalogManager
{
public:

	const static int UNKNOWN_FILE = -1;
	const static int TABLE_FILE = 1;
	const static int INDEX_FILE = 2;

	BufferManager bufferManager;
	CatalogManager();
	~CatalogManager();
	bool addTable(string tableName, vector<Attribute>* attriList);
	int findTable(string tableName);

};

