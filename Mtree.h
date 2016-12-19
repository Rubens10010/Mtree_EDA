#pragma once
#include "RouterNode.h"
#include "LeafNode.h"
#include <queue>

using setObjetos = std::vector<_object*>;
using queryResult = std::vector<Entry>;

template <typename splitp, typename distanceF>
class Mtree
{
public:
	Mtree(int m, splitp policy, distanceF D);
	~Mtree();

	void insert(Entry &e);
	double getDistanceFor(_object &N1, _object &N2);
	void setSplitPolicy(splitp policy);

	queryResult rangeQuery(_object &Q, double SR);
	queryResult knnQuery(_object &Q, int k);


protected:

	void insert(Node *&N, _object &entry, double dp);
	void split(Node *&N, _object &On);
	void promote(setObjetos &obj, _object &Op1, _object &Op2);
	void partition(setObjetos &obj, _object &Op1, _object &Op2, setObjetos &N1, setObjetos &N2);

	queryResult RS(Node *&N, _object &Q, double SR);
	queryResult K_nn_Search(Node *&r, _object &Q, int k);
	Node* chooseNode(std::queue<Node&> &pq);	// should be priority queue
	void K_nn_NodeSearch(Node *&next, _object &Q, int k);
	double nn_Update(queryResult &Knn);	// nnArray

private:

	distanceF Distance_function;
	splitp split_policy;
	Node* root;
	int M;
};

