#include "LeafNode.h"

LeafNode::~LeafNode()
{
	for (auto e : objetos)
		delete e;
}

bool LeafNode::check()
{
	assert(entries.size() > 0);
	return true;
}

Entry LeafNode::getEntryAt(int i)
{
	return entries[i];
}

void LeafNode::add(_object* n, double d, double r)
{
	assert(isFull() == false && initialized == true);
	objetos[current] = n;	// shall not be initialized like this
	distances[current] = d;
	current++;
	Entry *nueva = dynamic_cast<Entry*>(n);		// cast to get Entry information
	addEntry(*nueva);	// add it to class
}

void LeafNode::addEntry(Entry &e)
{
	entries[current - 1] = e;
}