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
