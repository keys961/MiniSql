// Main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "BufferManager.h"
#include <string>
using namespace std;

void bufferTest();

int main()
{
	//string a = "111111111111111111111111111111";
	/*
	int a = 1;
	int* b = &a;
	printf("%d\n", b);
	printf("%d\n", b + 1);
	printf("%d\n", b + sizeof(int));*/
	char* str = "123444";
	int a = 2048;
	char buffer[250];
	//FILE* fp = fopen("123.txt", "r");
	//fseek(fp, 0, 0);
	//fwrite(str, 1, 7, fp);
	//int a = fread(buffer, sizeof(char), 20, fp);
	memcpy(buffer, (size_t*)&a, sizeof(int));
	printf("%d", *(int*)buffer);
	system("pause");
    return 0;
}

void bufferTest()
{
	string fileName = "test";
	BufferManager* bufferManager = new BufferManager();//Ctor init block&file
	File* file = bufferManager->getFile(fileName);//get file
	Block* head = bufferManager->getBlockHead(file);//fill head block
	Block* next = bufferManager->getNextBlock(file, head);//fill next block
	
}
