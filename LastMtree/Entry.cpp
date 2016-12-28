#include "Entry.h"

Entry::Entry(fContainer v_Features, std::string obj_id)
{
	features = v_Features;
	obj_id = obj_id;
}

bool Entry::saveToFile(std::ofstream &f)
{
    if(!f.is_open())
        return false;
    
    for(auto &&et:features)
    {
        f << et << " ";
    }

	f << obj_id;
    return true;
}


bool Entry::readFromFile(std::ifstream &f)
{
    if(!f.is_open())
        return false;
    
    double value;
	for (int i = 0; i < features.size(); i++)
	{
		f >> value;
		features[i] = value;
	}
    f >> obj_id;
    return true;
}

std::string Entry::getId()
{
	return obj_id;
}