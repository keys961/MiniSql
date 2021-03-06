#pragma once
// Main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"
#include "Interpreter.h"
#include "API.h"
#include <string>
using namespace std;

void bufferTest();
void catalogTest();
void recordTest();
int  interpreterTest();


vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}
Interpreter * interpreter = new Interpreter();
int main(int argc,char *argv[])
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
	fclose(fp);
	bufferTest();*/

	//Catalog test
	/*catalogTest();*/

	//recordTest();
	//IndexManager m;
	int c;
	if (argc == 3)
	{
		freopen(argv[1], "r", stdin);
		freopen(argv[2], "w", stdout);
	}
	
	while (c=interpreterTest())
	{
		if (c == -1)
			break;
	}
	delete interpreter;
	if (argc == 3)
	{
		fclose(stdin);
		fclose(stdout);
	}
	return 0;
}
int interpreterTest()
{
	char s;
	string ss("");
	s = getchar();
	while (s != ';')
	{
		ss = ss + s;
		s = getchar();
	}
	int flag=interpreter->Parse(ss);
	return flag;
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
	} while (next->offset <= 300);
	system("pause");//OK big data
	//bufferManager->setDirty(*head, true);//OK
	bufferManager->setDirty(*next, true);//OK
	bufferManager->deleteFile(fileName);
}

void catalogTest()
{
	CatalogManager* catalogManager = new CatalogManager(new BufferManager());
	vector<Attribute> attriList;
	vector<Attribute> newList;
	vector<Index> indexList;
	for (int i = 0; i < 20; i++)
	{
		attriList.push_back(*(new Attribute("name", 20, false)));
		attriList[i].printAttribute();
	}
	attriList.push_back(*(new Attribute("nasdsadme", 20, false)));
	attriList.push_back(*(new Attribute("nasdsdsdsdadme", 20, false)));
	string tableName = "testTable";
	catalogManager->addTable(tableName, &attriList, "name", 1);
	bool isFind;
	int recordNum = catalogManager->getRecordNum(tableName);
	int singleSize = catalogManager->getRecordSize(tableName);
	int pKeyPos = catalogManager->getAttribute(tableName, &newList);

	//catalogManager->addIndex("tname", tableName, "name", 20);
	//catalogManager->addIndex("tname2", tableName, "nasdsadme", 20);
	//catalogManager->dropIndex("tname");
	bool indexFind = catalogManager->findIndex("tname");
	catalogManager->getIndex(&indexList, tableName);
	int indexType = catalogManager->getIndexType("tname");
	indexType = catalogManager->getIndexType("tname2");

	//catalogManager->dropTable(tableName);
	//isFind = catalogManager->findTable(tableName);
	delete catalogManager;
	system("pause");
}

void recordTest()
{
	BufferManager* buffer = new BufferManager();
	RecordManager* record = new RecordManager(buffer);
	CatalogManager* catalog = new CatalogManager(buffer);

	vector<Attribute> attriList;
	attriList.push_back(*(new Attribute("id", 0, false)));
	attriList.push_back(*(new Attribute("name", 10, false)));

	vector<Condition> conditionList;
	conditionList.push_back(*(new Condition("id", "1", Condition::EQUAL)));
	conditionList.push_back(*(new Condition("name", "Jack", Condition::EQUAL)));
	string tableName = "student";

	catalog->addTable(tableName, &attriList, "id", 1);

	char* recordContent = new char[14];
	char* temp;
	//First Record
	memset(recordContent, 0, 14);
	temp = recordContent;
	*(int*)temp = 1;
	temp += sizeof(int);
	strcpy(temp, "Jack");
	for (int i = 0; i < 88000; i++)
		record->insertRecord(tableName, recordContent, 14);
	record->deleteRecord(tableName, &attriList, &conditionList);
	int n = record->findRecord(tableName, &attriList, &conditionList);
	record->showRecord(tableName, &attriList, &conditionList);
	printf("%d", catalog->getRecordSize("student"));
	printf("%d record found\n", n);
	system("pause");
}
