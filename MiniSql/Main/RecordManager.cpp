#include "stdafx.h"
#include "RecordManager.h"

extern FILE* output;

RecordManager::RecordManager()
{
	this->bufferManager = new BufferManager();
}

RecordManager::~RecordManager()
{
	delete bufferManager;
}
//Create empty table file
bool RecordManager::createTable(string tableName)
{
	string fileName = (tableName);
	FILE* fp = fopen(fileName.c_str(), "w+");
	if (!fp)
		return false;
	fclose(fp);
	return true;
}
//Drop table file & clear relative slots
bool RecordManager::dropTable(string tableName)
{
	string fileName = (tableName);
	bufferManager->deleteFile(fileName);//Clear slots
	if (remove(fileName.c_str())) //Remove file from disk
		return false;
	return true;
}
//Create empty index file
bool RecordManager::createIndex(string indexName)
{
	string fileName = (indexName);
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
	string fileName = (indexName);
	bufferManager->deleteFile(fileName);//Clear slots
	if (remove(fileName.c_str())) //Remove file from disk
		return false;
	return true;
}
//Insert a record, return the offset number
//Record is length-fixed
//size is the single record length
//NOTE: We may need indexManager to judge the uniqueness, 
//thus the judgement of uniqueness may be done in API level
int RecordManager::insertRecord(string tableName, char* recordContent, int size)
{
	File* ftemp = bufferManager->getFile(tableName);
	Block* btemp = bufferManager->getBlockHead(ftemp);
	if (btemp == NULL)
		return -1;
	while (true)
	{
		if (bufferManager->getUsedSize(*btemp) <= MAX_BLOCK_SIZE - sizeof(size_t) - size)
		{
			char* beginAddr = bufferManager->getContent(*btemp)
				+ bufferManager->getUsedSize(*btemp);
			memcpy(beginAddr, recordContent, size); //Insert
			bufferManager->setDirty(*btemp, true);
			bufferManager->setUsedSize(*btemp, bufferManager->getUsedSize(*btemp) + size);
			return btemp->offset;
		}
		btemp = bufferManager->getNextBlock(ftemp, btemp);
	}
	return -1;
}
//Find right records in a table
//return the number of the records
int RecordManager::findRecord(string tableName, vector<Attribute>* attriList, vector<Condition> *conditionList)
{
	File* ftemp = bufferManager->getFile(tableName);
	Block* btemp = bufferManager->getBlockHead(ftemp);
	int count = 0;
	if (btemp == NULL)
		return -1; //ERROR
	while (true)
	{
		count += findRecordInBlock(tableName, attriList, conditionList, btemp);
		if (btemp->end)
			return count;
		btemp = bufferManager->getNextBlock(ftemp, btemp);
	}
	return 0;
}
//Show all record in the console that fits the conditions
int RecordManager::showRecord(string tableName, vector<Attribute>* attriList, vector<Condition>* conditionList)
{
	File* ftemp = bufferManager->getFile(tableName);
	Block* btemp = bufferManager->getBlockHead(ftemp);
	int count = 0;
	if (btemp == NULL)
		return count;
	while (true)
	{
		count += showRecordInBlock(tableName, attriList, conditionList, btemp);
		if (btemp->end)
			return count;
		btemp = bufferManager->getNextBlock(ftemp, btemp);
	}
}
//Delete all record that fits the condition
int RecordManager::deleteRecord(string tableName, vector<Attribute>* attriList, vector<Condition>* conditionList)
{
	File* ftemp = bufferManager->getFile(tableName);
	Block* btemp = bufferManager->getBlockHead(ftemp);
	int count = 0;
	if (btemp == NULL)
		return count;
	while (true)
	{
		count += deleteRecordInBlock(tableName, attriList, conditionList, btemp);
		if (btemp->end)
			return count;
		btemp = bufferManager->getNextBlock(ftemp, btemp);
	}
	return false;
}

size_t RecordManager::getTypeSize(int type)
{
	switch (type)
	{
	case Attribute::INT: return sizeof(int);
	case Attribute::FLOAT: return sizeof(float);
	default: return (size_t)type;
	}
}
//Find right records in a block
//return the number of the records
int RecordManager::findRecordInBlock(string tableName, vector<Attribute>* attriList, vector<Condition>* conditionList, Block * block)
{
	if (!block)
		return -1;
	int count = 0;
	int recordSize = 0;
	char* beginAddr = bufferManager->getContent(*block);
	char* tempAddr = bufferManager->getContent(*block);
	if (block->offset == 0) //If head
		tempAddr += (*attriList).size() * sizeof(Attribute) + sizeof(int) + 2;
	for (int i = 0; i < attriList->size(); i++)
		recordSize += getTypeSize((*attriList)[i].type);

	while (tempAddr - beginAddr < bufferManager->getUsedSize(*block))
	{
		if (fitCondition(tempAddr, recordSize, attriList, conditionList))
			count++;
		tempAddr += recordSize;
	}
	return count;

}
int RecordManager::showRecordInBlock(string tableName, vector<Attribute>* attriList, vector<Condition>* conditionList, Block * block)
{
	int count = 0;
	if (block == NULL)
		return count;
	char* addr = bufferManager->getContent(*block);
	char* temp = bufferManager->getContent(*block);
	int recordSize = 0;
	for (int i = 0; i < attriList->size(); i++)
		recordSize += getTypeSize((*attriList)[i].type);
	if(block->offset == 0)//Head
		temp += (*attriList).size() * sizeof(Attribute) + sizeof(int) + 2;
	while (temp - addr < bufferManager->getUsedSize(*block))
	{
		if (fitCondition(temp, recordSize, attriList, conditionList))
		{
			showSingleRecord(temp, recordSize, attriList);
			count++;
		}
		temp += recordSize;
	}
	return count;
}
void RecordManager::showSingleRecord(char * content, int size, vector<Attribute>* attriList)
{
	char* addr = content;
	char element[255];
	int type, typeSize;
	for (int i = 0; i < attriList->size(); i++)
	{
		type = attriList->at(i).type;
		typeSize = getTypeSize(type);
		memset(element, 0, 255);
		memcpy(element, addr, typeSize);
		switch (type)
		{
		case Attribute::INT: printf("| %d ", *(int*)element); fprintf(output, "| %d ", *(int*)element);  break;
		case Attribute::FLOAT: printf("| %f ", *(float*)element); fprintf(output, "| %f ", *(float*)element);  break;
		default: printf("| %s ", element); fprintf(output, "| %s ", element);
		}
		addr += typeSize;
	}
	printf("|\n");
}
int RecordManager::deleteRecordInBlock(string tableName, vector<Attribute>* attriList, vector<Condition>* conditionList, Block * block)
{
	int count = 0;
	if (block == NULL)
		return count;
	char* addr = bufferManager->getContent(*block);
	char* temp = bufferManager->getContent(*block);
	int recordSize = 0;
	for (int i = 0; i < attriList->size(); i++)
		recordSize += getTypeSize((*attriList)[i].type);
	if (block->offset == 0)//Head
		temp += (*attriList).size() * sizeof(Attribute) + sizeof(int) + 2;
	while (temp - addr < bufferManager->getUsedSize(*block))
	{
		if (fitCondition(temp, recordSize, attriList, conditionList))
		{
			char* lastRecord = addr + bufferManager->getUsedSize(*block) - recordSize;
			memcpy(temp, lastRecord, recordSize);
			memset(lastRecord, 0, recordSize);
			bufferManager->setDirty(*block, true);
			bufferManager->setUsedSize(*block, bufferManager->getUsedSize(*block) - recordSize);
			count++;
		}
		else
			temp += recordSize;
	}
	return count;
}
//Test an record is met conditions or not
bool RecordManager::fitCondition(char * recordContent, int size, vector<Attribute>* attriList, vector<Condition>* conditionList)
{
	if (conditionList == NULL || conditionList->empty())//No condition
		return true;

	char* addr = recordContent;
	string attriName;
	char element[255];
	int type, typeSize;
	for (int i = 0; i < attriList->size(); i++)
	{
		type = attriList->at(i).type;
		typeSize = getTypeSize(type);
		attriName = attriList->at(i).name;
		memset(element, 0, 255);
		memcpy(element, addr, typeSize);
		for (int j = 0; j < conditionList->size(); j++)
		{
			if (conditionList->at(j).getAttribute() == attriName)
				if (!compareElement(element, type, &(*conditionList)[j]))
					return false;
		}
		addr += typeSize;//Next element
	}
	return true;
}
//Compare the element meets the condition or not, element is the right value
//left value & compare operation is stored in condition objects
bool RecordManager::compareElement(char * element, int type, Condition * condition)
{
	switch (type)
	{
	case Attribute::INT: return condition->compare(*(int*)element);
	case Attribute::FLOAT: return condition->compare(*(float*)element);
	default: return condition->compare(element);
	}
}

