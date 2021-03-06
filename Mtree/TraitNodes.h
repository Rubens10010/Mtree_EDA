#ifndef TRAITNODES_H
#define TRAITNODES_H

#include "M_node.h"

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance<Data>,
	typename SplitFunction = functions::split_function<
	        functions::random_promotion,
	        functions::balanced_partition
		>
>
class RootNodeT : public virtual M_node<Data, DistanceFunction, SplitFunction>
{
public:
    void checkDistanceToP() const {
        if(this->distanceToParent != 0)
          std::cerr << "Error in root node";
    }
};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance<Data>,
	typename SplitFunction = functions::split_function<
	functions::random_promotion,
	functions::balanced_partition>>
class NotRootNodeT: public virtual M_node<Data, DistanceFunction, SplitFunction>
{
public:
    size_t getMinCapacity(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const
    {
        return mtree->minNodeCapacity;
    }

    void checkMinCapacity(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const
    {
        if( this->children.size() < mtree->minNodeCapacity)
        {
            std::cerr << "Error Underflow root node" << std::endl;
        }
    }
};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance<Data>,
	typename SplitFunction = functions::split_function<
	functions::random_promotion,
	functions::balanced_partition>>
class LeafNodeT : public virtual M_node<Data, DistanceFunction, SplitFunction>
{
public:
    void addDataConfirm(const Data& data, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) override
    {
        // Leaf nodes store entries
        Entry<Data, DistanceFunction, SplitFunction>* entry = new Entry<Data, DistanceFunction, SplitFunction>(data);
        // this data shouldn't already be in this node
        this->children[data] = entry;
        // assert correct insertion into map
        updateMetrics(entry, distance); // update M-tree semantics
    }

    void addChild(IndexObj* child, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) override
    {
        // new child shouldn't be into children map
        this->children[child->data] = child;
        // assert corrent insertion of new children
        updateMetrics(child, distance);
    }

    M_node* SplitNodeNew(const Data& data) const
    {
        return new LeafNode<Data, DistanceFunction, SplitFunction>(data);  // replace when overflow
    }

    void removeDataConfirm(const Data& data, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) throw(DataNotFound)
    {
        if(this->children.erase(data) == 0)
			std::cerr << "Error while removing" << std::endl;
        throw DataNotFound{data};
    }

    /* Verify correct assignation of leaf nodes*/
    void checkChildClass(IndexObj* child) const
    {
        if(dynamic_cast<Entry*>(child) == NULL)
           std::cerr << "Error while casting leaf node" << std::endl;
    }
};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance<Data>,
	typename SplitFunction = functions::split_function<
	functions::random_promotion,
	functions::balanced_partition>>
class NotLeafNodeT : public virtual M_node<Data, DistanceFunction, SplitFunction>
{
    /* Override base class virtual function*/
    void addDataConfirm(const Data &data, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree)
    {
        // if there are any child with radius containing data point
        EligebleChild minRadiusIncrease = {nullptr, -1.0, std::numeric_limits<double>::infinity()};
        // if found a child with radius >= distance(data,parent(this))
        EligebleChild nearestDistance = {nullptr, -1.0, std::numeric_limits<double>::infinity()};

        // iterate through all children
        for(M_node<Data, DistanceFunction, SplitFunction>::childrenMap::iterator i = this->children.begin(); i != this->children.end(); ++i)
        {
            // cast for polymorphism
            M_node<Data, DistanceFunction, SplitFunction>* child = dynamic_cast<M_node<Data, DistanceFunction, SplitFunction>*>(i->second);
            if(child == NULL)
              std::cerr << "Error while casting children nodes" << '\n';
            double distance = mtree->distance_function(child->data, data);  // distance of new data from child of this node

            // not in radius range
            if(distance > child->radius)
            {
                double increase = distance - child->radius;
                if(increase < minRadiusIncrease.m)
                {
                    // found new candidate child subtree to insert new data
                    minRadiusIncrease = {child,distance,increase};
                }
            }
            else
            {
                if(distance < nearestDistance.m)
                {
                    nearestDistance = {child,distance,distance};
                }
            }
        }

        // chosen candidate
        EligebleChild chosen = (nearestDistance.node != nullptr) ? nearestDistance : minRadiusIncrease;
        M_node* child = chosen.node;

        // catch any overflow
        try{
          child->addData(data, chosen.distance, mtree);
          updateRadius(child);  // since new data was added update covering radius on that subtree
        } catch(SplitNodeReplace &s)
        {
            // remove the child in overflow
            size_t temp = this->children.erase(child->data);
            // check success

            delete child;

            // NUM_nodeS normally 2
            // Create new Nodes and add children of node replace
            for(int i = 0; i < s.NUM_NODES; ++i)
            {
                M_node* newChild = s.newNodes[i];
                double distance = mtree->distance_function(this->data, newChild->data); // update distance to parent
                // add to this node
                addChild(newChild, distance, mtree);
            }
        }
    }

    void addChild(IndexObj* newChild, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree)
    {
        M_node<Data, DistanceFunction, SplitFunction>* nChild = dynamic_cast<M_node<Data, DistanceFunction, SplitFunction>*>(newChild);
        // assert null nChild
        std::vector<childTemp> nChildren;
        nChildren.push_back(childTemp{nChild,distance});

        // method to verify if it's not already a child node
        while(!nChildren.empty())
        {
            // stack
            childTemp cwd = nChildren.back();
            nChildren.pop_back();

            nChild = cwd.child;
            distance = cwd.distance;

            // find for it in children map
            M_node<Data, DistanceFunction, SplitFunction>::childrenMap::iterator i = this->children.find(nChild->data);
            if(i == this->children.end()) // not found
            {
              this->children[nChild->data] = nChild;
              updateMetrics(nChild,distance); // update metrics on this child
            }
            else
            {
              // point to existing child with same data
              M_node<Data, DistanceFunction, SplitFunction>* existing= dynamic_cast<M_node<Data, DistanceFunction, SplitFunction>*>(this->children[nChild->data]);
              if(existing == nullptr || nChild->data == existing->data)
                  std::cerr << " Error while casting existing child " << '\n';

              // assign new children to existing node
              for(M_node<Data, DistanceFunction, SplitFunction>::childrenMap::iterator i = nChild->children.begin(); i != nChild->children.end(); ++i)
              {
                  IndexObj<Data, DistanceFunction, SplitFunction>* grandchild = i->second;
                  existing->addChild(grandchild, grandchild->distanceToParent, mtree);
              }

              // delete node since is not needed
              nChild->children.clear();
              delete nChild;

              try
              {
                  // if it's in overflow
                  existing->checkMaxCapacity(mtree);
              }catch(SplitNodeReplace &e)
              {
                  size_t temp = children.erase(existing->data);
                  // should be successful
                  delete existing;

                  for(int i = 0; i < e.NUM_NODES; ++i)
                  {
                      M_node* nNode = e.newNodes[i];
                      double distance = mtree->distance_function(this->data, nNode->data);
                      nChildren.push_back(childTemp{nNode,distance});   // check for repeated children
                  }
              }
            }
        }
	}

    // Type of node to throw when split is requeried
    M_node* SplitNodeNew(const Data& data) const
    {
        return new InternalNode<Data,DistanceFunction,SplitFunction>(data);
    }

	void removeDataConfirm(const Data& data, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) throw(DataNotFound)
	{ throw DataNotFound{}; }

    M_node* balanceChildren(M_node* theChild, const Mtree<Data, DistanceFunction, SplitFunction>* mtree){}

    void checkChildClass(IndexObj* child) const{
      if(dynamic_cast<InternalNode*>(child) == nullptr || dynamic_cast<LeafNode*>(child) == nullptr);
          std::std::cerr << "Error while casting child nodes" << '\n';
    }

  private:
        struct EligebleChild
        {
            M_node<Data, DistanceFunction, SplitFunction>* node;
            double distance;
            double m; // metric
		};

        struct childTemp
        {
            M_node<Data, DistanceFunction, SplitFunction>* child;
            double distance;
		};
};

#endif // TRAITNODES_H
