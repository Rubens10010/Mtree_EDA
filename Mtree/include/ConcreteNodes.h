#ifndef CONCRETENODES_H
#define CONCRETENODES_H

#include "TraitNodes.h"


template <
	typename Data,
	typename DistanceFunction = ::mt::functions::euclidean_distance,
	typename SplitFunction = ::mt::functions::split_function<
	        ::mt::functions::random_promotion,
	        ::mt::functions::balanced_partition
		>
>
class RootLeafNode : public RootNodeT<Data,DistanceFunction,SplitFunction>, public LeafNodeT<Data,DistanceFunction, SplitFunction>
{
  public:
      RootLeafNode(const Data& data) : M_node(data){};
      void removeData(const Data& data, double distance, const mtree* mtree) throw (RootNodeReplacement, DataNotFound) {
  			try {
  				Node::removeData(data, distance, mtree);
  			} catch (NodeUnderCapacity&) {
  				if(!this->children.empty())
              std::std::cerr << "Error throw object while removing" << '\n';
          throw RootNodeReplacement{NULL};
  			}
  		}

      size_t getMinCapacity(const mtree* mtree) const
      {
          return 1;
      }

      void checkMinCapacity(const Mtree* mtree) const
      {
          if(this->children.size() < 1)
            std::std::cerr << "Node in underflow" << '\n';
      }
};

template <
	typename Data,
	typename DistanceFunction = ::mt::functions::euclidean_distance,
	typename SplitFunction = ::mt::functions::split_function<
	        ::mt::functions::random_promotion,
	        ::mt::functions::balanced_partition
		>
>
class RootNode : public RootNodeT<Data,DistanceFunction,SplitFunction>, public NotLeafNodeT<Data,DistanceFunction,SplitFunction>
{
  public:
      RootNode(const Data &data): M_node(data) {}
  private:
      void removeData(const Data& data, double distance, const Mtree* mtree) throw (RootNodeReplacement, NodeUnderCapacity, DataNotFound);
      size_t getMinCapacity(const Mtree* mtree) const{
        return 2;
      }

      void checkMinCapacity(const Mtree* mtree) const{
        if(this->children.size() < 2)
          std::std::cerr << "Error root node in underflow" << '\n';
      }
};

template <
	typename Data,
	typename DistanceFunction = ::mt::functions::euclidean_distance,
	typename SplitFunction = ::mt::functions::split_function<
	        ::mt::functions::random_promotion,
	        ::mt::functions::balanced_partition
		>
>
class InternalNode: public NotRootNodeT<Data,DistanceFunction,SplitFunction>,public NotLeafNodeT<Data,DistanceFunction,SplitFunction>
{
  public:
    InternalNode(const Data& data):M_node(data){}
};

template <
	typename Data,
	typename DistanceFunction = ::mt::functions::euclidean_distance,
	typename SplitFunction = ::mt::functions::split_function<
	        ::mt::functions::random_promotion,
	        ::mt::functions::balanced_partition
		>
>
class LeafNode: public NotRootNodeT<Data,DistanceFunction,SplitFunction>,public leafNodeT<Data,DistanceFunction,SplitFunction>
{
  public:
    LeafNode(const Data& data):M_node(data){}
};

template <
	typename Data,
	typename DistanceFunction = ::mt::functions::euclidean_distance,
	typename SplitFunction = ::mt::functions::split_function<
	        ::mt::functions::random_promotion,
	        ::mt::functions::balanced_partition
		>
>
class Entry : public IndexObj<Data,DistanceFunction,SplitFunction>
{
  public:
      Entry(const Data& data) : IndexObj(data) {}
};

#endif CONCRETENODES_H
