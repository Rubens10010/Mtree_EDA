#include "_object.h"

_object::_object()
{
	numberFeatures = 0;
}

int _object::getFeaturesCount() const
{
	return this->features.size();
}

fContainer _object::getFeatures() const
{
	return features;
}