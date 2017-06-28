#pragma once

#include "stdafx.h"
#include "Interpreter.h"

Interpreter::Interpreter()
{
}
vector<string> Interpreter:: split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}
//bool Interpreter::judge(char k)
//{
//	if (k == ' ' || k == '\n' || k == '\'' || k == ',')
//		return false;
//	return true;
//}
void Interpreter::getCondition(int k,vector<Condition>* conditionList)
{
	Condition * condition;
	string attri, ope, value;
	while (true)
	{
		attri = cmd[k++];
		ope = cmd[k++];
		value = cmd[k++];
		condition = new Condition(attri, value, 0);
		if (ope == "=")
		{
			condition->setOperateType(2);
		}
		else if (ope == "<>")
		{
			condition->setOperateType(-1);
		}
		else if (ope == "<")
		{
			condition->setOperateType(0);
		}
		else if (ope == ">")
		{
			condition->setOperateType(4);
		}
		else if (ope == "<=")
		{
			condition->setOperateType(1);
		}
		else if (ope == ">=")
		{
			condition->setOperateType(3);
		}
		conditionList->push_back(*condition);
		delete condition;
		if (k==cmd.size()) break;
	}
}
bool Interpreter::getCreateTable()
{
	int k = 2;
	string tableName=cmd[k++];
	vector<Attribute> attriList;
	Attribute attribute;
	string word,type,foo;
	word = cmd[k++];
	type = cmd[k++];
	while (!(word == "primary"&&type == "key"))
	{
		attribute.name = word;
		if (type == "int")
			attribute.type = attribute.INT;
		else if (type == "float")
			attribute.type = attribute.FLOAT;
		else
		{
			attribute.type=atoi(cmd[k++].c_str());
		}
		foo = cmd[k++];
		if (foo == "unique")
		{
			attribute.unique = 1;
			word = cmd[k++];
			type = cmd[k++];
		}
		else
		{
			attribute.unique = 0;
			word = foo;
			type = cmd[k++];
		}
		attriList.push_back(attribute);

	}
	word = cmd[k++];
	int i,pos,flag=0;
	for (i=0;i<attriList.size();i++)
		if (attriList[i].name == word)
		{
			pos = i;
			attriList[i].unique = 1;
			return api->createTable(tableName, &attriList, word, pos);
		}
	return false;
}
bool Interpreter::getCreateIndex()
{
	int k = 2;
	string indexName, tableName,columnName;
	indexName = cmd[k++];
	if (cmd[k++] != "on")
		return false;
	tableName = cmd[k++];
	columnName = cmd[k++];
	api->createIndex(indexName, tableName, columnName);
}
bool Interpreter::getDropTable()
{
	int k = 2;
	string tableName = cmd[k++];
	bool flag=api->dropTable(tableName);
	return flag;
}
bool Interpreter::getDropIndex()
{
	int k = 2;
	string indexName = cmd[k++];
	bool flag = api->dropIndex(indexName);
	return flag;
}
bool Interpreter::getSelect()
{
	int k = 1;
	vector<Condition> conditionList;
	if (cmd[k++] != "*")
		return false;
	if (cmd[k++] != "from")
		return false;
	string tableName = cmd[k++];
	if (k==cmd.size())
		return api->select(tableName, &conditionList);
	if (cmd[k++] != "where")
		return false;
	getCondition(k, &conditionList);
	return api->select(tableName, &conditionList);
}
bool Interpreter::getInsert()
{
	int k = 1;
	string tableName;
	if (cmd[k++] != "into")
		return false;
	tableName = cmd[k++];
	if (cmd[k++] != "values")
		return false;
	string record="";
	vector<string> st;
	while (k!=cmd.size())
	{
		st.push_back(cmd[k++]);
	}
	return api->insert(tableName,st);
}
bool Interpreter::getDelete()
{
	int k = 1;
	if (cmd[k++]!="from")
		return false;
	string tableName = cmd[k++];
	vector<Condition> conditionList;
	getCondition(k, &conditionList);
	api->deleteFromTable(tableName,&conditionList);
	return true;
}
bool Interpreter::getExecfile(string&)
{
	return 0;
}
//string Interpreter::getWord(string &cmd)
//{
//	while (judge(cmd[0])==false)
//	{
//		cmd.erase(0, 1);
//	}
//	string ret = "";
//	while (cmd.length() != 0 && judge(cmd[0]) == true)
//	{
//		ret += cmd[0];
//		cmd.erase(0, 1);
//	}
//	return ret;
//}
string Interpreter::lowwerCase(string& st)
{
	int i = 0;
	while (i < st.length())
	{
		if (int(st[i]) >= 65 && int(st[i]) <= 90) st[i] = char(int(st[i]) + 32);
		i++;
	}
	return st;
}
bool Interpreter::Parse(string st)
{
	st = lowwerCase(st);
	cmd = split(st, " ,();'\n");
	string option=cmd[0];
	if (option == "select")
	{
		return getSelect();
	}
	else
	if (option == "create")
	{
		if (cmd[1] == "index")
		{
			return getCreateIndex();
		}
		else
		if (cmd[1] == "table")
		{
			return getCreateTable();
		}
		else
			return false;
	}
	else
	if (option == "drop")
	{
		if (cmd[1] == "index")
		{
			return getDropIndex();
		}
		else
		if (cmd[1] == "table")
		{
			return getDropTable();
		}
		else
			return false;
	}
	else
	if (option == "insert")
	{
		return getInsert();
	}
	else
	if (option == "delete")
	{
		return getDelete();
	}
	else
	if (option == "execfile")
	{
		return getExecfile(cmd[1]);
	}
	else
	{
		return false;
	}
}
Interpreter::~Interpreter()
{
	delete api;
}