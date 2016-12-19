#pragma once
#include <vector>
#include "_object.h"
#include <assert.h>
//#include "Entry.h"

using values = std::vector<double>;

class Node
{
    public:
        Node();
        Node(unsigned int M);
		virtual ~Node();

        virtual void add(_object* n, double d = 0, double r = 0) = 0;
        void setNodeSize(unsigned int m);
        bool isFull();
		bool isLeaf();
        fContainer getFeaturesAt(int i);
		_object* getObjectAt(int i);		// Return the object : routerObj
		virtual double getPDistanceTo(int i);

		static unsigned int minCapacity;
		static unsigned int maxCapacity;

	protected:
	    virtual void setDistanceOf(int i,double d);
		bool leaf;
        bool initialized;
		int current;
        std::vector<_object*> objetos;	// routerNodes almacenan RouterObj , LeafNodes almacenan Entries
		values distances;
};