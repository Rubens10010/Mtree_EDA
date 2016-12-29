#ifndef CONCRETENODES_H
#define CONCRETENODES_H
#include "TraitNodes.h"

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance,
	typename SplitFunction = functions::split_function<
	        functions::random_promotion,
	        functions::balanced_partition
		>
>
class RootLeafNode : public RootNodeT<Data,DistanceFunction,SplitFunction>, public LeafNodeT<Data,DistanceFunction, SplitFunction>
{
  public:
      RootLeafNode(const Data& data) : M_node(data){}
      void removeData(const Data& data, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) throw (RootNodeReplacement, NodeUnderCapacity, DataNotFound) {
  			try {
  				M_node::removeData(data, distance, mtree);
  			} catch (NodeUnderCapacity&) {
  				if(!this->children.empty())
					std::cerr << "Error throw object while removing" << '\n';
				throw RootNodeReplacement{NULL};
  			}
  		}

      size_t getMinCapacity(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const
      {
          return 1;
      }

      void checkMinCapacity(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const
      {
          if(this->children.size() < 1)
			std::cerr << "Node in underflow" << '\n';
      }
};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance,
	typename SplitFunction = functions::split_function<
	        functions::random_promotion,
	        functions::balanced_partition
		>
>
class RootNode : public RootNodeT<Data,DistanceFunction,SplitFunction>, public NotLeafNodeT<Data,DistanceFunction,SplitFunction>
{
  public:
      RootNode(const Data &data): M_node(data) {}
  private:
	  void removeData(const Data& data, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) throw (RootNodeReplacement, NodeUnderCapacity, DataNotFound) { throw DataNotFound{}; }
      size_t getMinCapacity(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const{
        return 2;
      }

      void checkMinCapacity(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const{
        if(this->children.size() < 2)
			std::cerr << "Error root node in underflow" << '\n';
      }
};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance,
	typename SplitFunction = functions::split_function<
	        functions::random_promotion,
	        functions::balanced_partition
		>
>
class InternalNode: public NotRootNodeT<Data,DistanceFunction,SplitFunction>,public NotLeafNodeT<Data,DistanceFunction,SplitFunction>
{
  public:
    InternalNode(const Data& data):M_node(data){}
};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance,
	typename SplitFunction = functions::split_function<
	        functions::random_promotion,
	        functions::balanced_partition
		>
>
class LeafNode: public NotRootNodeT<Data,DistanceFunction,SplitFunction>, public LeafNodeT<Data,DistanceFunction,SplitFunction>
{
  public:
    LeafNode(const Data& data):M_node(data){}
};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance,
	typename SplitFunction = functions::split_function<
	        functions::random_promotion,
	        functions::balanced_partition
		>
>
class Entry : public IndexObj<Data,DistanceFunction,SplitFunction>
{
  public:
      Entry(const Data& data) : IndexObj(data) {}
};

#endif
