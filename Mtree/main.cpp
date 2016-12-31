#include <iostream>
#include "Mtree.h"

using namespace std;

typedef vector<int> Data;
typedef set<Data> DataSet;
typedef functions::f_distance_cached<Data, functions::euclidean_distance<Data>> CachedDistanceFunction;
typedef pair<Data, Data>(*PromotionFunction)(const DataSet&, CachedDistanceFunction&);

PromotionFunction nonRandomPromotion =
[](const DataSet& dataSet, CachedDistanceFunction&) -> pair<Data, Data> {
	vector<Data> dataObjects(dataSet.begin(), dataSet.end());
	sort(dataObjects.begin(), dataObjects.end());
	return { dataObjects.front(), dataObjects.back() };
};

void print_Data(Data& d)
{
	for (auto i : d)
	{
		std::cout << i << std::endl;
	}
}

int main()
{
    typedef Mtree<
		Data,
		functions::euclidean_distance<Data>,
		functions::split_function<
		functions::random_promotion,
		functions::Generalized_Hyperplane
		>
	>
	MTree;

	MTree arbol(2, 3);
	Data d2 = {0,0};
	Data d3 = {1,1 };
	Data d4 = {2,1};
	Data d5 = {3,5};
	Data d = { 4,6 };
	Data d6 = { 5,8 };
	Data d7 = { 7,3};
	Data d8 = { 9,3};

	arbol.add(d2);
	arbol.add(d3);
	arbol.add(d4);
	arbol.add(d5);
	arbol.add(d6);
	arbol.add(d7);
	arbol.add(d8);
	arbol.add(d);

	arbol.root->data;

	print_Data(arbol.root->data);
	
	Data q = {5,5};
	arbol.rs(q, 3);

	std::cout << "Resultados\n";
	for (int i = 0; i < arbol.results.size(); i++)
	{
		std::cout << i <<": "<< std::endl;
		print_Data(arbol.results[i]);
	}

	arbol.knn_query(q, 4);
	std::cout << "\nKNN\n";
	for (int i = 0; i < arbol.NN.size(); i++)
	{
		std::cout << i << ": distance =  " << arbol.NN[i].second << std::endl;
		print_Data(arbol.NN[i].first);
	}

    return 0;
}
