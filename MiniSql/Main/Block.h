#pragma once
#include <string>
#include "BufferManager.h"
using namespace std;

#define MAX_BLOCK_NUM 300
#define MAX_BLOCK_SIZE 8092 //8KB

class Block
{
public:
	int offset; //Offset position in a block
	bool pin;
	bool end; //End of file
	string fileName; //block is whom?
	friend class BufferManager;

private:
	char* address; //Content address w/o head, C++ doesn't have byte type
	Block* pre;
	Block* next;
	bool dirty;//need to write back
	bool lru;//LRU flag
	size_t usedSize;
};