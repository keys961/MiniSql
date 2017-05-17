// Main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CatalogManager.h"
#include <string>
using namespace std;

void bufferTest();
void catalogTest();

int main()
{
	//Buffer test
	/*FILE *fp = fopen("TestFile2", "wb+");
	char data[4096];
	char another[2048];
	int* addr = (int*)data;
	*addr = 4096 - 4;
	addr = (int*)another;
	*addr = 2048 - 4;
	for (int i = 1; i <= 300; i++)
	{
		fwrite(data, sizeof(char), 4096, fp);
		fseek(fp, 4096 * i, 0);
	}
	fwrite(another, sizeof(char), 2048, fp);
	fclose(fp);*/
	//bufferTest();

	//Catalog test
	//catalogTest();
    return 0;
}

void bufferTest()
{
	string fileName = "TestFile2";
	BufferManager* bufferManager = new BufferManager();//Ctor init block&file
	File* file = bufferManager->getFile(fileName);//get file
	Block* head = bufferManager->getBlockHead(file);//fill head block
	Block* next = head;
	do
	{
		 next = bufferManager->getNextBlock(file, next);//fill next block OK

	} while (next->offset < 300);
	system("pause");//OK big data
	//bufferManager->setDirty(*head, true);//OK
	bufferManager->setDirty(*next, true);//OK
	bufferManager->deleteFile(fileName);
}

void catalogTest()
{
	CatalogManager* catalogManager = new CatalogManager();
	vector<Attribute> attriList;
	vector<Attribute> newList;
	vector<Index> indexList;
	for (int i = 0; i < 20; i++)
	{
		attriList.push_back(*(new Attribute("name", 20, false)));
		attriList[i].printAttribute();
	}
	string tableName = "testTable";
	//catalogManager->addTable(tableName, &attriList, "name", 1);
	bool isFind;
	int recordNum = catalogManager->getRecordNum(tableName);
	int singleSize = catalogManager->getRecordSize(tableName);
	int pKeyPos = catalogManager->getAttribute(tableName, &newList);

	//catalogManager->addIndex("tname", tableName, "name", 20);
	catalogManager->dropIndex("tname");
	bool indexFind = catalogManager->findIndex("tname");
	catalogManager->getIndex(&indexList, tableName);
	int indexType = catalogManager->getIndexType("tname");

	catalogManager->dropTable(tableName);
	isFind = catalogManager->findTable(tableName);
	delete catalogManager;
	system("pause");
}