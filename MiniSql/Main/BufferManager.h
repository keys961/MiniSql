#pragma once
#include "File.h"

static int replacedBlock = -1;

class BufferManager
{
public:	
	BufferManager();
	~BufferManager();

	File* getFile(const string fileName, bool pin = false);
	Block* getBlock(File* file, Block* block, bool pin = false);
	Block* getBlockHead(File* file);
	Block* getNextBlock(File* file, Block* block);
	Block* getBlockByOffset(File* file, int offset);
	void deleteFile(const string fileName);
	void setDirty(Block& block, bool dirty);
	void setPin(File& file, bool pin);
	void setPin(Block& block, bool pin);
	void setUsedSize(Block& block, size_t usage);
	size_t getUsedSize(Block& block);
	char* getContent(Block& block);
	int getBlockSize()//get size w/o head
	{
		return MAX_BLOCK_SIZE - sizeof(size_t);
	}

private:
	File* filehead; //Head ptr to File
	File filePool[MAX_FILE_NUM];//Slots of file, can be tables or indices
	Block blockPool[MAX_BLOCK_NUM];//Slots of block
	int fileNum;
	int blockNum;

	void initFile(File& file);
	void initBlock(Block& block);
	void writeToDisk(string fileName, Block* block);
	void writeAllToDisk();
};

