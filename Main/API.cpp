
#include "stdafx.h"
#include "API.h"

API::API()
{
	buffer = new BufferManager();
	this->recordManager = new RecordManager(buffer);
	this->catalogManager = new CatalogManager(buffer);
	vector<Index> indexAll;
	catalogManager->getIndex(&indexAll);
	this->indexManager = new IndexManager(indexAll);
}

bool API::createTable(string tableName, vector<Attribute>* attriList, string pKeyName, int pKeyPos)
{

	bool flag = catalogManager->findTable(tableName);
	if (flag == true)
	{
		return false;
	}

	flag = catalogManager->addTable(tableName, attriList, pKeyName, pKeyPos);
	//flag = flag&&recordManager->createTable(tableName);
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
		return false;
	flag = flag&&catalogManager->addIndex(indexName, tableName, attriName, type);
	indexManager->createIndex(indexName, type);
	return flag;
}

bool API::dropTable(string tableName)
{
	bool flag = catalogManager->findTable(tableName);
	if (flag == false)
	{
		return false;
	}
	flag = dropIndex(tableName + "_index");
	flag = catalogManager->dropTable(tableName);
	return flag;
}

bool API::dropIndex(string indexName)
{
	bool flag = catalogManager->findIndex(indexName);
	if (flag == false)
	{
		return false;
	}
	int type = catalogManager->getIndexType(indexName);
	catalogManager->dropIndex(indexName);
	indexManager->dropIndex(indexName, type);
	recordManager->dropIndex(indexName);

	return flag;
}

bool API::select(string tableName, vector<Condition>* conditionList)
{
	int j, offset = -2;
	string indexName;
	vector<Index> indexList;
	vector<Attribute> attriAll;
	//bool flag=api->getIndex(&indexList, tableName);
	int pos = (catalogManager->getAttribute(tableName, &attriAll));
	if (/*(flag == false)||*/(pos == -1))
	{
		return false;
	}
	string pName = attriAll[pos].name;
	for (int i = 0; i < (*conditionList).size(); i++)
	{
		if ((*conditionList)[i].getOperateType() == 2)
		{
			for (j = 0; j < attriAll.size(); j++)
			{
				indexName = attriAll[j].indexName;
				//if (index->attriName == (*conditionList)[i].getAttribute())
				if (indexName != "")
				{
					offset = indexManager->searchIndex(indexName, (*conditionList)[i].getComparedValue(), attriAll[j].type);
					if ((offset == -1) && (attriAll[j].name == pName))
						return false;
					else if (offset == -1)
						break;
					Block* block = recordManager->findBlock(tableName, offset);
					bool flag = recordManager->showRecordInBlock(tableName, &attriAll, conditionList, block);
					return flag;
				}
			}
		}
	}
	return recordManager->showRecord(tableName, &attriAll, conditionList);
}

bool API::insert(string tableName, vector<string>record)//!--todo-
{
	vector<Attribute> attriList;
	int i, j,pp, pos = catalogManager->getAttribute(tableName, &attriList);
	int typesize,inttemp, size = 0;
	float floattemp;
	string st = "";
	char sss[256];
	char * recordContent = (char *)malloc(catalogManager->getRecordSize(tableName));
	char *head = recordContent;
	for (i = 0; i < attriList.size(); i++)
	{
		typesize = catalogManager->getTypeSize(attriList[i].type);
		if (attriList[i].type > 0)
		{
			pp = attriList[i].type - record[i].length();
			if (pp < 0)
				return false;
			//sss = (char*)malloc(typesize);
			memset(sss, 0, 256);
			/*for (pp = 0; pp < record[i].length(); pp++)
			{
				sss[pp] = record[i][pp];
			}
			while (pp < typesize)
			{
				sss[pp++] = '0';
			}*/
			memcpy(sss, record[i].c_str(), typesize);
			memcpy(recordContent,sss, typesize);
		}
		else
		if (attriList[i].type==0)
		{
			sscanf(record[i].data(), "%d", &inttemp);
			memcpy(recordContent, &inttemp, typesize);
		}
		else
		if (attriList[i].type == -1)
		{
			sscanf(record[i].data(), "%f", &floattemp);
			memcpy(recordContent, &floattemp, typesize);
		}
		size += typesize;
		recordContent += typesize;
	}
	int offset = recordManager->insertRecord(tableName, head, size);
	if (offset == -1)
	{
		return false;
	}
	vector<Index> indexList;
	Index* index;
	catalogManager->getIndex(&indexList, tableName);
	for (i = 0; i < indexList.size(); i++)
	{
		index = &(indexList[i]);
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
	vector<Index> indexList;
	vector<Attribute> attriList;
	Index* index;
	catalogManager->getIndex(&indexList, tableName);
	catalogManager->getAttribute(tableName, &attriList);
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
	return recordManager->deleteRecord(tableName, &attriList, conditionList);
}
//void API::getIndex(vector<Index>* indexList, string& tableName)
//{
//	vector<Attribute> attriList;
//	Index * index;
//	catalogManager->getAttribute(tableName, &attriList);
//	for (int i = 0; i < attriList.size(); i++)
//	{
//		if (attriList[i].indexName != "")
//		{
//			Index index(attriList[i].indexName, tableName, attriList[i].name, attriList[i].type);
//			indexList->push_back(index);
//		}
//	}
//}

API::~API()
{
	delete catalogManager;
	delete indexManager;
	delete recordManager;
	delete buffer;
}
