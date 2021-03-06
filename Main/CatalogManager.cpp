#include "stdafx.h"
#include "CatalogManager.h"

//Index info stored in IndiceInfo file
//Attri & table info stored in each table file

CatalogManager::CatalogManager()
{
	bufferManager = new BufferManager();
}

CatalogManager::~CatalogManager()
{
	//delete bufferManager;
}
//Add a new table with attrs to the memory block(add table info TO THE TABLE FILE)
//Position is 0 means no primary key
bool CatalogManager::addTable(string tableName, vector<Attribute>* attriList, string pKeyName, int pKeyPos)
{
	FILE* fp = fopen(tableName.c_str(), "wb+");
	if (!fp)
		return false;
	fclose(fp);
	File* ftemp = bufferManager->getFile(tableName);
	Block* btemp = bufferManager->getBlockHead(ftemp);
	if (btemp)
	{
		char* addrBegin = bufferManager->getContent(*btemp);
		int* size = (int*)addrBegin;
		*size = 0;//First note number of record
		addrBegin += sizeof(int);
		*addrBegin = pKeyPos; //Second note pKey
		addrBegin++;//1 byte
		*addrBegin = (*attriList).size();//Third not attribute number
		addrBegin++;//1 byte
		for (int i = 0; i < (*attriList).size(); i++)//Copy attributes
		{
			memcpy(addrBegin, &(*attriList)[i], sizeof(Attribute));
			addrBegin += sizeof(Attribute);
		}
		bufferManager->setUsedSize(*btemp, bufferManager->getUsedSize(*btemp)
			+ (*attriList).size() * sizeof(Attribute) + sizeof(int) + 2);
		bufferManager->setDirty(*btemp, true);//Set dirty
		return true;
	}
	return false;
}
//Find table is existed or not on the disk
bool CatalogManager::findTable(string tableName)
{
	FILE* fp = fopen((tableName).c_str(), "rb");
	if (!fp)
		return false;
	fclose(fp);
	return true;
}
//Delete the table from the disk & memory block
bool CatalogManager::dropTable(string tableName)
{
	bufferManager->deleteFile(tableName.c_str());//Clear all buffer
	int status = 0;
	//char command[255] = { 0 };
	//sprintf(command, "del %s", tableName.c_str());
	status = remove((tableName).c_str());//Clear the disk
	if(status)
		return false;
	//system(command);
	return true;
}
//Get table record number
int CatalogManager::getRecordNum(string tableName)
{
	File* ftemp = bufferManager->getFile(tableName);
	Block* btemp = bufferManager->getBlockHead(ftemp);
	if (btemp)
		return *(int*)bufferManager->getContent(*btemp);//*(Addr of record number) 
	return -1;
}
//Update table record number with num
bool CatalogManager::updateRecordNum(string tableName, int num)
{
	File* ftemp = bufferManager->getFile(tableName);
	Block* btemp = bufferManager->getBlockHead(ftemp);
	if (btemp)
	{
		char* addr = bufferManager->getContent(*btemp);
		int* recordNum = (int*)addr;
		if (num < 0)
		{
			std::cerr << "Number can't be less than ZERO!" << endl;
			return false;
		}
		*recordNum = num;
		bufferManager->setDirty(*btemp, true);
		return true;
	}
	return false;
}
//Get single record size according to the attribute
size_t CatalogManager::getRecordSize(string tableName)
{
	File* ftemp = bufferManager->getFile(tableName);
	Block* btemp = bufferManager->getBlockHead(ftemp);
	if (btemp)
	{
		size_t size = 0;
		char* addr = bufferManager->getContent(*btemp) + sizeof(int) + 1;
		int count = *(addr++);
		Attribute* attri = (Attribute*)addr;
		for (int i = 0; i < count; i++, attri++)
			size += getTypeSize(attri->type);
		return size;
	}
	else
	{
		std::cerr << "Cannot get record size info" << endl;
		return 0;
	}
}
//Get all attributes of table in attriList, return pKey position
//-1 failed; 0 no pKey; other success
int CatalogManager::getAttribute(string tableName, vector<Attribute>* attriList)
{
	File* ftemp = bufferManager->getFile(tableName.c_str());
	Block* btemp = bufferManager->getBlockHead(ftemp);
	int pos = -1;
	if (btemp)
	{
		char* beginAddr = bufferManager->getContent(*btemp);
		beginAddr += sizeof(int);
		pos = *(beginAddr++);
		int attriSize = *beginAddr;
		beginAddr++;
		Attribute* attri = (Attribute*)beginAddr;
		for (int i = 0; i < attriSize; i++, attri++)
			attriList->push_back(*attri);		
	}
	return pos;
}
//Set attribute in table as index with indexName
//If revoke, set indexName as NULL or ""
//It doesn't call addIndex method
bool CatalogManager::setIndexOnAttribute(string tableName, string attriName, string indexName)
{
	File* ftemp = bufferManager->getFile(tableName.c_str());
	Block* btemp = bufferManager->getBlockHead(ftemp);
	if (btemp)
	{
		char* addrBegin = bufferManager->getContent(*btemp) + 1 + sizeof(int);
		int count = *addrBegin;
		addrBegin++;
		Attribute* attr = (Attribute*)addrBegin;
		for (int i = 0; i < count; i++, attr++)
		{
			if (attr->name == attriName)
			{
				attr->indexName = indexName;
				bufferManager->setDirty(*btemp, true);
				return true;
			}
		}
	}
	return false;
}
//Get type size
size_t CatalogManager::getTypeSize(int type)
{
	switch (type)
	{
		case Attribute::INT: return sizeof(int);
		case Attribute::FLOAT: return sizeof(float);
		default: return (size_t)type;
	}
}
//Add index info to FILE "IndicesInfo"
bool CatalogManager::addIndex(string indexName, string tableName, string attriName, int type)
{
	File* ftemp = bufferManager->getFile("IndicesInfo");//Store
	Block* btemp = bufferManager->getBlockHead(ftemp);
	Index index(indexName, tableName, attriName, type);
	while (true)
	{
		if (!btemp)
			return false;
		if (bufferManager->getUsedSize(*btemp) <= MAX_BLOCK_SIZE - sizeof(size_t) - sizeof(Index))//Can get space
		{
			char* addrBegin = bufferManager->getContent(*btemp) + bufferManager->getUsedSize(*btemp);
			memcpy(addrBegin, &index, sizeof(Index));
			bufferManager->setUsedSize(*btemp, bufferManager->getUsedSize(*btemp) + sizeof(Index));
			bufferManager->setDirty(*btemp, true);
			return this->setIndexOnAttribute(tableName, attriName, indexName);
		}
		else
			btemp = bufferManager->getNextBlock(ftemp, btemp);//Alloc next block
	}
	return false;
}
//Find indexName of index is existed or not
bool CatalogManager::findIndex(string indexName)
{
	FILE *fp = fopen(indexName.c_str(), "r");
	if (fp)
	{
		fclose(fp);
		return true;
	}
	//fclose(fp);
	return false;
	File *ftemp = bufferManager->getFile("IndicesInfo");
	Block* btemp = bufferManager->getBlockHead(ftemp);
	while(btemp)
	{
		char* addrBegin = bufferManager->getContent(*btemp);
		Index* index = (Index*)addrBegin;
		int count = bufferManager->getUsedSize(*btemp) / sizeof(Index);
		for (int i = 0; i < count; i++, index++)
		{
			if (index->indexName == indexName)
				return true;
		}
		//return false;
		btemp = bufferManager->getNextBlock(ftemp, btemp);
		if (btemp->end)//New next block with no content
			break;
	}
	return false;
}
//Get all the indice of one table, if table name is "", get all indice
bool CatalogManager::getIndex(vector<Index>* indexList, string tableName)
{
	if (tableName == "")
		return getAllIndice(indexList);
	File *ftemp = bufferManager->getFile("IndicesInfo");
	Block* btemp = bufferManager->getBlockHead(ftemp);
	Index* index = NULL;
	while(btemp)
	{
		index = (Index*)bufferManager->getContent(*btemp);
		int size = bufferManager->getUsedSize(*btemp) / sizeof(Index);
		for (int i = 0; i < size; i++, index++)
		{
			if (index->tableName == tableName)
				indexList->push_back(*index);//Single block
		}
		btemp = bufferManager->getNextBlock(ftemp, btemp);
		if (btemp->end)//New next block with no content
			break;
	}
	return false;
}
//Get index type with specified index name, return -2 that not found
int CatalogManager::getIndexType(string indexName)
{
	File *ftemp = bufferManager->getFile("IndicesInfo");
	Block* btemp = bufferManager->getBlockHead(ftemp);
	Index* index = NULL;
	while (btemp)
	{
		index = (Index*)bufferManager->getContent(*btemp);
		int size = (bufferManager->getUsedSize(*btemp)) / sizeof(Index);
		for (int i = 0; i < size; i++, index++)
			if (index->indexName == indexName)
				return index->type;
		btemp = bufferManager->getNextBlock(ftemp, btemp);
		if (btemp->end)
			break;
	}
	return -2;//Not found
}
//Drop index info from IndiceInfo
bool CatalogManager::dropIndex(string indexName)
{
	File *ftemp = bufferManager->getFile("IndicesInfo");
	Block* btemp = bufferManager->getBlockHead(ftemp);
	while (btemp)
	{
		char* addr = bufferManager->getContent(*btemp);
		Index* index = (Index*)addr;
		int count = (bufferManager->getUsedSize(*btemp))
			/ sizeof(Index);
		int i = 0;
		for (; i < count; i++, index++)
		{
			if (index->indexName == indexName)
			{
				this->setIndexOnAttribute(index->tableName, index->attriName, "");//Revoke
				for (; i < count; i++, index++)
				{
					*index = *(index + 1);//Move forward
				}
				bufferManager->setUsedSize(*btemp, bufferManager->getUsedSize(*btemp) - sizeof(Index));
				bufferManager->setDirty(*btemp, true);
				return true;
			}
		}
		btemp = bufferManager->getNextBlock(ftemp, btemp);
		if (btemp->end)
			break;
	}
	return false;
}
//Get all indices across all table files
bool CatalogManager::getAllIndice(vector<Index>* indexList)
{
	File *ftemp = bufferManager->getFile("IndicesInfo");
	Block* btemp = bufferManager->getBlockHead(ftemp);
	Index* index = NULL;
	while (btemp)
	{
		index = (Index*)bufferManager->getContent(*btemp);
		int size = (bufferManager->getUsedSize(*btemp)) / sizeof(Index);
		for (int i = 0; i < size; i++, index++)
			indexList->push_back(*index);
		btemp = bufferManager->getNextBlock(ftemp, btemp);
		if (btemp->end)//New next block with no content
			break;
	}
	return false;
}
