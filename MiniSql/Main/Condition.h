#pragma once
#include <string>

using namespace std;

class Condition
{
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
	~Condition();

	bool compare(int value);
	bool compare(float value);
	bool compare(string value);

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

private:
	string attriName;
	string comparedValue;//Value that's being compared
	int operateType;

};

