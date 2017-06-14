#include "stdafx.h"
#include "IndexManager.h"



IndexManager::IndexManager()
{

}


IndexManager::~IndexManager()
{
	map<string, BPTree<int>*>::iterator itInt = intIndices.begin();
	map<string, BPTree<float>*>::iterator itFloat = floatIndices.begin();
	map<string, BPTree<string>*>::iterator itString = stringIndices.begin();

	for (; itInt != intIndices.end(); itInt++)
	{
		if (itInt->second)
		{
			itInt->second->write();
			delete itInt->second;
		}
	}

	for (; itFloat != floatIndices.end(); itFloat++)
	{
		if (itFloat->second)
		{
			itFloat->second->write();
			delete itFloat->second;
		}
	}

	for (; itString != stringIndices.end(); itString++)
	{
		if (itString->second)
		{
			itString->second->write();
			delete itString->second;
		}
	}
}

//Create an empty index with new fileName
void IndexManager::createIndex(string fileName, int type)
{
	int keySize = getKeySize(type);
	int degree = 20;
	if (type == INT)
	{
		BPTree<int> * tree = new BPTree<int>(fileName, keySize, degree);
		intIndices.insert(std::pair<string, BPTree<int>*>(fileName, tree));
	}
	else if (type == FLOAT)
	{
		BPTree<float> * tree = new BPTree<float>(fileName, keySize, degree);
		floatIndices.insert(std::pair<string, BPTree<float>*>(fileName, tree));
	}
	else
	{
		BPTree<string> * tree = new BPTree<string>(fileName, keySize, degree);
		stringIndices.insert(std::pair<string, BPTree<string>*>(fileName, tree));
	}
}

void IndexManager::dropIndex(string fileName, int type)
{
	if (type == INT)
	{
		map<string, BPTree<int>*>::iterator it = intIndices.find(fileName);
		if (it == intIndices.end())
		{
			cout << "No Index Found" << endl;
			return;
		}
		else
		{
			delete it->second;
			intIndices.erase(it);
		}
	}
	else if (type == FLOAT)
	{
		map<string, BPTree<float>*>::iterator it = floatIndices.find(fileName);
		if (it == floatIndices.end())
		{
			cout << "No Index Found" << endl;
			return;
		}
		else
		{
			delete it->second;
			floatIndices.erase(it);
		}
	}
	else
	{
		map<string, BPTree<string>*>::iterator it = stringIndices.find(fileName);
		if (it == stringIndices.end())
		{
			cout << "No Index Found" << endl;
			return;
		}
		else
		{
			delete it->second;
			stringIndices.erase(it);
		}
	}
}

int IndexManager::searchIndex(string fileName, string key, int type)
{
	setKey(type, key);
	if (type == INT)
	{
		map<string, BPTree<int>*>::iterator it = intIndices.find(fileName);
		if (it == intIndices.end())
		{
			cout << "No Index Found" << endl;
			return -1;
		}
		else
			return it->second->searchKey(keyTemp.intTemp);
	}
	else if (type == FLOAT)
	{
		map<string, BPTree<float>*>::iterator it = floatIndices.find(fileName);
		if (it == floatIndices.end())
		{
			cout << "No Index Found" << endl;
			return -1;
		}
		else
			return it->second->searchKey(keyTemp.floatTemp);
	}
	else
	{
		map<string, BPTree<string>*>::iterator it = stringIndices.find(fileName);
		if (it == stringIndices.end())
		{
			cout << "No Index Found" << endl;
			return -1;
		}
		else
			return it->second->searchKey(keyTemp.stringTemp);
	}
}

void IndexManager::insertKeyIntoIndex(string fileName, string key, int type, int offset)
{
	setKey(type, key);
	if (type == INT)
	{
		map<string, BPTree<int>*>::iterator it = intIndices.find(fileName);
		if (it == intIndices.end())
		{
			cout << "No Index Found" << endl;
			return;
		}
		else
			it->second->insertKey(keyTemp.intTemp, offset);
	}
	else if (type == FLOAT)
	{
		map<string, BPTree<float>*>::iterator it = floatIndices.find(fileName);
		if (it == floatIndices.end())
		{
			cout << "No Index Found" << endl;
			return ;
		}
		else
			it->second->insertKey(keyTemp.floatTemp, offset);
	}
	else
	{
		map<string, BPTree<string>*>::iterator it = stringIndices.find(fileName);
		if (it == stringIndices.end())
		{
			cout << "No Index Found" << endl;
			return;
		}
		else
			it->second->insertKey(keyTemp.stringTemp, offset);
	}
}

void IndexManager::deleteKeyFromIndex(string fileName, string key, int type)
{
	setKey(type, key);
	if (type == INT)
	{
		map<string, BPTree<int>*>::iterator it = intIndices.find(fileName);
		if (it == intIndices.end())
		{
			cout << "No Index Found" << endl;
			return;
		}
		else
			it->second->deleteKey(keyTemp.intTemp);
	}
	else if (type == FLOAT)
	{
		map<string, BPTree<float>*>::iterator it = floatIndices.find(fileName);
		if (it == floatIndices.end())
		{
			cout << "No Index Found" << endl;
			return;
		}
		else
			it->second->deleteKey(keyTemp.floatTemp);
	}
	else
	{
		map<string, BPTree<string>*>::iterator it = stringIndices.find(fileName);
		if (it == stringIndices.end())
		{
			cout << "No Index Found" << endl;
			return;
		}
		else
			it->second->deleteKey(keyTemp.stringTemp);
	}
}
