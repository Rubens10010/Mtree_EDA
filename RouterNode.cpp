#include "RouterNode.h"

RouterNode::RouterNode():Node(){}
RouterNode::RouterNode(int M):Node(M){}

RouterNode::~RouterNode()
{
	for (auto &e : children)
		delete e;
}

void RouterNode::add(_object* n, double d, double r)
{
	assert(isFull() == false && initialized == true);
	objetos[current] = n;
	setDistanceOf(current, d);
	setDistanceOf(current, r);
	current++;
}

void RouterNode::check()
{
	checkDistance();
}
void RouterNode::checkDistance()
{

}

Node* RouterNode::getPointerTo(int i)
{
	return children[i];
}
double RouterNode::getRadiusAt(int i)
{
	return radius[i];
}

double RouterNode::getPDistanceTo(int i)
{
	return distances[i];
}

void RouterNode::setPDistanceOf(int i, double d)
{
	distances[i] = d;
}

void RouterNode::setRadiusOf(int i, double r)
{
	radius[i] = r;
}