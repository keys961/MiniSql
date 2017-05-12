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
		initBlock(blockPool[i]);
	}
}


BufferManager::~BufferManager()
{
	
}

//Get and add file if required
File * BufferManager::getFile(const string fileName, bool pin)
{
	Block* btemp = NULL;
	File* ftemp = NULL;
	if (filehead)
	{
		ftemp = filehead;
		for (; ftemp; ftemp = ftemp->next)
		{
			if (fileName == ftemp->fileName)
			{
				ftemp->pin = pin;
				return ftemp;
			}
		}
	}
	else
	{
		if (this->fileNum == 0)
		{
			ftemp = filePool;
			this->fileNum++;
			filehead = ftemp;
		}
		else if (this->fileNum < MAX_FILE_NUM)
		{
			ftemp = &filePool[this->fileNum];
			filePool[this->fileNum - 1].next = ftemp;
			ftemp->pre = &filePool[this->fileNum - 1];
			this->fileNum++;
		}
		else
		{
			ftemp = filehead;
			for (; ftemp; ftemp = ftemp->next)
			{
				if (!ftemp->pin)
					break;
			}
			if (ftemp == nullptr)
			{
				std::cerr << "No More File Node in the pool" << std::endl;
				exit(2);
			}
			for (btemp = ftemp->head; btemp; btemp = btemp->next)
			{
				if (btemp->pre)
				{
					initBlock(*(btemp->pre));
					blockNum--;
				}
				writeToDisk(btemp->fileName, btemp);
			}
			initFile(*ftemp);
		}
		ftemp->fileName = fileName;
		ftemp->pin = pin;
		return ftemp;
	}
}

//Get and replace block if required
Block * BufferManager::getBlock(File * file, Block * block, bool pin)
{
	string filename = file->fileName;
	Block* temp = NULL;
	if (blockNum == 0)
	{
		temp = blockPool;
		blockNum++;
	}
	else if (blockNum < MAX_BLOCK_NUM)
	{
		for (int i = 0; i < MAX_BLOCK_NUM; i++)
		{
			if (blockPool[i].offset == -1)
			{
				temp = &blockPool[i];//Empty block
				blockNum++;
				break;
			}
		}//Else the last block
	}
	else//No empty block, use lru
	{
		for (int i = replacedBlock + 1; ; i++)
		{
			if (i == blockNum)
				i = 0;
			if (!blockPool[i].pin)
			{
				if (blockPool[i].lru == true)
					blockPool[i].lru = false;
				else
				{
					temp = &blockPool[i];
					if (temp->pre)
						temp->pre->next = temp->next;
					if (temp->next)
						temp->next->pre = temp->pre;
					if (temp == file->head)//Update head
						file->head = temp->next;
					replacedBlock = i;
					initBlock(*temp);
					break;
				}
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
	else
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
	fseek(fp, temp->offset * MAX_BLOCK_SIZE, 0);
	if (fread(temp->address, 1, MAX_BLOCK_SIZE, fp) == 0)//Read EOF
		temp->end = true;
	temp->usedSize = *(size_t*)temp->address;
	return temp;
}

Block * BufferManager::getBlockHead(File * file)
{
	Block* temp = nullptr;
	if (file->head)
	{
		if (file->head->offset == 0)
			temp = file->head;
		else
			temp = getBlock(file, NULL);
	}
	else
		temp = getBlock(file, NULL);
	return temp;
}

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
		if (block->offset + 1 == block->next->offset)
			return block->next;
		else
			return getBlock(file, block);
	}
}

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

void BufferManager::deleteFile(const string fileName)//Delete file node & all related blocks
{
	File* ftemp = getFile(fileName);
	Block* btemp = getBlockHead(ftemp);
	list<Block*> blockList;
	if (!btemp)
		return;
	while (true)
	{
		blockList.push_back(btemp);
		if (btemp->end)
			break;
		btemp = getNextBlock(ftemp, btemp);
	}
	blockNum -= blockList.size();
	while (!blockList.empty())
	{
		initBlock(*blockList.back());
		blockList.pop_back();
	}
	if (ftemp->pre)
		ftemp->pre->next = ftemp->next;
	if (ftemp->next)
		ftemp->next->pre = ftemp->pre;
	if (ftemp == filehead)
		filehead = ftemp->next;
	initFile(*ftemp); //Clear file ptr
	fileNum -= 1;
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

void BufferManager::setPin(Block & block, bool pin)
{
	block.pin = pin;
}

void BufferManager::setUsedSize(Block & block, size_t usage)
{
	block.usedSize = usage;
	memcpy(block.address, (char*)&usage, sizeof(size_t));
}

size_t BufferManager::getUsedSize(Block & block)
{
	return block.usedSize;
}

char * BufferManager::getContent(Block & block)
{
	return block.address + sizeof(size_t);//add header
}

void BufferManager::initFile(File & file)
{
	file.next = file.pre = false;
	file.head = NULL;
	file.pin = false;
}

void BufferManager::initBlock(Block & block)
{
	memset(block.address, 0, MAX_BLOCK_SIZE);
	size_t initUsage = 0;
	memcpy(block.address, (char*)&initUsage, sizeof(size_t));//Set head
	block.usedSize = sizeof(size_t);
	block.dirty = block.pin = block.lru = block.end = false;
	block.offset = -1;
	block.pre = block.next = NULL;
}

void BufferManager::writeToDisk(string fileName, Block * block)//Flush dirty block to the list
{
	if (block->dirty == false)
		return;
	FILE* fp = fopen(fileName.c_str(), "rb+");
	fseek(fp, block->offset * MAX_BLOCK_SIZE, 0);
	fwrite(block->address, 1, block->usedSize + sizeof(size_t), fp);
	fclose(fp);
}

void BufferManager::writeAllToDisk()//Flush all to disk
{
	Block* btemp = nullptr;
	File* ftemp = nullptr;
	if (filehead)
	{
		for (ftemp = filehead; ftemp; ftemp = ftemp->next)
		{
			if (ftemp->head)
			{
				for (btemp = ftemp->head; btemp; btemp = btemp->next)
				{
					if (btemp->pre)
						initBlock(*(btemp->pre));
					writeToDisk(btemp->fileName, btemp);
				}
			}
		}
	}
}
