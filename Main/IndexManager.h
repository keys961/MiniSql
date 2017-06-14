#pragma once
#include "BPTree.h"
#include "Index.h"
#include "Attribute.h"
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

using namespace std;

class IndexManager
{
private:
	map<string, BPTree<int>*> intIndices; //Filename, BTree
	map<string, BPTree<string>*> stringIndices;
	map<string, BPTree<float>*> floatIndices;

	static const int FLOAT = Attribute::FLOAT;
	static const int INT = Attribute::INT;

	struct KeyTemp
	{
		int intTemp;
		float floatTemp;
		string stringTemp;
	};
	KeyTemp keyTemp;

	void setKey(int type, string key) 
	{
		stringstream ss;
		ss << key;
		switch (type)
		{
		case INT: ss >> this->keyTemp.intTemp; break;
		case FLOAT: ss >> this->keyTemp.floatTemp; break;
		default:
			ss >> this->keyTemp.stringTemp; break;
		}
	}
	int getKeySize(int type)
	{
		switch (type)
		{
		case INT: return sizeof(int);
		case FLOAT: return sizeof(float);
		default:
			return type;
		}
	}

public:
	IndexManager();
	~IndexManager();

	void createIndex(string fileName, int type);
	void dropIndex(string fileName, int type);
	int searchIndex(string fileName, string key, int type);
	void insertKeyIntoIndex(string fileName, string key, int type, int offset);
	void deleteKeyFromIndex(string fileName, string key, int type);
};

