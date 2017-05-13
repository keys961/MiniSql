#include "stdafx.h"
#include "CatalogManager.h"


CatalogManager::CatalogManager()
{
}


CatalogManager::~CatalogManager()
{
}
//Add a new table with attrs to the memory block, create table with table info
bool CatalogManager::addTable(string tableName, vector<Attribute>* attriList, string pKeyName, int pKeyPos)
{
	FILE* fp = fopen(tableName.c_str(), "w+");
	if (!fp)
		return false;
	fclose(fp);
	File* ftemp = bufferManager.getFile(tableName);
	Block* btemp = bufferManager.getBlockHead(ftemp);
	if (btemp)
	{
		char* addrBegin = bufferManager.getContent(*btemp);
		int* size = (int*)addrBegin;
		*size = 0;//First note number of record
		addrBegin += sizeof(int);
		*addrBegin = pKeyPos; //Second note pKey
		addrBegin++;//1 byte
		*addrBegin = (*attriList).size();//Third not attribute number
		addrBegin++;//1 byte
		for (int i = 0; i < (*attriList).size(); i++)
		{
			memcpy(addrBegin, &(*attriList)[i], sizeof(Attribute));
			addrBegin += sizeof(Attribute);
		}
		bufferManager.setUsedSize(*btemp, bufferManager.getUsedSize(*btemp)
			+ (*attriList).size() * sizeof(Attribute) + sizeof(int) + 2);
		bufferManager.setDirty(*btemp, true);//Set dirty
	}
	return false;
}

bool CatalogManager::findTable(string tableName)
{
	return false;
}

bool CatalogManager::dropTable(string tableName)
{
	return false;
}
