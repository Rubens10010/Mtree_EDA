#pragma once
#include "_object.h"

class routerObj :public _object
{
public:
	routerObj(){}
	routerObj(fContainer v);
	virtual ~routerObj(){}

	virtual bool saveToFile(std::ofstream &f);
	virtual bool readFromFile(std::ifstream &f);
};
