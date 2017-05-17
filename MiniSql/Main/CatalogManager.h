#ifndef CATALOG_MANAGER_H
#define CATALOG_MANAGER_H

//#include <string>
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

	/*const static int UNKNOWN_FILE = -1;
	const static int TABLE_FILE = 1;
	const static int INDEX_FILE = 2;*/

	BufferManager* bufferManager;
	CatalogManager();
	CatalogManager(BufferManager* bufferManager)
	{
		this->bufferManager = bufferManager;
	}
	~CatalogManager();
	//Operation on table info
	bool addTable(string tableName, vector<Attribute>* attriList, string pKeyName, int pKeyPos);
	bool findTable(string tableName);
	bool dropTable(string tableName);
	int getRecordNum(string tableName);
	bool updateRecordNum(string tableName, int num);
	size_t getRecordSize(string tableName);
	//Operation on attributes
	int getAttribute(string tableName, vector<Attribute>* attriList);
	size_t getTypeSize(int type);
	
	//Operation on indices
	bool addIndex(string indexName, string tableName, string attriName, int type);
	bool findIndex(string indexName);
	bool getIndex(vector<Index>* indexList, string tableName = "");
	int getIndexType(string indexName);
	bool dropIndex(string indexName);

private:
	bool getAllIndice(vector<Index>* indexList);
	bool setIndexOnAttribute(string tableName, string attriName, string indexName);
};

#endif // !CATALOG_MANAGER