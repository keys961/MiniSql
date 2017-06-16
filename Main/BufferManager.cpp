#include "stdafx.h"
#include "BufferManager.h"
#include <cstdio>
#include <list>
#include <iostream>

using namespace std;

BufferManager::BufferManager()
{
	fileNum = blockNum = 0;
	filehead = NULL;
	for (int i = 0; i < MAX_FILE_NUM; i++)
		initFile(filePool[i]);
	for (int i = 0; i < MAX_BLOCK_NUM; i++)
	{
		blockPool[i].address = new char[MAX_BLOCK_SIZE];
		//frequency[i] = 0;
		initBlock(blockPool[i]);
	}
}


BufferManager::~BufferManager()
{
	writeAllToDisk();
	for (int i = 0; i < MAX_BLOCK_NUM; i++)
		delete[] blockPool[i].address;
}

//Get and add file if required (not find file in para list)
//If you want to load File to the block
//First call getFile, then call getBlock(file, NULL) to add 1 block of the file
File * BufferManager::getFile(const string fileName, bool pin)
{
	Block* btemp = NULL;
	File* ftemp = NULL;
	if (filehead)//To find existed file node
	{
		ftemp = filePool;
		int i = 0;
		for (; i < MAX_FILE_NUM; ftemp++, i++)
		{
			if (fileName == ftemp->fileName)//Found, directly return
			{
				ftemp->pin = pin;
				return ftemp;
			}
		}
	}
	//Not find the fileNode, add fileNode
	if (this->fileNum == 0)//Get first address, if no file left
	{
		ftemp = filePool;
		this->fileNum++;
		filehead = ftemp;
	}
	else if (this->fileNum < MAX_FILE_NUM)//Remain
	{
		int i = 0;
		for (ftemp = filePool; i < MAX_FILE_NUM; ftemp++, i++)
		{
			if (ftemp->head == NULL)//Get free file node slot
				break;
		}
		//ftemp->pre = &filePool[fileNum - 1];
		//filePool[fileNum - 1].next = ftemp;
		fileNum++;
	}
	else//Need replace
	{
		for (ftemp = filePool; ftemp; ftemp++)
		{
			if (!ftemp->pin)//Not pinned
				break;
		}
		if (ftemp == nullptr)//Exception
		{
			std::cerr << "No More File Node in the pool" << std::endl;
			exit(2);
		}
		Block* lastBlock = NULL;
		for (btemp = ftemp->head; btemp; btemp = btemp->next)
		{
			if (btemp->pre)
			{
				initBlock(*(btemp->pre));
				blockNum--;
			}
			writeToDisk(btemp->fileName, btemp);
			lastBlock = btemp;
		}
		initBlock(*(lastBlock));//Clear last block
		initFile(*ftemp);//Clear file node
	}
	ftemp->fileName = fileName;
	ftemp->pin = pin;
	return ftemp;
}

//Get current block & add next block, replace block if required (not find block in para list)
//Return the new block added (not in the para list)
//Try not to use this function for it may cause mistakes
Block * BufferManager::getBlock(File * file, Block * block, bool pin)
{
	string filename = file->fileName;
	Block* temp = NULL;//Result block
	if (blockNum == 0)//No block exists
	{
		temp = blockPool;
		blockNum++;
	}
	else if (blockNum < MAX_BLOCK_NUM)
	{
		for (int i = 0; i < MAX_BLOCK_NUM; i++)
		{
			if (blockPool[i].offset == -1)//NO USED
			{
				temp = &blockPool[i];//Empty block
				blockNum++;
				break;
			}
		}//Else the last block
	}
	else//No empty block, FIFO
	{
		for (int i = replacedBlock + 1; ; i++)
		{
			if (i == MAX_BLOCK_NUM)
				i = 0;
			if (!blockPool[i].pin)
			{
					temp = &blockPool[i];
					if (temp->pre)//link pre
						temp->pre->next = temp->next;
					if (temp->next)//link next
						temp->next->pre = temp->pre;
					if (temp == file->head)//Update head in file slot
						file->head = temp->next;
					replacedBlock = i;
					writeToDisk(temp->fileName, temp);//Right back to disk
					initBlock(*temp);
					break;
			}
		}
	}
	//add block to the list
	if (block && block->next == nullptr)
	{
		temp->pre = block;
		block->next = temp;
		temp->offset = block->offset + 1;
	}
	else if(block && block->next)
	{
		temp->pre = block;
		temp->next = block->next;
		block->next->pre = temp;
		block->next = temp;
		temp->offset = block->offset + 1;
	}
	else//Head block
	{
		temp->offset = 0;
		if (file->head)
		{
			file->head->pre = temp;
			temp->next = file->head;
		}
		file->head = temp;
	}
	temp->pin = pin;
	temp->fileName = file->fileName;
	FILE* fp = fopen(file->fileName.c_str(), "ab+");
	std::fseek(fp, temp->offset * (MAX_BLOCK_SIZE), 0);//If rest space cannot hold a record, set 0 in the file
	if (fread(temp->address, 1, (MAX_BLOCK_SIZE), fp) == 0)//Read with head
		temp->end = true;//If empty, set it as an end
	temp->usedSize = *(size_t*)(temp->address) + sizeof(size_t);//Get size from block
	std::fclose(fp);
	return temp;
}

//Get head block, load file in the head block
Block * BufferManager::getBlockHead(File * file)
{
	Block* temp = nullptr;
	if (file->head)
	{
		if (file->head->offset == 0)//Head really existed
			temp = file->head;
		else //Reload true file head
			temp = getBlock(file, NULL);
	}
	else
		temp = getBlock(file, NULL);
	return temp;
}
//Get next block (next offset)
//If you want to alloc new block, please check usedSize
Block * BufferManager::getNextBlock(File * file, Block * block)
{
	if (block->next == NULL)
	{
		if (block->end)
			block->end = false;
		return getBlock(file, block);
	}
	else
	{
		if (block->offset + 1 == block->next->offset)//Really exists
			return block->next;
		else //Reload next block
			return getBlock(file, block);
	}
}
//Get block by offset
Block * BufferManager::getBlockByOffset(File * file, int offset)
{
	Block* temp = getBlockHead(file);
	while (offset)
	{
		temp = getNextBlock(file, temp);
		offset--;
	}
	return temp;
}
//Delete file node & all related blocks, 
//Just flush, not write back
void BufferManager::deleteFile(const string fileName)
{ //Just flush, not write back
	File* ftemp = getFile(fileName);
	Block* btemp = getBlockHead(ftemp);
	list<Block*> blockList;
	
	while (btemp)
	{
		blockList.push_back(btemp);
		btemp = btemp->next;
	}
	blockNum -= blockList.size();
	while (!blockList.empty())
	{
		initBlock(*blockList.front());
		blockList.pop_front();
	}
	
	/*if (ftemp->pre)
		ftemp->pre->next = ftemp->next;
	if (ftemp->next)
		ftemp->next->pre = ftemp->pre;*/
	initFile(*ftemp); //Clear file ptr
	fileNum -= 1;
	if (ftemp == filehead)
	{
		int i;
		for(i = 0; i < MAX_FILE_NUM; i++)
			if (filePool[i].head)
			{
				filehead = &filePool[i];
				break;
			}
		if (i == MAX_FILE_NUM)
			filehead = NULL;
	}
	//fclose(fp);
}

void BufferManager::setDirty(Block & block, bool dirty)
{
	block.dirty = dirty;
}

void BufferManager::setPin(File & file, bool pin)
{
	file.pin = pin;
}

void BufferManager::setPin(Block & block, bool pin)//If pin that means not lru
{
	block.pin = pin;
}
//Set used size of block without header
void BufferManager::setUsedSize(Block & block, size_t usage)
{
	block.usedSize = usage + sizeof(size_t);
	memcpy(block.address, (char*)&usage, sizeof(size_t));
}

//Get used size of block without header
size_t BufferManager::getUsedSize(Block & block)
{
	return block.usedSize - sizeof(size_t);
}
//Get all content of block w/o header
char * BufferManager::getContent(Block & block)
{
	//frequency[((&block) - blockPool) / sizeof(Block)]++;
	return block.address + sizeof(size_t);//add header
}
//Init an unused file node
void BufferManager::initFile(File & file)
{
	//file.next = file.pre = NULL;
	file.head = NULL;
	file.pin = false;
	file.fileName = "";
}
//Init an unused block
void BufferManager::initBlock(Block & block)
{
	memset(block.address, 0, MAX_BLOCK_SIZE);
	size_t initUsage = 0;
	memcpy(block.address, (char*)&initUsage, sizeof(size_t));//Set head, the value doesn't contain header
	block.usedSize = sizeof(size_t);//usedSized: with header
	block.dirty = block.pin = block.end = false;
	block.offset = -1;//No use marked
	block.pre = block.next = NULL;
	block.fileName = "";
}
//Flush dirty block to the disk
//If a deletion cause a string of block is NULL value
//Just write it to the list and when read, judge if the value
//is 0, leave it out.
//That is, the size of file is the max number of records that have
//been inserted.
void BufferManager::writeToDisk(string fileName, Block * block)
{
	if (block->dirty == false)
		return;
	FILE* fp = fopen(fileName.c_str(), "rb+");
	std::fseek(fp, block->offset * (MAX_BLOCK_SIZE), 0);
	fwrite(block->address, 1, block->usedSize, fp);//Write with header
	std::fclose(fp);
}
//Flush all to disk
void BufferManager::writeAllToDisk()
{
	Block* btemp = nullptr;
	Block* lastBlock = nullptr;
	File* ftemp = nullptr;
	if (filehead)
	{
		int i = 0;
		for (ftemp = filePool; i < MAX_FILE_NUM; ftemp++, i++)
		{
			if (ftemp->head)
			{
				for (btemp = ftemp->head; btemp; btemp = btemp->next)
				{
					if (btemp->pre)
						initBlock(*(btemp->pre));
					writeToDisk(btemp->fileName, btemp);
					lastBlock = btemp;
				}
				initBlock(*(lastBlock));
			}
		}
	}
}
