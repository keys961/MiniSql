
#include "stdafx.h"
#include "API.h"

API::API()
{
}

bool API::createTable(string tableName, vector<Attribute>* attriList, string pKeyName, int pKeyPos)
{

	bool flag = catalogManager->findTable(tableName);
	if (flag == true)
	{
		return false;
	}
		
	flag = catalogManager->addTable(tableName, attriList, pKeyName, pKeyPos);
	flag = flag&&recordManager->createTable(tableName);
	flag = flag&&createIndex(tableName + "_index", tableName, (*attriList)[pKeyPos].name);
	return flag;
}

bool API::createIndex(string indexName, string tableName, string attriName)
{
	vector<Attribute> attriList;
	bool flag = true;
	int i, type;
	if (catalogManager->findIndex(indexName) == true)
	{
		return false;
	}
	catalogManager->getAttribute(tableName, &attriList);
	for (i = 0; i < attriList.size(); i++)
	{
		if (attriList[i].name == attriName)
		{
			type = attriList[i].type;
			break;
		}
	}
	if (i == attriList.size())
		flag = false;
	flag = flag&&catalogManager->addIndex(indexName, tableName, attriName, type);
	flag = flag&&recordManager->createIndex(indexName);
	return flag;
}

bool API::dropTable(string tableName)
{
	bool flag=catalogManager->findTable(tableName);
	if (flag == false)
	{
		return false;
	}
	flag = dropIndex(tableName + "_index");
	flag = flag&&catalogManager->dropTable(tableName);
	return flag;
}

bool API::dropIndex(string indexName)
{
	bool flag = catalogManager->findIndex(indexName);
	if (flag == false)
	{
		return false;
	}
	flag = catalogManager->dropIndex(indexName);
	return flag;
}

bool API::select(string tableName, vector<Condition>* conditionList)
{
	int j,offset=-2;
	Index* index;
	vector<Index> indexList;
	vector<Attribute> attriAll;
	bool flag=catalogManager->getIndex(&indexList, tableName);
	int pos = (catalogManager->getAttribute(tableName, &attriAll));
	if ((flag == false)||(pos==-1))
	{
		return false;
	}
	string pName = attriAll[pos].name;
	for (int i = 0; i < (*conditionList).size(); i++)
	{
		if ((*conditionList)[i].getOperateType()==2)
		{
			for (j = 0; j < indexList.size(); j++)
			{
				index = &(indexList[j]);
				if (index->attriName == (*conditionList)[i].getAttribute())
				{
					offset=indexManager->searchIndex(index->indexName, (*conditionList)[i].getComparedValue(), index->type);
					if ((offset == -1) && (index->attriName == pName))
						return false;
					else if (offset == -1)
						break;
					Block* block = recordManager->findBlock(tableName, offset);
					flag = recordManager->showRecordInBlock(tableName, &attriAll, conditionList, block); 
					return flag;
				}
			}
		}
	}
	return recordManager->showRecord(tableName, &attriAll, conditionList);
}

bool API::insert(string tableName, string * record)
{
	vector<Attribute> attriList;
	int i,j,pos=catalogManager->getAttribute(tableName, &attriList);
	int typesize,size=0;
	char * recordContent = (char *)malloc(catalogManager->getRecordSize(tableName));
	char *head = recordContent;
	for (i = 0; i < attriList.size(); i++)
	{
		typesize = catalogManager->getTypeSize(attriList[i].type);
		memcpy(recordContent, &((*record)[i]), typesize);//有bug,record[i]可能比typesize小
		size += typesize;
		recordContent += typesize;
	}
	int offset=recordManager->insertRecord(tableName, head, size);
	if (offset == -1)
	{
		return false;
	}
	vector<Index>* indexList;
	Index* index;
	catalogManager->getIndex(indexList, tableName);
	for (i = 0; i < indexList->size(); i++)
	{
		index = &(*indexList)[i];
		for (j = 0; j < attriList.size(); j++)
		{
			if (attriList[j].name == index->attriName)
			{
				indexManager->insertKeyIntoIndex(index->indexName, record[j], attriList[j].type, offset);
				break;
			}
		}
	}
	return true;
}

bool API::deleteFromTable(string tableName, vector<Condition> *conditionList)
{
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
	return recordManager->deleteRecord(tableName, attriList, conditionList);
}


API::~API()
{
	delete catalogManager;
	delete indexManager;
	delete recordManager;
}
