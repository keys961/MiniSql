#pragma once
#include <string>
#include "Block.h"
using namespace std;

#define MAX_FILE_NUM 40
#define MAX_FILE_NAME 100

class File
{
public:
	string fileName;
	bool pin; //Pin
	Block* head; //Block head
	File* pre; //Previous file
	File* next; //Next file
};