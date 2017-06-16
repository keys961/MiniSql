#ifndef CONDITION_H
#define CONDITION_H

#include <string>

using namespace std;

class Condition
{
private:
	string attriName;
	string comparedValue;//Value that's being compared, right value
	int operateType;

public:	
	int const static INEQUAL = -1;
	int const static LESS = 0; // <
	int const static NO_MORE = 1; // <= 
	int const static EQUAL = 2; // ==
	int const static NO_LESS = 3;// >= 
	int const static MORE = 4;// >

	Condition(string attributeName, string comparedValue, int operateType)
	{
		this->attriName = attributeName;
		this->comparedValue = comparedValue;
		this->operateType = operateType;
	}
	~Condition() {}

	bool compare(string value)
	{
		switch (this->operateType)
		{
		case INEQUAL: return value != comparedValue;
		case LESS: return value < comparedValue;
		case NO_MORE: return value <= comparedValue;
		case EQUAL: return value == comparedValue;
		case NO_LESS: return value >= comparedValue;
		case MORE: return value > comparedValue;
		default: return false;//No such comparision
		}
	}

	bool compare(int value)
	{
		int cmpValue = atoi(comparedValue.c_str());
		switch (this->operateType)
		{
		case INEQUAL: return value != cmpValue;
		case LESS: return value < cmpValue;
		case NO_MORE: return value <= cmpValue;
		case EQUAL: return value == cmpValue;
		case NO_LESS: return value >= cmpValue;
		case MORE: return value > cmpValue;
		default: return false;//No such comparision
		}
	}

	bool compare(float value)
	{
		float cmpValue = (float)atof(comparedValue.c_str());
		switch (this->operateType)
		{
		case INEQUAL: return value != cmpValue;
		case LESS: return value < cmpValue;
		case NO_MORE: return value <= cmpValue;
		case EQUAL: return value == cmpValue;
		case NO_LESS: return value >= cmpValue;
		case MORE: return value > cmpValue;
		default: return false;//No such comparision
		}
	}

	

	string getAttribute()
	{
		return this->attriName;
	}
	void setAttribute(string attributeName)
	{
		this->attriName = attributeName;
	}

	string getComparedValue()
	{
		return this->comparedValue;
	}

	void setComparedValue(string comparedValue)
	{
		this->comparedValue = comparedValue;
	}

	int getOperateType()
	{
		return this->operateType;
	}

	void setOperateType(int type)
	{
		this->operateType = type;
	}



};
#endif // !CONDITION_H