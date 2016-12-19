#pragma once
#include "Node.h"
#include "Entry.h"
/**
*	Leaf Node es un nodo usado para almacenar los identificadores de los objetos y como objetos de salida de
*	consultas por rango.
*/

class LeafNode: public Node
{
public:
	LeafNode() :Node() { leaf = true; }
	LeafNode(int M) : Node(M) { leaf = true; }
	~LeafNode();

	virtual void add(_object* n, double d = 0, double r = 0) override;
	void addEntry(Entry &e);
	bool check();
	Entry getEntryAt(int i);

private:
	std::vector<Entry> entries;
};

