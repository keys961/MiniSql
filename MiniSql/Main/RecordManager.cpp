#include "stdafx.h"
#include "RecordManager.h"


RecordManager::RecordManager()
{
}


RecordManager::~RecordManager()
{
}
//Create empty table file
bool RecordManager::createTable(string tableName)
{
	string fileName = getTableFileName(tableName);
	FILE* fp = fopen(fileName.c_str(), "w+");
	if (!fp)
		return false;
	fclose(fp);
	return true;
}
//Drop table file & clear relative slots
bool RecordManager::dropTable(string tableName)
{
	string fileName = getTableFileName(tableName);
	bufferManager.deleteFile(fileName);//Clear slots
	if (remove(fileName.c_str())) //Remove file from disk
		return false;
	return true;
}
//Create empty index file
bool RecordManager::createIndex(string indexName)
{
	string fileName = getIndexFileName(indexName);
	FILE* fp = fopen(fileName.c_str(), "w+");
	if (!fp)
	{
		//fclose(fp);
		return false;
	}
	fclose(fp);
	return true;
}
//Drop index file & clear relative slots
bool RecordManager::dropIndex(string indexName)
{
	string fileName = getIndexFileName(indexName);
	bufferManager.deleteFile(fileName);//Clear slots
	if (remove(fileName.c_str())) //Remove file from disk
		return false;
	return true;
}



int RecordManager::insertRecord(string tableName, string record, int size)
{
	File* ftemp = bufferManager.getFile(tableName);
	Block* btemp = bufferManager.getBlockHead(ftemp);
	if (btemp == NULL)
		return -1;
	while (true)
	{
		if (bufferManager.getUsedSize(*btemp) <= bufferManager.getBlockSize() - size)
		{
			char* beginAddr = bufferManager.getContent(*btemp)
				+ bufferManager.getUsedSize(*btemp);
			memcpy(beginAddr, record.c_str(), size); //Insert
			bufferManager.setUsedSize(*btemp, bufferManager.getUsedSize(*btemp) + size);
			return btemp->offset;
		}
		btemp = bufferManager.getNextBlock(ftemp, btemp);
	}
	return -1;
}
//Return table file name
string RecordManager::getTableFileName(string tableName)
{
	return "Table_" + tableName;
}
//Return index file name
string RecordManager::getIndexFileName(string indexName)
{
	return "Index_" + indexName;
}

int RecordManager::findRecordInBlock(string tableName, vector<Condition*> conditionList, Block * block)
{
	if (!block)
		return -1;
	int count = 0;
	char* beginAddr = bufferManager.getContent(*block);

}
