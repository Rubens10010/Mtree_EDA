#ifndef M_node_H
#define M_node_H
#include "IndexObj.h"

typedef std::map<Data, IndexObj*> childrenMap;  // used to store data and child nodes of this.

template <
	typename Data,
	typename DistanceFunction = ::mt::functions::euclidean_distance,
	typename SplitFunction = ::mt::functions::split_function<
	        ::mt::functions::random_promotion,
	        ::mt::functions::balanced_partition
		>
>
class M_node : IndexObj
{
    public:
        M_node(int m);

        virtual ~M_node()
        {
            for(typename childrenMap::iterator i = children.begin(); i != children.end(); ++i)
            {
                IndexObj* child = i->second;
                delete child;
            }
        }

        void addData(const Data& data, double distance, const mtree* Mtree) throw (SplitNodeReplace); // this class add a new data object to this Node, throws an exception if it's full
        void checkMaxCapacity(const Mtree* mtree) throw (SplitNodeNew)
        {
            if(children.size() > mtree->maxCapacity)  // Node in overflow
            {
                Partition p1;
                for(auto childrenMap::iterator i = childrenMap.begin(); i != childrenMap.end(); ++i)
                {
                    p1.insert(i->first);  // add all children objects to first patition
                }

                distance_function_cached savedDistanceFunction(mtree->distance_function);

                Partition p2;
                promotedObjects promoted = mtree->split_function(p1,p2,distance_function_cached); // get Promoted Nodes from split policy

                M_node* newNodes[2];
                for(int i = 0; i < 2; ++i)
                {
                    Data& promotedObject = (i == 0)? promoted.first : promoted.second;
                    Partition& partition = (i == 0) ? p1 : p2;
                    M_node* newNode = SplitNodeNew(promotedObject); // creating Node to throw with first partition data
                    for(auto Partition::iterator j = partition.begin(); j!= partition.end(); ++j)
                    {
                        const Data& data = *j;
                        IndexObj* child = children[data];
                        children.erase(data);
                        double distance = savedDistanceFunction(promotedObject, data);
                        newNode->addChild(child, distance, mtree);
                    }

                    newNodes[i] = newNode;
                }
                throw SplitNodeReplace(newNode);
            }
        }

        virtual void addChild(IndexObj* child, double distance, const Mtree* mtree) = 0; // insert new Child node into this node
        virtual void removeData();
        virtual size_t getMinCapacity(const Mtree* mtree) const = 0;
    protected:

        M_node(const Data& data):IndexObj(data){}
        M_node():IndexObj(*(Data*)(0)){std::cout << "This is never called";}
        // delete copy and move semantics

        virtual void addDataConfirm(const Data& data, double distance, const Mtree* mtree) = 0;
        virtual void removeDataConfirm(const Data& data, double distance, const Mtree* mtree) = 0;

        virtual Node* SplitNodeNew(const Data&) const = 0;  // creates a new temporary Node containing promoted object and childs to Replace another node data

        void updateMetrics(IndexObj* child, double distance)  // preserves semantics of M-tree
        {
            child->distanceToParent = distance;
            updateRadius(child);
        }

        void updateRadius(IndexObj* child)
        {
            this->radius = std::max(this->radius, child->distanceToParent + child->radius);
        }

        // functions to check if node it's ok.

    private:

        childrenMap children;
};

#endif // M_node_H
