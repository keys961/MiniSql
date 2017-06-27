#pragma once
#include "API.h"
using namespace std;
class Interpreter
{
private:
	/*string getWord(string&);*/
	bool getCreateTable();
	bool getCreateIndex();
	bool getDropTable();
	bool getDropIndex();
	bool getSelect();
	bool getInsert();
	bool getDelete();
	bool getExecfile(string&);
	void getCondition(int,vector<Condition>* conditionList);
	vector<string> split(const string &s, const string &seperator);
	string lowwerCase(string&);
	API* api = new API();
	vector<string> cmd;
public:
	Interpreter();
	bool Parse(string);
	~Interpreter();
};

