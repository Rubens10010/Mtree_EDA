#ifndef MTREE_H
#define MTREE_H

#include "ConcreteNodes.h"
#include <queue>

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

/* Esta es la clase M-tree, usa templates para obtener:
	@Data : Tipo de objeto a indexar
	@DistanceFunction : Funcion de distancia metrica
	@SplitFunction : Cuando ocurre un overflown en una hoja se llama a esta funcion para aplicar la politica correspondiente: promotion & partition*/

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

	// first double for minDistance, second double for distance To parent
	typedef std::pair<M_node<Data,DistanceFunction,SplitFunction>*, std::pair<double,double> > element;

	struct greater
	{
		bool operator()(const element& e1, const element &e2)
		{
			if (e1.second.first > e2.second.first)
				return true;
			return false;
		}
	};

	friend bool operator >(const std::pair<M_node<Data, DistanceFunction, SplitFunction>*,std::pair<double,double>> &n, double d)
	{
		if (n.second.first > d)
			return true;
		return false;
	}

	typedef std::custom_priority_queue<element, greater> pqueue;

    //using distance_function_cached = functions::f_distance_cached<Data,DistanceFunction>;

	// construye un M-tree con los parametros especificados o default
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

    /* Agrega un nuevo objeto del tipo @Data al arbol para ser indexado
	   Agrega los nuevos objetos a la raiz y asi recursivamente hasta llegar a las hojas
	   si la hoja donde es agregado se encuentra en overflown, lanzara una exepcion, si la excepcion llega hasta este punto
	   se creara una nueva raiz con los objetos dados por el nodo de remplazo temporal.
	*/

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


	/*
		Remueve el objeto indexado con el valor de @data, Todavia en desarrollo...
	*/

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

	// Verifica si la raiz del arbol esta cumpliendo las semanticas del M-tree
    void check() const
    {
        if(root != NULL)
        {
          root->check(this);
        }
    }

	// Realiza una consulta "Range Search" sobre los objetos del arbol dado un objeto Query @Q y un radio @rQ
	// Los objetos que cumplan los criterios, puedes ser vistos en el vector 'results' al finalizar la consulta
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

	// Funcion auxiliar que realiza el algoritmo RS del paper
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

	// Realiza una consulta "K nearest neightbors" sobre el M-tree, dado un objeto @Q y un numero @k
	// Los objetos obtenidos son colocados en 'KNNresults' y pueden ser visibles
	void knn_query(Data &Q, int k)
	{
		knn_search(root, Q, k);
	}

	// Funcion axuliar escoge el siguiente nodo mas cercano a el objeto Query
	M_node<Data, DistanceFunction, SplitFunction>* chooseNode(pqueue &priority_Q, double &distance_parent)
	{
		auto n = priority_Q.top();   // CHOOSE THE CLOSEST TO QUERY OBJECT
		distance_parent = n.second.second;
		priority_Q.pop();
		return n.first;
	}

	// Ejecuta iterativamente el algoritmo principal para encontrar los knn-neightbors
	void knn_search(M_node<Data, DistanceFunction, SplitFunction>* n_root, Data &queryObj, int k)
	{
		minimunQueue = pqueue();

		double distance = distance_function(queryObj, root->data);
		double minDistance = std::max(distance - root->radius, 0.0);

		element r = { n_root, {minDistance,distance} };	// maybe infinity
		minimunQueue.push(r);

		NN.resize(k);
		for (int i = 0; i < k; i++)
		{
			NN[i].first = Data{};
			NN[i].second = std::numeric_limits<double>::infinity();
		}

		while (!minimunQueue.empty())
		{
			M_node<Data, DistanceFunction, SplitFunction>* nextNode = chooseNode(minimunQueue, distance);		// root first
			knn_NodeSearch(nextNode, distance,queryObj, k);
		}
	}

	// Algoritmo principal de knn, puede ser encontrado en el paper, similar a rs.
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
						// store distance to parent with pointer in priority queue
						element toInsert = std::make_pair(dynamic_cast<M_node<Data,DistanceFunction,SplitFunction>*>(i->second),std::make_pair(childMinDistance,d));
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
				if(std::abs(dp - (i->second)->distanceToParent) <= dk /*+ (i->second)->radius*/)
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

	// funcion auxiliar permite actualizar la lista de candidatos obtenidos durante la ejecucion.
	double NN_update(std::pair<Data,double> &d, int k)
	{
		if (NN.size() > k)	// this should never happen
			std::cerr << "Result vector size violated" << std::endl;
		NNresult::iterator et = NN.begin();
		et = std::find_if(NN.begin(), NN.end(), [d](const std::pair<Data, double> &i) {
			if (i.second == d.second)
				return true;
			return false;
		});
		if (et == NN.end())
		{
			NN.push_back(d);
			std::sort(NN.begin(), NN.end(), [](std::pair<Data, double> &d1, std::pair<Data, double> &d2)
			{
				if (d1.second < d2.second)
					return true;
				return false;
			});

			if (NN.size() == k + 1)	// this should be the only case when new element is inserted
				NN.pop_back();
		}
		else {
			et->first = d.first;
		}
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