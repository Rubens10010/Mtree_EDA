#pragma once
#include "Node.h"

/**
*	RouterNode es una nodo usado para realizar busquedas en el M tree, sirve como ruta para encontrar objetos que cumplan
	las condiciones de busqueda.
*/

using vChildren = std::vector<Node*>;

class RouterNode : public Node
{
public:
	RouterNode();
	RouterNode(int M);
	~RouterNode();

	virtual void add(_object* n, double d = 0, double r = 0) override;
	void check();
	void checkDistance();
	Node* getPointerTo(int i);
	double getRadiusAt(int i);
	double getPDistanceTo(int i);

private:

	void setPDistanceOf(int i, double d);
	void setRadiusOf(int i, double r);

	int current;
	vChildren children;
	values radius;
	values distances;
};

