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

int main()
{
    typedef Mtree<
		Data,
		functions::euclidean_distance<Data>,
		functions::split_function<
		PromotionFunction,
		functions::balanced_partition
		>
	>
	MTree;

	MTree arbol(2, 3);
	Data d = { 1,2,3,4,5,6,7,8,9 };
	Data d2 = { 9,8,7,6,5,4,3,2,1 };
	Data d3 = { 2,2,3,4,4,6,8,8,9 };
	Data d4 = { 1,1,1,4,9,6,9,8,9 };

	arbol.add(d);
	arbol.add(d2);
	arbol.add(d3);
	arbol.add(d4);

	arbol.root->data;

	for (int i = 0; i < arbol.root->data.size(); i++)
	{
		std::cout << arbol.root->data[i] << std::endl;
	}
	int x;
	//std::cin >> x;
    return 0;
}
