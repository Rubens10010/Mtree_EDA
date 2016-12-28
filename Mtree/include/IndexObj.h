#ifndef INDEXOBJ_H
#define INDEXOBJ_H

#include <iterator>
#include <limits>
#include <map>
#include <queue>
#include <utility>
#include "functions.h"

template <
	typename Data,
	typename DistanceFunction = ::mt::functions::euclidean_distance,
	typename SplitFunction = ::mt::functions::split_function<
	        ::mt::functions::random_promotion,
	        ::mt::functions::balanced_partition
		>
>
class IndexObj
{
    public:
        IndexObj();
        virtual ~IndexObj();

        /* Copy and move semantics are not allowed for this object*/
        IndexObj() = delete;
        IndexObj(const IndexObj&) = delete;
        IndexObj(IndexObj&&) = delete;
        IndexObj& operator=(const IndexObj&) = delete;
        IndexObj& operator=(IndexObj&&) = delete;

        Data data;    // data used as reference for distanceToParent
        double radius;  // Covering radius
        double distanceToParent;

    protected:
        IndexObj(const Data& data) : data(data), radius(0), distanceToParent(-1){}
    private:
        // Functions to check if M tree semantics are always true
};

#endif // INDEXOBJ_H
