#ifndef CONDITION_H
#define CONDITION_H


#include <string>

using namespace std;

class Condition
{
private:
	string attriName;
	string comparedValue;//Value that's being compared
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
		case INEQUAL: return comparedValue != value;
		case LESS: return comparedValue < value;
		case NO_MORE: return comparedValue <= value;
		case EQUAL: return comparedValue == value;
		case NO_LESS: return comparedValue >= value;
		case MORE: return comparedValue > value;
		default: return false;//No such comparision
		}
	}

	bool compare(int value)
	{
		int cmpValue = atoi(comparedValue.c_str());
		switch (this->operateType)
		{
		case INEQUAL: return cmpValue != value;
		case LESS: return cmpValue < value;
		case NO_MORE: return cmpValue <= value;
		case EQUAL: return cmpValue == value;
		case NO_LESS: return cmpValue >= value;
		case MORE: return cmpValue > value;
		default: return false;//No such comparision
		}
	}

	bool compare(float value)
	{
		float cmpValue = (float)atof(comparedValue.c_str());
		switch (this->operateType)
		{
		case INEQUAL: return cmpValue != value;
		case LESS: return cmpValue < value;
		case NO_MORE: return cmpValue <= value;
		case EQUAL: return cmpValue == value;
		case NO_LESS: return cmpValue >= value;
		case MORE: return cmpValue > value;
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