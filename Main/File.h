#ifndef FILE_H
#define FILE_H


#include <string>
//#include "BufferManager.h"
using namespace std;

#define MAX_FILE_NUM 40
#define MAX_BLOCK_NUM 300
#define MAX_BLOCK_SIZE 4096 //4KB

class Block
{
public:
	int offset; //Offset position in a block, file relative
	bool pin;
	bool end; //End of file, actually no use
	string fileName; //block is whom?
	friend class BufferManager;

private:
	char* address; //Content address w/o head, C++ doesn't have byte type
				   //The first 4 bit is used to store usedSize without head
	Block* pre;
	Block* next;
	bool dirty;//need to write back
	size_t usedSize;// the byte size that the block have used. The total size of the block is MAX_BLOCK_SIZE
};

class File
{
public:
	string fileName;
	bool pin; //Pin
	Block* head; //Block head
	//File* pre; //Previous file node
	//File* next; //Next file node
};

#endif // !FILE_H