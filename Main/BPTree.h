#pragma once
#include <vector>
#include <iostream>
#include "BufferManager.h"

template <typename T>
class TreeNode
{
public:
	size_t count;
	bool isLeaf;
	std::vector<TreeNode*> childs;
	std::vector<T> keys;
	std::vector<int> offsets;
	TreeNode* parent;
	TreeNode* next;

	int degree;

	TreeNode(int degree, bool isLeaf = false);
	virtual ~TreeNode() {}

	bool isRoot()
	{
		return parent == NULL;
	}
	int search(T key);//Return index in NODE
	TreeNode<T>* split(T &key); //Return new node & key goes to upper level
	int add(T &key);//Internal, return the index of key in one node
	int add(T &key, int offset);//Leaf
	bool remove(int index);//Remove element & child/offset by index
};

template<typename T>
class BPTree
{
public:
	typedef TreeNode<T>* Node;
	typedef struct SearchResult
	{
		Node node;
		int index;
		bool isFound;
	}SearchResult;
	string fileName;
private:
	static BufferManager bufferManager;
	Node root, leafHead;
	int keyCount, nodeCount;
	int level;
	File* file;
	int keySize;
	int degree;

	void findLeaf(Node node, T key, SearchResult& res);
	void insertFixUp(Node node);
	void deleteFixUp(Node node);

public:
	BPTree(string fileName, int keySize, int degree);
	~BPTree();
	void init();
	void readBlock(Block* block);
	void read();
	void write();

	int searchKey(T& key); //Search key, return offset
	bool insertKey(T& key, int offset); //Insert key with block offset
	bool deleteKey(T& key);
	void dropTree(Node node);
};

template <typename T>
BufferManager BPTree<T>::bufferManager;

template<typename T>
TreeNode<T>::TreeNode(int degree, bool isLeaf)
{
	this->count = 0;
	this->parent = this->parent = NULL;
	this->isLeaf = isLeaf;
	this->degree = degree;
	for (int i = 0; i <= degree; i++)
	{
		childs.push_back(0);
		keys.push_back(T());
		offsets.push_back(0);
	}
}

template<typename T>
int TreeNode<T>::search(T key)
{	//Linear search
	int index = -1;//If not searched
	for (int i = 0; i < this->count; i++)
	{
		if (this->keys[i] == key)
		{
			index = i;
			break;
		}
		else if (this->keys[i] > key)
			break;
	}
	return index;
}

template<typename T>
TreeNode<T> * TreeNode<T>::split(T & key)
{
	int lCount = (this->degree - 1) / 2;
	TreeNode<T>* newNode = new TreeNode<T>(degree, this->isLeaf);
	if (isLeaf)
	{
		key = keys[lCount + 1];
		for (int i = lCount + 1; i < degree; i++)
		{
			newNode->keys[i - lCount - 1] = keys[i];
			keys[i] = T();
			newNode->offsets[i - lCount - 1] = offsets[i];
			offsets[i] = 0;
		}
		newNode->next = this->next;
		this->next = newNode;
		newNode->parent = this->parent;
		newNode->count = lCount;
		this->count = lCount + 1;
	}
	else
	{
		key = keys[lCount];
		for (int i = lCount + 1; i <= degree; i++)
		{
			newNode->childs[i - lCount - 1] = this->childs[i];
			newNode->childs[i - lCount - 1]->parent = newNode;
			this->childs[i] = NULL;
			if (i != degree)
			{
				newNode->keys[i - lCount - 1] = this->keys[i];
				this->keys[i] = T();
			}
		}
		this->keys[lCount] = T();
		newNode->parent = this->parent;
		newNode->count = this->count = lCount;
	}
	return newNode;
}

template<typename T>
int TreeNode<T>::add(T & key)
{
	if (count == 0)
	{
		this->keys[++count] = key;
		return 0;
	}
	else
	{
		int index = search(key);
		if (index != -1)
			return -1; //Repeated, return, no insertion
		for (int i = 0; i < count; i++)
		{
			if (keys[i] > key)
			{
				index = i;
				break;
			}
		}
		for (int i = count; i > index; i--)
			keys[i] = keys[i - 1];
		keys[index] = key;
		for (int i = count + 1; i > index + 1; i--)
			childs[i] = childs[i - 1];
		childs[index + 1] = NULL;
		++count;
		return index;
	}
}

template<typename T>
int TreeNode<T>::add(T & key, int offset)//Leaf
{
	if (!isLeaf)
	{
		std::cout << "ERROR: This ADD is for LEAF" << std::endl;
		return -1;
	}
	if (count == 0)
	{
		keys[0] = key;
		offsets[0] = offset;
		++count;
		return 0;
	}
	else
	{
		int index = search(key);
		if (index != -1)//Repeated
			return -1;
		for (int i = 0; i < count; i++)
		{
			if (keys[i] > key)
			{
				index = i;
				break;
			}
		}
		for (int i = count; i > index; i--)
		{
			keys[i] = keys[i - 1];
			offsets[i] = offsets[i - 1];
		}
		keys[index] = key;
		offsets[index] = offset;
		count++;
		return index;
	}
}

template<typename T>
bool TreeNode<T>::remove(int index)
{
	if (index >= count) //OUT of bounds
		return false;
	if (isLeaf)
	{
		for (int i = index; i < count - 1; i++)
		{
			keys[i] = keys[i + 1];
			offsets[i] = offsets[i + 1];
		}
		keys[count - 1] = T();
		offsets[count - 1] = 0;
	}
	else
	{
		for (int i = index; i < count - 1; i++)
			keys[i] = keys[i + 1];
		for (int i = index + 1; i < count; i++)
			childs[i] = childs[i + 1];
		keys[count - 1] = T();
		childs[count] = NULL;
	}
	count--; 
	return true;
}



template<typename T>
void BPTree<T>::init()
{
	root = new TreeNode<T>(degree, true);
	keyCount = 0;
	level = nodeCount = 1;
	leafHead = root;
}

template<typename T>
void BPTree<T>::readBlock(Block * block)
{
	int offsetSize = sizeof(int);
	char* indexAddr = bufferManager.getContent(*block);
	char* contentAddr = bufferManager.getContent(*block);
	char* offsetAddr = indexAddr + keySize;
	T key;
	int offset;

	while (offsetAddr - contentAddr < bufferManager.getUsedSize(*block))
	{
		key = *(T*)indexAddr;
		offset = *(int*)offsetAddr;
		insertKey(key, offset);
		offsetAddr += keySize + offsetSize;
		indexAddr += keySize + offsetSize;
	}
}

template<typename T>
void BPTree<T>::read()
{
	file = bufferManager.getFile(fileName);
	Block *temp = bufferManager.getBlockHead(file);
	while (true)
	{
		if (temp == NULL)
			break;
		readBlock(temp);
		if (temp->end)
			break;
		temp = bufferManager.getNextBlock(file, temp);
	}
}

template<typename T>
void BPTree<T>::write()
{
	Block* btemp = bufferManager.getBlockHead(file);
	Node ntemp = leafHead;
	int offsetSize = sizeof(int);
	
	while (ntemp != NULL)
	{
		char* contentAddr = bufferManager.getContent(*btemp);
		char* baseAddr = contentAddr;
		bufferManager.setUsedSize(*btemp, 0);
		bufferManager.setDirty(*btemp, true);
		for (int i = 0; i < ntemp->count; i++)
		{
			char* key = (char*)&(ntemp->keys[i]);
			char* offset = (char*)&(ntemp->offsets[i]);
			memcpy(contentAddr, key, keySize);
			contentAddr += keySize;
			bufferManager.setUsedSize(*btemp, contentAddr - baseAddr);
			memcpy(contentAddr, offset, offsetSize);
			contentAddr += keySize;
			bufferManager.setUsedSize(*btemp, contentAddr - baseAddr);
		}
		btemp = bufferManager.getNextBlock(file, btemp);
		ntemp = ntemp->next;
	}
	while (true)
	{
		if (btemp->end)
			break;
		bufferManager.setUsedSize(*btemp, 0);
		bufferManager.setDirty(*btemp, true);
		btemp = bufferManager.getNextBlock(file, btemp);
	}
}

template<typename T>
int BPTree<T>::searchKey(T & key)
{
	if (root == NULL)
		return -1; //NO OFFSET
	SearchResult res;
	findLeaf(root, key, res);
	if (!res.isFound)
		return -1;
	else
		return res.node->offsets[res.index];
	return 0;
}

template<typename T>
bool BPTree<T>::insertKey(T & key, int offset)
{
	if (root == NULL)
		init();
	SearchResult res;
	findLeaf(root, key, res);
	if (res.isFound)
		return false; //Duplicated
	else
	{
		res.node->add(key, offset);
		if (res.node->count == degree)
			insertFixUp(res.node);
		keyCount++;
		return true;
	}
}

template<typename T>
bool BPTree<T>::deleteKey(T & key)
{
	SearchResult res;
	if (root == NULL)
		return false;
	findLeaf(root, key, res);
	if (!res.isFound)
		return false;
	if (res.node->isRoot())
	{
		res.node->remove(res.index);
		keyCount--;
		deleteFixUp(res.node);
		return true;
	}
	else
	{
		if (res.index == 0 && leafHead != res.node)
		{
			int index = 0;
			int found = 0;
			Node parent = res.node->parent;
			found = parent->search(key);
			while (found != -1)
			{
				if (parent->parent)
					parent = parent->parent;
				else
					break;
				found = parent->search(key);
			}
			index = found;
			parent->keys[index] = res.node->keys[1];
			res.node->remove(res.index);
			keyCount--;
			deleteFixUp(res.node);
			return true;
		}
		else
		{
			res.node->remove(res.index);
			keyCount--;
			deleteFixUp(res.node);
			return true;
		}
	}
}

template<typename T>
void BPTree<T>::dropTree(Node node)
{
	if (node == NULL)
		return;
	if (!node->isLeaf)
	{
		for (int i = 0; i <= node->count; i++)
		{
			dropTree(node->childs[i]);
			node->childs[i] = NULL;
		}
	}
	delete node;
	nodeCount--;
}

template<typename T>
void BPTree<T>::findLeaf(Node node, T key, SearchResult& res)
{
	//SearchResult res;
	int index = node->search(key);
	if (index != -1) //Found
	{
		if (node->isLeaf)
		{
			res.node = node;
			res.index = index;
			res.isFound = true;
		}
		else
		{
			node = node->childs[index + 1];
			while (!node->isLeaf)
				node = node->childs[0];
			res.node = node;
			res.index = 0;
			res.isFound = true;
		}
	}
	else
	{
		if (node->isLeaf)
		{
			res.node = node;
			res.index = index;
			res.isFound = false;
		}
		else
		{
			for (int i = 0; i < node->count; i++)
			{
				if (node->keys[i] > key)
				{
					index = i;
					break;
				}
			}
			findLeaf(node->childs[index], key, res);
		}
	}
}

template<typename T>
void BPTree<T>::insertFixUp(Node node)
{
	T key;
	Node newNode = node->split(key);
	nodeCount++;
	if (node->isRoot())
	{
		Node root = new TreeNode<T>(degree, false);
		level++; nodeCount++;
		this->root = root;
		node->parent = root;
		newNode->parent = root;
		root->add(key);
		root->childs[0] = node;
		root->childs[1] = newNode;
	}
	else
	{
		Node parent = node->parent;
		int index = parent->add(key);
		parent->childs[index + 1] = newNode;
		newNode->parent = parent;
		if (parent->count == degree)
			insertFixUp(parent);
	}
}

template<typename T>
void BPTree<T>::deleteFixUp(Node node)
{
	int mCount = (degree - 1) / 2;
	if ((node->isLeaf && node->count >= mCount)
		|| (degree != 3 && !node->isLeaf && node->count >= mCount - 1)
		|| (degree == 3 && !node->isLeaf && node->count < 0))
		return;
	if (node->isRoot())
	{
		if (node->count > 0)
			return;
		else
		{
			if (root->isLeaf)
			{
				delete node;
				root = NULL;
				leafHead = NULL;
				level--; nodeCount--;
			}
			else
			{
				root = node->childs[0];
				root->parent = NULL;
				delete node;
				level--; nodeCount--;
			}
		}
	}
	else
	{
		Node parent = node->parent;
		Node sibling = NULL;
		if (node->isLeaf)
		{
			int index = 0;
			T key = node->keys[0];
			for (int i = 0; i < parent->count; i++)
			{
				if (parent->keys[i] >= key)
				{
					index = i;
					break;
				}
			}
			if ((parent->childs[0] != node) && ((index + 1) == parent->count))
			{
				sibling = parent->childs[index];
				if (sibling->count > mCount)
				{
					for (int i = node->count; i > 0; i--)
					{
						node->keys[i] = node->keys[i - 1];
						node->offsets[i] = node->offsets[i - 1];
					}
					node->keys[0] = sibling->keys[sibling->count - 1];
					node->offsets[0] = sibling->offsets[sibling->count - 1];
					sibling->remove(sibling->count - 1);
					node->count++;
					parent->keys[index] = node->keys[0];
					return;
				}
				else
				{
					parent->remove(index);
					for (int i = 0; i < node->count; i++)
					{
						sibling->keys[i + sibling->count] = node->keys[i];
						sibling->offsets[i + sibling->count] = node->offsets[i];
					}
					sibling->count += node->count;
					sibling->next = node->next;
					delete node;
					nodeCount--;
					deleteFixUp(parent);
				}
			}
			else
			{
				if (parent->childs[0] == node)
					sibling = parent->childs[1];
				else
					sibling = parent->childs[index + 2];
				if (sibling->count > mCount)
				{
					node->keys[node->count] = sibling->keys[0];
					node->offsets[node->count] = sibling->offsets[0];
					node->count++;
					sibling->remove(0);
					if (parent->childs[0] == node)
						parent->keys[0] = sibling->keys[0];
					else
						parent->keys[index + 1] = sibling->keys[0];
					return;
				}
				else
				{
					for (int i = 0; i < sibling->count; i++)
					{
						node->keys[node->count + i] = sibling->keys[i];
						node->offsets[node->count + i] = sibling->offsets[i];
					}
					node->count += sibling->count;
					node->next = sibling->next;
					delete sibling;
					nodeCount--;
					if (node == parent->childs[0])
						parent->remove(0);
					else
						parent->remove(index + 1);
					deleteFixUp(parent);
				}
			}
		}
		else //Internal
		{
			int index = 0;
			T key = node->childs[0]->keys[0];
			for (int i = 0; i < parent->count; i++)
			{
				if (parent->keys[i] >= key)
				{
					index = i;
					break;
				}
			}

			if ((parent->childs[0] != node) && (index + 1 == parent->count))
			{
				sibling = parent->childs[index];
				if (sibling->count > mCount - 1)
				{
					node->childs[node->count + 1] = node->childs[node->count];
					for (int i = node->count; i > 0; i--)
					{
						node->childs[i] = node->childs[i - 1];
						node->keys[i] = node->keys[i - 1];
					}
					node->childs[0] = sibling->childs[sibling->count];
					node->keys[0] = parent->keys[index];
					node->count++;
					parent->keys[index] = sibling->keys[sibling->count - 1];
					if (sibling->childs[sibling->count])
						sibling->childs[sibling->count]->parent = node;
					sibling->remove(sibling->count - 1);
					return;
				}
				else
				{
					sibling->keys[sibling->count] = parent->keys[index];
					parent->remove(index);
					sibling->count++;

					for (int i = 0; i < node->count; i++)
					{
						sibling->childs[sibling->count + i] = node->childs[i];
						sibling->keys[sibling->count + i] = node->keys[i];
						sibling->childs[sibling->count + i]->parent = sibling;
					}
					sibling->childs[sibling->count + node->count] = node->childs[node->count];
					sibling->childs[sibling->count + node->count]->parent = node->childs[node->count]->parent;
					sibling->count += node->count;
					delete node;
					nodeCount--;
					deleteFixUp(parent);
				}
			}
			else
			{
				if (parent->childs[0] == node)
					sibling = parent->childs[1];
				else
					sibling = parent->childs[index + 2];
				if (sibling->count > mCount - 1)
				{
					node->childs[node->count + 1] = sibling->childs[0];
					node->keys[node->count] = sibling->keys[0];
					node->childs[node->count + 1]->parent = node;
					node->count++;
					if (node == parent->childs[0])
						parent->keys[0] = sibling->keys[0];
					else
						parent->keys[index + 1] = sibling->keys[0];
					sibling->childs[0] = sibling->childs[1];
					sibling->remove(0);
					return;
				}
				else
				{
					node->keys[node->count] = parent->keys[index];
					if (node == parent->childs[0])
						parent->remove(0);
					else
						parent->remove(index + 1);
					node->count++;
					for (int i = 0; i < sibling->count; i++)
					{
						node->childs[node->count + i] = sibling->childs[i];
						node->keys[node->count + i] = sibling->keys[i];
						node->childs[node->count + i]->parent = node;
					}
					node->childs[node->count + sibling->count] = sibling->childs[sibling->count];
					node->childs[node->count + sibling->count]->parent = node;
					node->count += sibling->count;
					delete sibling;
					nodeCount--;
					deleteFixUp(parent);
				}
			}
		}
	}
}

template<typename T>
BPTree<T>::BPTree(string fileName, int keySize, int degree)
{
	this->fileName = fileName;
	this->keySize = keySize;
	this->degree = degree;
	this->keyCount = this->nodeCount = this->level = 0;
	this->root = this->leafHead = NULL;
	this->file = NULL;
	init();
	read();
}

template<typename T>
BPTree<T>::~BPTree()
{
	//dropTree(root);
}
