#ifndef MTREE_H
#define MTREE_H

#include "ConcreteNodes.h"
#include <queue>
//#include <functional>

// used as priority queue for KNN search
namespace std {
	template<typename T, typename _Pr>
	class custom_priority_queue : public std::priority_queue<T, std::vector<T>, _Pr>
	{
	public:

		void remove_if_Less(const double& value) {
			this->c.erase(std::remove_if(this->c.begin(), this->c.end(), [value](T &d1) {
				if (d1 > value)
					return true;
				return false;
			}), this->c.end());
		}
	};
};

template <
	typename Data,
	typename DistanceFunction = functions::euclidean_distance<Data>,
	typename SplitFunction = functions::split_function<
	       functions::random_promotion,
	       functions::balanced_partition>>
class Mtree
{
  public:
    using distance_function_type = DistanceFunction;
    using split_function_policy = SplitFunction;

	typedef std::vector<Data> result;
	typedef std::vector<std::pair<Data, double>> NNresult;
	typedef std::pair<M_node<Data,DistanceFunction,SplitFunction>*, double> element;
	typedef bool(*greater_comp)(const element&, const element&);

	struct greater
	{
		bool operator()(const element& e1, const element &e2)
		{
			if (e1.second > e2.second)
				return true;
			return false;
		}
	};

	friend bool operator >(const std::pair<M_node<Data, DistanceFunction, SplitFunction>*,double> &n, double d)
	{
		if (n.second > d)
			return true;
		return false;
	}
	//typedef std::priority_queue<element, std::vector<element>,greater> pqueue;
	typedef std::custom_priority_queue<element, greater> pqueue;

    //using distance_function_cached = functions::f_distance_cached<Data,DistanceFunction>;

	explicit Mtree(size_t min_capacity = DEFAULT_MIN_NODE_CAPACITY, size_t max_capacity = -1, const DistanceFunction& distance_func = DistanceFunction{},
		const SplitFunction& split_function = SplitFunction{})
    :minNodeCapacity(min_capacity),maxNodeCapacity(max_capacity),distance_function(distance_func), split_function(split_function), root(NULL)
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
            root = new RootLeafNode<Data, DistanceFunction, SplitFunction>(data);
            root->addData(data,0,this);
        }
        else{
          double distance = distance_function(data,root->data);
          try{
            root->addData(data, distance, this);
          }catch(IndexObj<Data, DistanceFunction, SplitFunction>::SplitNodeReplace &e)
          {
            // creates new root to insert new nodes to replace
            M_node<Data, DistanceFunction, SplitFunction>* nRoot = new RootNode<Data, DistanceFunction, SplitFunction>(root->data);
            delete root;
            root = nRoot;
            for(int i= 0; i < e.NUM_NODES; i++)
            {
                M_node<Data, DistanceFunction, SplitFunction>* nNode = e.newNodes[i];
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

	void rs(Data &Q, double rQ)
	{
		results.clear();
		Data aux = Data{};

		// temp
		double distance =distance_function(Q, root->data);
		double minDistance = std::max(distance - root->radius, 0.0);

		rs_aux(root, aux, Q, distance, rQ);
		std::cout << "Number of results: " << results.size() << std::endl;
	}

	void rs_aux(M_node<Data, DistanceFunction, SplitFunction>* n, Data &Op, Data &Q, double dp, double rQ)
	{
		LeafNode<Data, DistanceFunction, SplitFunction>* N = dynamic_cast<LeafNode<Data, DistanceFunction, SplitFunction>*>(n);
		if (N == NULL)
		{
			for (auto i = n->children.begin(); i != n->children.end(); ++i)
			{
				//if (std::abs(distance_function(Op, Q) - (i->second)->distanceToParent) <= rQ + (i->second)->radius)
				if (std::abs(dp - (i->second)->distanceToParent) <= rQ + (i->second)->radius)
				{
					double d = distance_function((i->second)->data, Q);		// this should be std::abs(distance_function(Op, Q)
					double childMinDistance = std::max(d - (i->second)->radius, 0.0);
					if (childMinDistance <= rQ )
					{
						rs_aux(dynamic_cast<M_node<Data,DistanceFunction,SplitFunction>*>(i->second), n->data, Q, d, rQ);
					}
				}
			}

		}
		else
		{
			for (auto i = N->children.begin(); i != N->children.end(); ++i)
			{
				//if (std::abs(distance_function(Op, Q) - (i->second)->distanceToParent) <= rQ)
				if (std::abs(dp - (i->second)->distanceToParent) <= rQ + (i->second)->radius)
				{
					// distance between entry and query object
					double d = distance_function(i->first, Q);
					if (d <= rQ)
					{
						Entry<Data, DistanceFunction, SplitFunction>* r = dynamic_cast<Entry<Data, DistanceFunction, SplitFunction>*>(i->second);
						if (r == NULL)
							std::cerr << "Error while computing result" << std::endl;
						results.push_back(r->data);
					}
				}
			}
		}
	}

	void knn_query(Data &Q, int k)
	{
		knn_search(root, Q, k);
	}

	M_node<Data, DistanceFunction, SplitFunction>* chooseNode(pqueue &priority_Q)
	{
		auto n = priority_Q.top();
		priority_Q.pop();
		return n.first;
	}

	void knn_search(M_node<Data, DistanceFunction, SplitFunction>* n_root, Data &queryObj, int k)
	{
		minimunQueue = pqueue();

		element r = { n_root, 0 };	// maybe infinity
		minimunQueue.push(r);

		NN.resize(k);
		for (int i = 0; i < k; i++)
		{
			NN[i].first = Data{};
			NN[i].second = std::numeric_limits<double>::infinity();
		}

		double distance = distance_function(queryObj, root->data);		// distance of Op(parent of current node) to Q
		while (!minimunQueue.empty())
		{
			M_node<Data, DistanceFunction, SplitFunction>* nextNode = chooseNode(minimunQueue);		// root first
			//nextNode->distanceToParent;
			distance = knn_NodeSearch(nextNode, distance,queryObj, k);
		}
	}

	double knn_NodeSearch(M_node<Data, DistanceFunction, SplitFunction>* n, double dp, Data &Q, int k)
	{
		double d;
		LeafNode<Data, DistanceFunction, SplitFunction>* N = dynamic_cast<LeafNode<Data, DistanceFunction, SplitFunction>*>(n);
		if (N == NULL)	// if N is not a leaf then
		{
			for (auto i = n->children.begin(); i != n->children.end(); ++i)	// For all Or(i) in N
			{
				//if (std::abs(distance_function(Op, Q) - (i->second)->distanceToParent) <= rQ + (i->second)->radius)
				double dk = NN[k-1].second;		// kth element
				if (std::abs(dp - (i->second)->distanceToParent) <= dk + (i->second)->radius)
				{
					d = distance_function((i->second)->data, Q);		// compute d(Or,Q)
					double childMinDistance = std::max(d - (i->second)->radius, 0.0);
					if (childMinDistance <= dk)	//dmin(T(Or))<=dk
					{
						element toInsert = std::make_pair(dynamic_cast<M_node<Data,DistanceFunction,SplitFunction>*>(i->second),childMinDistance);
						minimunQueue.push(toInsert);	// add [ptr(T(Or)),dmin(T(Or))] to PR
						double dmax = d + (i->second)->radius;
						if (dmax < dk)
						{
							std::pair<Data, double> temp = { Data{}, dmax };
							dk = NN_update(temp,k);	// perform an ordered insertion in the NN array and recives a new value of dk
							// remove from PR all entries for which childMinDistance > dk
							minimunQueue.remove_if_Less(dk);
						}
					}
				}
			}
		}
		else
		{
			for (auto i = N->children.begin(); i != N->children.end(); ++i)
			{
				double dk = NN[k-1].second;
				if(std::abs(dp - (i->second)->distanceToParent) <= dk)
				{
					// distance between entry and query object
					d = distance_function(i->first, Q);
					if (d <= dk)
					{
						Entry<Data, DistanceFunction, SplitFunction>* r = dynamic_cast<Entry<Data, DistanceFunction, SplitFunction>*>(i->second);
						std::pair<Data, double> temp = { r->data, d};
						dk = NN_update(temp,k);	// perform an ordered insertion in the NN array and recives a new value of dk, should return id of object
						// remove from PR all entries for which childMinDistance > dk
						minimunQueue.remove_if_Less(dk);
					}
				}
			}
		}
		return d;
	}

	double NN_update(std::pair<Data,double> &d, int k)
	{
		if (NN.size() > k)	// this should never happen
			std::cerr << "Result vector size violated" << std::endl;
		NN.push_back(d);
		std::sort(NN.begin(), NN.end(), [](std::pair<Data, double> &d1, std::pair<Data, double> &d2)
		{
			if(d1.second < d2.second)
				return true;
			return false;
		});

		if (NN.size() == k + 1)	// this should be the only case when new element is inserted
			NN.pop_back();
		return NN[k-1].second;
	}

    size_t minNodeCapacity;
    size_t maxNodeCapacity;
    M_node<Data, DistanceFunction, SplitFunction>* root;
    DistanceFunction distance_function;
    SplitFunction split_function;

	result results;
	NNresult NN;
	pqueue minimunQueue;

	enum {
		/**
		* @brief The default minimum capacity of nodes in an M-Tree, when not
		* specified in the constructor call.
		*/
		DEFAULT_MIN_NODE_CAPACITY = 50
	};

};

#endif