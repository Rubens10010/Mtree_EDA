/*#ifndef EXCEPTIONCLASSES_H
#define EXCEPTIONCLASSES_H
#include "M_node.h"

//class M_node;

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance,
	typename SplitFunction = functions::split_function<
	functions::random_promotion,
	functions::balanced_partition>>
class SplitNodeReplace
{
public:
    enum {NUM_NODES = 2};
    M_node<Data,DistanceFunction,SplitFunction>* newNodes[NUM_NODES];

    SplitNodeReplace(M_node<Data, DistanceFunction, SplitFunction>* newNodes[NUM_NODES])
    {
      for(int i=0; i<NUM_NODES; ++i)
      {
          this->newNodes[i] = newNodes[i];
      }
    }
};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance,
	typename SplitFunction = functions::split_function<
	functions::random_promotion,
	functions::balanced_partition>>
class RootNodeReplacement
{
public:
    M_node* newRoot;
};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance,
	typename SplitFunction = functions::split_function<
	functions::random_promotion,
	functions::balanced_partition>>
class NodeUnderCapacity{};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance,
	typename SplitFunction = functions::split_function<
	functions::random_promotion,
	functions::balanced_partition>>
class DataNotFound{
  public:
      Data data;
};

#endif*/