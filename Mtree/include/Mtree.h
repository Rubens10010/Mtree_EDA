#ifndef MTREE_H
#define MTREE_H

#include "ConcreteNodes.h"
#include "ExceptionClasses.h"

template <
	typename Data,
	typename DistanceFunction = ::mt::functions::euclidean_distance,
	typename SplitFunction = ::mt::functions::split_function<
	        ::mt::functions::random_promotion,
	        ::mt::functions::balanced_partition
		>
>
class Mtree
{
  public:
    using distance_function_type = DistanceFunction;
    using split_function_policy = SplitFunction;
    using distance_function_cached = functions::f_distance_cached<Data,DistanceFunction>;

    explicit Mtree(size_t min_capacity = DEFAULT_MIN_NODE_CAPACITY, size_t max_capacity = -1, const DistanceFunction& distance_func = DistanceFunction(),
    const SplitFunction& split_function = SplitFunction())
    :minNodeCapacity(min_capacity),maxNodeCapacity(max_capacity),distance_function(distance_function), split_function(split_function)
    {
      if(max_capacity == size_t(-1))
      {
          this->maxNodeCapacity = 2*min_capacity -1;
      }
    }

    Mtree(const Mtree&) = delete;

    Mtree(Mtree&& other):root(other.root), maxNodeCapacity(other.maxNodeCapacity),
                        minNodeCapacity(other.distance_function), split_function(other.split_function)
    {
      other.root = NULL;
    }

    ~Mtree()
    {
      delete root;
    }

    Mtree& operator=(const Mtree&) = delete;

    // add a new object data to index
    void add(const Data& data)
    {
        if(root == NULL)
        {
            root = new RootLeafNode(data);
            root->addData(data,0,this);
        }
        else{
          double distance = distance_function(data,root->data);
          try{
            root->addData(data, distance, this);
          }catch(SplitNodeReplace &e)
          {
            // creates new root to insert new nodes to replace
            M_node* nRoot = new RootNode(root->data);
            delete root;
            root = nRoot;
            for(int i= 0; i < SplitNodeReplace::NUM_NODES; i++)
            {
                M_node* nNode = e.newNodes[i];
                double distance = distance_function(root->data, nNode->data);
                // insert new children into root
                root->addChild(nNode,distance,this);
            }
          }
        }
    }

    bool remove(const Data& data)
    {
        if(root == nullptr)
          return false;
        double distanceToRoot = distance_function(data,root->data);
        try{
          root->removeData(data, distanceToRoot, this);
        }catch(RootNodeReplacement &e)  // when underflow
        {
          delete root;
          root = e.newRoot;
        }catch(DataNotFound)    // if query data wasn't found
        {
          return false;
        }
        return true;
    }

    // queries

    void check() const
    {
        if(root != NULL)
        {
          root->check(this);
        }
    }

    size_t minNodeCapacity;
    size_t maxNodeCapacity;
    M_node* root;
    DistanceFunction distance_function;
    SplitFunction split_function;

  private:

};

#endif MTREE_H
