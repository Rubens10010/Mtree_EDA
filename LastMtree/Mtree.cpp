#include "Mtree.h"


template <typename splitp, typename distanceF>
Mtree<splitp,distanceF>::Mtree(int m, splitp policy, distanceF D)
{
	this->M = m;
	split_policy = policy;
	Distance_function = D;
	root = new LeafNode(M);	// at the beginning root is always a Leaf Node
}

template <typename splitp, typename distanceF>
Mtree<typename splitp, typename distanceF>::~Mtree()
{
	delete root;
}

template <typename splitp, typename distanceF>
void Mtree<splitp,distanceF>::insert(Entry &e)
{
	if (!root->isLeaf())
	{
		double currentMin = 1000000;
		double distanceToParent = -1;
		int minIndex = -1;
		int i = 0;
		for (std::vector<_object*>::iterator et = root->getBegin(); et != root->getEnd(); et++)
		{
			double distance = getDistanceFor(*et, dynamic_cast<_object>(e));	// calcula la distancia entre el objeto del nodo actual y la nueva entrada
			if (distance < dynamic_cast<RouterNode*>(root)->getRadiusAt())	// verifica si la nueva entrada se encuentra en el rango del nodo actual
			{
				if (currentMin > distance)	// Si la distancia es la minima hasta ahora
				{
					currentMin = distance;	// actualiza la distancia minima
					minIndex = i;	// actualiza el indice del objeto que tiene la distancia minima hacia la nueva entrada
					distanceToParent = distance;	
				}
			}
			i++;
		}

		if (minIndex == -1)	// no hay un objeto en el que la nueva entrada este dentro de su radio
		{
			i = 0;
			double minDif = 100000000;
			double newRadius = -1;
			for (std::vector<_object*>::iterator et = root->getBegin(); et != root->getEnd(); et++)
			{
				double distance = getDistanceFor(*et, dynamic_cast<_object>(e));	// calcula la distancia entre el objeto del nodo actual y la nueva entrada
				double dif = distance - dynamic_cast<RouterNode*>(root)->getRadiusAt();
				if (dif < minDif)	// verifica si es la minima diferencia
				{
					
					minDif = dif;	// actualiza la diferencia de distancia y radio minima hasta ahora
					minIndex = i;	// actualiza el indice del objeto que tiene la diferencia de distancia y radio minima hacia la nueva entrada
					newRadius = distance;
				}
				i++;
			}
			distanceToParent = newRadius;	// falta verificar

			dynamic_cast<RouterNode*>(root->getObjectAt(minDif))->setRadiusOf(minIndex, newRadius);	// falta verificar si lo realiza en el objeto
		}

		insert(root, minIndex, e, distanceToParent);	// inserta 'e' en mi hijo[minIndex] con una distancia distanceToParent
	}
	else
	{
		if (!root->isFull())	// si root es leaf nuevas entradas no tienen padre
		{
			_object* nuevo = &e;	// convertir la entrada en un puntero
			root->add(nuevo,-1,-1);	// la distancia a su padre es indeterminada
		}
		else
		{
			split(root,&e);	// split root
		}
	}
}

template <typename splitp, typename distanceF>
void Mtree<splitp, distanceF>::insert(Node *&N,int i_, _object &entry, double dp)		// Inserta la entrada 'entry' en el Hijo 'i' del nodo N con una distancia dp
{
	Node *Nist = dynamic_cast<RouterNode*>(N->getPointerTo(i_));
	if (!Nist->isLeaf())
	{
		double currentMin = 1000000;
		double distanceToParent = -1;
		int minIndex = -1;
		int i = 0;
		_objet* padre = N->getObjectAt(i_);	// padre del hijo donde se insertara entry

		double auxDistance = getDistanceFor(entry, *padre);

		for (std::vector<_object*>::iterator et = Nist->getBegin(); et != Nist->getEnd(); et++)
		{
			if(auxDistance - Nist->getPDistanceTo(i) < dynamic_cast<RouterNode*>(Nist)->getRadiusAt(i))	// para ahorrar calculos de distancia
			{
				double distance = getDistanceFor(*et, dynamic_cast<_object>(e));	// calcula la distancia de este objeto a la nueva entrada

				if (currentMin > distance)	// Si la distancia es la minima hasta ahora
				{
					currentMin = distance;	// actualiza la distancia minima
					minIndex = i;	// actualiza el indice del objeto que tiene la distancia minima hacia la nueva entrada
					distanceToParent = distance;
				}
			}
			i++;
		}

		if (minIndex == -1)	// no hay un objeto en el que la nueva entrada este dentro de su radio
		{
			i = 0;
			double minDif = 100000000;
			double newRadius = -1;
			for (std::vector<_object*>::iterator et = Nist->getBegin(); et != Nist->getEnd(); et++)
			{
				double distance = getDistanceFor(*et, dynamic_cast<_object>(e));	// calcula la distancia entre el objeto del nodo actual y la nueva entrada
				double dif = distance - dynamic_cast<RouterNode*>(root)->getRadiusAt(i);
				if (dif < minDif)	// verifica si es la minima diferencia
				{

					minDif = dif;	// actualiza la diferencia de distancia y radio minima hasta ahora
					minIndex = i;	// actualiza el indice del objeto que tiene la diferencia de distancia y radio minima hacia la nueva entrada
					newRadius = distance;
				}
				i++;
			}
			distanceToParent = newRadius;	// falta verificar

			dynamic_cast<RouterNode*>(root->getObjectAt(minDif))->setRadiusOf(minIndex, newRadius);	// falta verificar si lo realiza en el objeto
		}

		insert(Nist, minIndex, e, distanceToParent);	// inserta 'e' en mi hijo[minIndex] con una distancia distanceToParent
	}
	else
	{
		if (!Nist->isFull())	// si node es leaf y no esta lleno aun 
		{
			_object* nuevo = &entry;	// convertir la entrada en un puntero
			Nist->add(nuevo, dp, -1);	// entradas en las hojas no tienen radio
		}
		else
		{
			split(Nist, &entry);	// split node
		}
	}
}