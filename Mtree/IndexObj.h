#ifndef INDEXOBJ_H
#define INDEXOBJ_H

#include <iterator>
#include <limits>
#include <map>
#include <queue>
#include <utility>
#include <iostream>
#include "functions.h"
//#include "TraitNodes.h"

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance<Data>,
	typename SplitFunction = functions::split_function<
	functions::random_promotion,
	functions::balanced_partition>>
class Mtree;

template <typename Data, typename DistanceFunction = functions::euclidean_distance<Data>, typename SplitFunction = functions::split_function<functions::random_promotion, functions::balanced_partition>>
class M_node;

template <typename Data, typename DistanceFunction = functions::euclidean_distance<Data>, typename SplitFunction = functions::split_function<functions::random_promotion, functions::balanced_partition>>
class IndexObj
{
    public:
		/*class SplitNodeReplace;
		class RootNodeReplacement;
		class NodeUnderCapacity;
		class DataNotFound;*/
		using distance_function_cached = functions::f_distance_cached<Data, DistanceFunction>;

		class SplitNodeReplace
		{
		public:
			enum { NUM_NODES = 2 };
			M_node<Data,DistanceFunction,SplitFunction>* newNodes[NUM_NODES];

			SplitNodeReplace(M_node<Data, DistanceFunction, SplitFunction>* newNodes[NUM_NODES])
			{
				for (int i = 0; i<NUM_NODES; ++i)
				{
					this->newNodes[i] = newNodes[i];
				}
			}
		};

		class RootNodeReplacement
		{
		public:
			M_node<Data, DistanceFunction, SplitFunction>* newRoot;
		};

		class NodeUnderCapacity {};
		class DataNotFound {
		public:
			Data data;
		};

		//class Mtree;

		using promotedObjects = std::pair<Data, Data>;
		using Partition = std::set<Data>;

        /* Copy and move semantics are not allowed for this object*/
        IndexObj() = delete;
        IndexObj(const IndexObj&) = delete;
        IndexObj(IndexObj&&) = delete;
        IndexObj& operator=(const IndexObj&) = delete;
        IndexObj& operator=(IndexObj&&) = delete;
		virtual ~IndexObj() { };

        Data data;    // data used as reference for distanceToParent
        double radius;  // Covering radius
        double distanceToParent;

		virtual size_t _check(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const{
		// checkRadius
				checkDistanceToP();
				return 1;
		}


    protected:
        IndexObj(const Data& data) : data(data), radius(0), distanceToParent(0){}	// changed distanceToParent from -1 to 0
		virtual void checkDistanceToP() const {
			if (dynamic_cast<const RootNodeT<Data,DistanceFunction,SplitFunction>*>(this) != NULL || distanceToParent < 0)
				std::cerr << "Error in distance to Parent" << std::endl;
		}

	private:
		void checkRadius() const
		{
			if (radius < 0)
				std::cerr << "Error in object radius/n";
		}
        // Functions to check if M tree semantics are always true
};

#endif // INDEXOBJ_H
