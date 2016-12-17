#pragma once
#include <iostream>
#include <string>
#include "_object.h"

class Entry:public _object
{
public:

	Entry() {}
	Entry(fContainer v_Features, std::string obj_id);
	~Entry() {}

    virtual bool saveToFile(std::ofstream &f);
    virtual bool readFromFile(std::ifstream &f);
	std::string getId();

private:
     std::string obj_id;
};