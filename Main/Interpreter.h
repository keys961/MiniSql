#pragma once
#include "API.h"
using namespace std;
class Interpreter
{
private:
	string getWord(string&);
	bool getCreateTable(string&);
	bool getCreateIndex(string&);
	bool getDropTable(string&);
	bool getDropIndex(string&);
	bool getSelect(string&);
	bool getInsert(string&);
	bool getDelete(string&);
	bool getExecfile(string&);
	bool judge(char);
	void getCondition(string&,vector<Condition>* conditionList);
	string lowwerCase(string&);
	API* api = new API();
public:
	Interpreter();
	bool Parse(string);
	~Interpreter();
};

