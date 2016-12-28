#include "routerObj.h"

routerObj::routerObj(fContainer v)
{
	features = v;
}

bool routerObj::saveToFile(std::ofstream &f)
{
	if (!f.is_open())
		return false;

	for (auto &&et : features)
	{
		f << et << " ";
	}

	return true;
}


bool routerObj::readFromFile(std::ifstream &f)
{
	if (!f.is_open())
		return false;

	double value;
	for (int i = 0; i < features.size(); i++)
	{
		f >> value;
		features[i] = value;
	}
	return true;
}