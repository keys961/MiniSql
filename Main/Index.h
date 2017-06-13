#ifndef INDEX_H
#define INDEX_H



#include <string>

using namespace std;

class Index
{
public:
	Index(string indexName, string tableName, string attriName, int type)
	{
		this->indexName = indexName;
		this->tableName = tableName;
		this->attriName = attriName;
		this->type = type;
	}
	string indexName;
	string tableName;
	string attriName;
	int type;
};

#endif // !INDEX_H