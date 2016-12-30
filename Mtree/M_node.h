#ifndef M_NODE_H
#define M_NODE_H
#include "IndexObj.h"

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance<Data>,
	typename SplitFunction = functions::split_function<
	        functions::random_promotion,
	        functions::balanced_partition>>
class M_node : public IndexObj<Data,DistanceFunction,SplitFunction>
{
    public:


		typedef std::map<Data, IndexObj*> childrenMap;

        virtual ~M_node()
        {
            for(childrenMap::iterator i = children.begin(); i != children.end(); ++i)
            {
                auto child = i->second;
                delete child;
            }
        }

		size_t _check(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const {
			checkMinCapacity(mtree);
			_checkMaxCapacity(mtree);
			return 1;
		}

		void addData(const Data& data, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) throw(IndexObj<Data, DistanceFunction, SplitFunction>::SplitNodeReplace) // this class add a new data object to this Node, throws an exception if it's full
		{
			addDataConfirm(data,distance,mtree);
			checkMaxCapacity(mtree);
		}

		void checkMaxCapacity(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) throw(IndexObj<Data, DistanceFunction, SplitFunction>::SplitNodeReplace)
        {
            if(children.size() > mtree->maxNodeCapacity)  // Node in overflow
            {
                Partition p1;
                for(childrenMap::iterator i = children.begin(); i != children.end(); ++i)
                {
                    p1.insert(i->first);  // add all children objects to first patition
                }

                IndexObj<Data,DistanceFunction,SplitFunction>::distance_function_cached savedDistanceFunction(mtree->distance_function);

                Partition p2;
                promotedObjects promoted = mtree->split_function.operator()<Data, IndexObj<Data, DistanceFunction, SplitFunction>::distance_function_cached>(p1,p2,savedDistanceFunction); // get Promoted Nodes from split policy

                M_node<Data, DistanceFunction, SplitFunction>* newNodes[2];
                for(int i = 0; i < 2; ++i)
                {
                    Data& promotedObject = (i == 0)? promoted.first : promoted.second;
                    Partition& partition = (i == 0) ? p1 : p2;
                    M_node<Data, DistanceFunction, SplitFunction>* newNode = SplitNodeNew(promotedObject); // creating Node to throw with first partition data
                    for(Partition::iterator j = partition.begin(); j!= partition.end(); ++j)
                    {
                        const Data& data = *j;
                        IndexObj<Data, DistanceFunction, SplitFunction>* child = children[data];
                        children.erase(data);
                        double distance = savedDistanceFunction(promotedObject, data);
                        newNode->addChild(child, distance, mtree);
                    }

                    newNodes[i] = newNode;
                }
				throw IndexObj<Data, DistanceFunction, SplitFunction>::SplitNodeReplace(newNodes);
            }
        }

        virtual void addChild(IndexObj* child, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) = 0; // insert new Child node into this node
        virtual void removeData(const Data &data, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) throw (RootNodeReplacement, NodeUnderCapacity, DataNotFound) {}
		virtual size_t getMinCapacity(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const = 0;

		childrenMap children;
    protected:

        M_node(const Data& data):IndexObj(data){}
        M_node():IndexObj(*(Data*)(0)){std::cout << "This is never called";}
        // delete copy and move semantics

        virtual void addDataConfirm(const Data& data, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) = 0;
        virtual void removeDataConfirm(const Data& data, double distance, const Mtree<Data, DistanceFunction, SplitFunction>* mtree) throw (DataNotFound) = 0;

        virtual M_node<Data, DistanceFunction, SplitFunction>* SplitNodeNew(const Data&) const = 0;  // creates a new temporary Node containing promoted object and childs to Replace another node data

        void updateMetrics(IndexObj* child, double distance)  // preserves semantics of M-tree
        {
            child->distanceToParent = distance;
            updateRadius(child);
        }

        void updateRadius(IndexObj* child)
        {
            this->radius = std::max(this->radius, child->distanceToParent + child->radius);
        }

		virtual void checkMinCapacity(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const = 0;

        // functions to check if node it's ok.
	private:
		void _checkMaxCapacity(const Mtree<Data, DistanceFunction, SplitFunction>* mtree) const {
			if (children.size() > mtree->maxNodeCapacity)
				std::cerr << "error in max capacity" << std::endl;
		}
};

#endif // M_node_H
