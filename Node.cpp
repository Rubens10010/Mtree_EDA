#include "Node.h"

unsigned int Node::minCapacity = 0;
unsigned int Node::maxCapacity = 0;

Node::Node()
{
    initialized = false;
	current = 0;
	leaf = false;
}

Node::Node(unsigned int M)
{
	objetos.resize(M);
	current = 0;
	initialized = true;
	leaf = false;
}

Node::~Node()
{
	for (auto e : objetos)
		delete e;
}

void Node::setNodeSize(unsigned int m)
{
    this->maxCapacity = m;
    objetos.resize(maxCapacity);
    initialized = true;
}

bool Node::isFull()
{
	assert(Node::maxCapacity > 0);	// El que llama a la funcion ya tuvo que haber inicializado los limites
    return (objetos.size() == maxCapacity) ? true : false;
}

fContainer Node::getFeaturesAt(int i)
{
	return objetos[i]->getFeatures();
}

_object* Node::getObjectAt(int i)
{
	return objetos[i];
}

double Node::getPDistanceTo(int i)
{
	return distances[i];
}

void Node::setDistanceOf(int i, double d)
{
	distances[i] = d;
}

bool Node::isLeaf()
{
	return leaf;
}
std::vector<_object*>::iterator Node::getBegin()
{
	return objetos.begin();
}

std::vector<_object*>::iterator Node::getEnd()
{
	return objetos.end();
}