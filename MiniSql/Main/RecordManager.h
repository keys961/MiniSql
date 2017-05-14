#pragma once
#include "Condition.h"
//#include "Attribute.h"
//#include "File.h"
#include "BufferManager.h"
#include <string>
#include <vector>

using namespace std;

class RecordManager
{
public:
	RecordManager();
	~RecordManager();
	BufferManager bufferManager;

	bool createTable(string tableName);
	bool dropTable(string tableName);

	bool createIndex(string indexName);
	bool dropIndex(string indexName);

	int insertRecord(string tableName, string record, int size); //Return the block offset
	int findRecord(string tableName, vector<Condition*> conditionList);

	string getTableFileName(string tableName); //Return table file name
	string getIndexFileName(string indexName); //Return index file name
private:
	int findRecordInBlock(string tableName, vector<Condition*> conditionList, Block* block);
};

