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
	/*FILE *fp = fopen("TestFile2", "wb+");
	char data[4096];
	data[4095] = 0;
	for (int i = 1; i <= 300; i++)
	{
		fwrite(data, sizeof(char), 4096, fp);
		fseek(fp, 4096 * i, 0);
	}
	fclose(fp);*/
	//fp = fopen("TestFile", "rb");
	//size_t a = fread(data, 1, 5000, fp);
	//bufferTest();
	catalogTest();
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
	//bufferManager->setDirty(*next, true);//OK
	
}

void catalogTest()
{
	CatalogManager* catalogManager = new CatalogManager();
	Attribute attri[20];
	for (int i = 0; i < 20; i++)
	{
		attri[i] = Attribute("name", 10, false);
		attri[i].printAttribute();
	}
	string tableName = "testTable";
	
}