#pragma once
#include <vector>
#include <fstream>

using fContainer = std::vector<double>;

class _object
{
public:
	_object();
	virtual ~_object() {}

	int getFeaturesCount() const;
	fContainer getFeatures() const;

	virtual bool saveToFile(std::ofstream &f) = 0;
	virtual bool readFromFile(std::ifstream &f) = 0;

protected:

	int numberFeatures;
	fContainer features;
};
