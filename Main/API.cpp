
#include "stdafx.h"
#include "API.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"

API::API()
{
}

bool API::createTable(string tableName, vector<Attribute>* attriList, string pKeyName, int pKeyPos)
{
	CatalogManager* catalogManager = new CatalogManager();
	RecordManager* recordManager = new RecordManager();
	bool flag = catalogManager->findTable(tableName);
	if (flag == true)
	{
		delete catalogManager;
		delete recordManager;
		return false;
	}
		
	flag=catalogManager->addTable(tableName, attriList, pKeyName, pKeyPos);
	if (flag == false)
	{
		delete catalogManager;
		delete recordManager;
		return false;
	}
	flag=recordManager->createTable(tableName);
	delete catalogManager;
	delete recordManager;
	return flag;
}

bool API::createIndex(string indexName, string tableName, string attriName)
{
	CatalogManager* catalogManager = new CatalogManager();
	RecordManager* recordManager = new RecordManager();
	IndexManager* indexManager = new IndexManager();
	vector<Attribute>* attriList;
	catalogManager->getAttribute(tableName, attriList);
	int i,type;
	for (i = 0; i < attriList->size; i++)
	{
		if ((*attriList)[i].name == attriName)
		{
			type = (*attriList)[i].type;
			break;
		}
	}
	bool flag = catalogManager->findIndex(indexName);
	if (flag == true)
	{
		delete catalogManager;
		delete recordManager;
		return false;
	}

	flag = catalogManager->addIndex(indexName, tableName, attriName, type);
	if (flag == false)
	{
		delete catalogManager;
		delete recordManager;
		return false;
	}
	flag = recordManager->createIndex(indexName);
	delete catalogManager;
	delete recordManager;
	return flag;
}

bool API::dropTable(string tableName)
{
	CatalogManager* catalogManager = new CatalogManager();
	bool flag=catalogManager->findTable(tableName);
	if (flag == false)
	{
		delete catalogManager;
		return false;
	}
	flag=catalogManager->dropTable(tableName);
	delete catalogManager;
	return flag;
}

bool API::dropIndex(string indexName)
{
	CatalogManager* catalogManager = new CatalogManager();
	bool flag = catalogManager->findIndex(indexName);
	if (flag == false)
	{
		delete catalogManager;
		return false;
	}
	flag = catalogManager->dropIndex(indexName);
	delete catalogManager;
	return flag;
}

bool API::select(string tableName, vector<Condition>* conditionList)
{
	int j,offset=-2;
	Index* index;
	vector<Index>* indexList;
	vector<Attribute>* attriAll;
	CatalogManager* catologManager = new CatalogManager();
	IndexManager* indexManager = new IndexManager();
	RecordManager* recordManager = new RecordManager();
	bool flag=catologManager->getIndex(indexList, tableName);
	int pos = (catologManager->getAttribute(tableName, attriAll));
	if ((flag == false)||(pos==-1))
	{
		delete catologManager;
		delete indexManager;
		delete recordManager;
		return false;
	}
	string pName = (*attriAll)[pos].name;
	for (int i = 0; i < (*conditionList).size; i++)
	{
		if ((*conditionList)[i].getOperateType==2)
		{
			for (j = 0; j < (*indexList).size; j++)
			{
				index = &(*indexList)[j];
				if (index->attriName == (*conditionList)[i].getAttribute())
				{
					offset=indexManager->searchIndex(index->indexName, (*conditionList)[i].getComparedValue, index->type);
					if ((offset == -1) && (index->attriName == pName))
						return false;
					else if (offset == -1)
						break;
					Block* block = recordManager->findBlock(tableName, offset);
					flag = recordManager->showRecordInBlock(tableName, attriList, conditionList, block); 
					return flag;
				}
			}
		}
	}
	return recordManager->showRecord(tableName, attriAll, conditionList);
}

bool API::insert(string tableName, string * record)
{
	CatalogManager* catalogManager = new CatalogManager();
	RecordManager* recordManager = new RecordManager();
	vector<Attribute>* attriList;
	int i,j,pos=catalogManager->getAttribute(tableName, attriList);
	int typesize,size=0;
	char * recordContent = (char *)malloc(catalogManager->getRecordSize);
	char *head = recordContent;
	for (i = 0; i < attriList->size; i++)
	{
		typesize = catalogManager->getTypeSize((*attriList)[i].type);
		memcpy(recordContent, &((*record)[i]), typesize);//有bug,record[i]可能比typesize小
		size += typesize;
		recordContent += typesize;
	}
	int offset=recordManager->insertRecord(tableName, head, size);
	if (offset == -1)
	{
		delete catalogManager;
		delete recordManager;
		return false;
	}
	IndexManager* indexmanager = new IndexManager();
	vector<Index>* indexList;
	Index* index;
	catalogManager->getIndex(indexList, tableName);
	for (i = 0; i < indexList->size; i++)
	{
		index = &(*indexList)[i];
		for (j = 0; j < attriList->size; j++)
		{
			if ((*attriList)[j].name == index->attriName)
			{
				indexmanager->insertKeyIntoIndex(index->indexName, record[j], (*attriList)[j].type, offset);
				break;
			}
		}
	}
	delete catalogManager;
	delete recordManager;
	delete indexmanager;
	return true;
}

bool API::deleteFromTable(string tableName, vector<Condition> *conditionList)
{
	IndexManager* indexManager = new IndexManager();
	RecordManager* recordManager = new RecordManager();
	CatalogManager* catalogManager = new CatalogManager();
	vector<Index>* indexList;
	vector<Attribute>* attriList;
	Index* index;
	catalogManager->getIndex(indexList, tableName);
	catalogManager->getAttribute(tableName, attriList);
	int i, j;
	//for (i = 0; i < indexList->size; i++)
	//{
	//	index = &(*indexList)[i];
	//	for (j = 0; j < attriList->size; j++)
	//	{
	//		if ((*attriList)[j].name == index->attriName)
	//		{
	//			indexManager->deleteKeyFromIndex(index->indexName,)
	//			break;
	//		}
	//	}
	//}
	recordManager->deleteRecord(tableName, attriList, conditionList);
}


API::~API()
{
}
