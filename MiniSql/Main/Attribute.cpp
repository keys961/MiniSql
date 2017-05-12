#include "stdafx.h"
#include "Attribute.h"

Attribute::~Attribute()
{
}

string Attribute::getName()
{
	return this->name;
}

void Attribute::setName(string name)
{
	this->name = name;
}

int Attribute::getType()
{
	return this->type;
}

void Attribute::setType(int type)
{
	this->type = type;
}

bool Attribute::isUnique()
{
	return this->unique;
}

void Attribute::setUnique(int unique)
{
	this->unique = unique;
}

void Attribute::setIndexName(string indexName)
{
	this->indexName = indexName;
}

string Attribute::getIndexName()
{
	return this->indexName;
}
