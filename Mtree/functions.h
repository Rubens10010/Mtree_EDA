#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <algorithm>
#include <iterator>
#include <set>
#include <utility>
#include <vector>

namespace functions
{
	template <typename Sequence>
	struct euclidean_distance {
		//template <typename Sequence>
		double operator()(const Sequence& data1, const Sequence& data2) const {
			double distance = 0;
			for (auto i1 = data1.begin(), i2 = data2.begin(); i1 != data1.end() && i2 != data2.end(); ++i1, ++i2) {
				double diff = *i1 - *i2;
				distance += diff * diff;
			}
			distance = sqrt(distance);
			return distance;
		}
	};

	struct random_promotion {
		template <typename Data, typename DistanceFunction>
		std::pair<Data, Data> operator()(const std::set<Data>& data_objects, DistanceFunction& distance_function) const {
			std::vector<Data> promoted;
			//random_sample_n(data_objects.begin(), data_objects.end(), inserter(promoted, promoted.begin()), 2);
			assert(promoted.size() == 2);
			return { promoted[0], promoted[1] };
		}
	};

	struct balanced_partition{
		template <typename Data, typename DistanceFunction>
		void operator()(const std::pair<Data, Data>& promoted,
			std::set<Data>& first_partition,
			std::set<Data>& second_partition,
			DistanceFunction& distance_function
			) const
		{
			std::vector<Data> queue1(first_partition.begin(), first_partition.end());
			// Sort by distance to the first promoted data
			std::sort(queue1.begin(), queue1.end(),
				[&](const Data& data1, const Data& data2) {
				double distance1 = distance_function(data1, promoted.first);
				double distance2 = distance_function(data2, promoted.first);
				return distance1 < distance2;
			}
			);

			std::vector<Data> queue2(first_partition.begin(), first_partition.end());
			// Sort by distance to the second promoted data
			std::sort(queue2.begin(), queue2.end(),
				[&](const Data& data1, const Data& data2) {
				double distance1 = distance_function(data1, promoted.second);
				double distance2 = distance_function(data2, promoted.second);
				return distance1 < distance2;
			}
			);

			first_partition.clear();

			typename std::vector<Data>::iterator i1 = queue1.begin();
			typename std::vector<Data>::iterator i2 = queue2.begin();

			while (i1 != queue1.end() || i2 != queue2.end()) {
				while (i1 != queue1.end()) {
					Data& data = *i1;
					++i1;
					if (second_partition.find(data) == second_partition.end()) {
						first_partition.insert(data);
						break;
					}
				}

				while (i2 != queue2.end()) {
					Data& data = *i2;
					++i2;
					if (first_partition.find(data) == first_partition.end()) {
						second_partition.insert(data);
						break;
					}
				}
			}
		}
	};

	template <typename PromotionFunction, typename PartitionFunction>
	struct split_function{
		typedef PromotionFunction promotion_function_type;

		/** */
		typedef PartitionFunction partition_function_type;

		PromotionFunction promotion_function;
		PartitionFunction partition_function;

		/** */
		explicit split_function(
			PromotionFunction promotion_function = PromotionFunction(),
			PartitionFunction partition_function = PartitionFunction()
		)
			: promotion_function(promotion_function),
			partition_function(partition_function)
		{}

		template <typename Data, typename DistanceFunction>
		std::pair<Data, Data> operator()(
			std::set<Data>& first_partition,
			std::set<Data>& second_partition,
			DistanceFunction& distance_function
			) const
		{
			std::pair<Data, Data> promoted = promotion_function(first_partition, distance_function);
			partition_function(promoted, first_partition, second_partition, distance_function);
			return promoted;
		}
	};

	template <typename Data, typename DistanceFunction>
	class f_distance_cached{
	public:
		explicit f_distance_cached(const DistanceFunction& distance_function)
			: distance_function(distance_function)
		{}

		double operator()(const Data& data1, const Data& data2) {
			typename CacheType::iterator i = cache.find(make_pair(data1, data2));
			if (i != cache.end()) {
				return i->second;
			}

			i = cache.find(make_pair(data2, data1));
			if (i != cache.end()) {
				return i->second;
			}

			// Not found in cache
			double distance = distance_function(data1, data2);

			// Store in cache
			cache.insert(make_pair(make_pair(data1, data2), distance));
			cache.insert(make_pair(make_pair(data2, data1), distance));

			return distance;
		}

	private:
		typedef std::map<std::pair<Data, Data>, double> CacheType;

		const DistanceFunction& distance_function;
		CacheType cache;
	};
}

#endif // !FUNCTIONS_H


