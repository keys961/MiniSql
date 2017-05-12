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
	//The first 4 bit is used to store usedSize, actually, 
	//there's nothing in first 4 bit, the size is stored in size_t usedSize
	Block* pre;
	Block* next;
	bool dirty;//need to write back
	bool lru;//LRU flag
	size_t usedSize;// the byte size that the block have used. The total size of the block is MAX_BLOCK_SIZE
};