#pragma once

#include <BasicGlobal.h>

namespace bsc
{
	template<typename Iterator>
	inline void swap_value(const Iterator a_it, const Iterator b_it)
	{
		auto temp = *a_it;
		*a_it = *b_it;
		*b_it = temp;
	}

	// --------------------------------------------- SORTING ---------------------------------------------

	/*
	 *	SELECTION SORT
	 *
	 *	Iterates through the values and selects the minimum and moves it to the front
	 *
	 *	avg time: O(n^2)
	 *
	 */
	template<typename Iterator>
	void selection_sort(const Iterator begin_it, const Iterator end_it)
	{
		Iterator next_min_it = begin_it;

		while (next_min_it != end_it)
		{
			Iterator min_it = next_min_it;
			Iterator it = next_min_it;

			while (it != end_it)
			{
				if (*it < *min_it)
				{
					min_it = it;
				}
				++it;
			}

			swap_value(min_it, next_min_it);

			++next_min_it;
		}
	}


	/*
	 *	BUBBLE SORT
	 *
	 *	Swaps adjacent values if they are not sorted. Iterates through the list until it's sorted
	 *
	 *	avg time: O(n^2)
	 *	
	 */
	template<typename Iterator>
	void bubble_sort(const Iterator begin_it, const Iterator end_it)
	{
		bool swap = true;

		while (swap)
		{
			swap = false;
			Iterator it = begin_it;
			Iterator prev_it = it;

			while (it != end_it)
			{
				if (it != prev_it)
				{
					if (*it < *prev_it)
					{
						swap_value(it, prev_it);
						swap = true;
					}
				}

				prev_it = it;
				++it;
			}
		}
	}


	/*
	 *	MERGE SORT
	 *
	 *	Divides the n values in n sublists, merge two sublist to produce an ordered sublist untile there is none left
	 *
	 *	avg time: O(n log n)
	 *
	 */
	template<typename Iterator>
	void merge(const Iterator a_begin, const Iterator mid, const Iterator b_end)
	{
		if (a_begin == mid || mid == b_end)
		{
			return;
		}

		Iterator a_it = a_begin;
		Iterator b_begin = mid;

		// In place merge of the two ordered lists of items
		// Swap element of a with smaller element of b if it's greater, then 
		while (a_it != mid && b_begin != b_end)
		{
			if (*a_it < *b_begin)
			{	
				++a_it;
			}
			else
			{
				swap_value(a_it, b_begin);

				auto b0 = *b_begin;
				Iterator b_it = b_begin;

				// Move the new first val of b to its correct position to maintain the order
				while (b_it != b_end && *b_it < b0)
				{
					*(b_it - 1) = *b_it;
					++b_it;
				}

				*(b_it - 1) = b0;
			}

			
		}
	}

	template<typename Iterator>
	void merge_sort(const Iterator begin_it, const Iterator end_it)
	{
		auto size = std::distance(begin_it, end_it);

		if (size <= 1)
		{
			return;
		}

		Iterator mid = std::next(begin_it, size / 2);

		merge_sort(begin_it, mid);
		merge_sort(mid, end_it);

		merge(begin_it, mid, end_it);
	}


	/*
	 *	QUICK SORT
	 *
	 *	Pick a pivot (the last element for the Lomuto partition scheme).
	 *	Re-order the element of the array so that the smaller are before the pivot and the other after
	 *	The pivot is in its final position. Recursively apply this to the subset before and after the pivot.
	 *
	 *	avg time: O(n log n)
	 *
	 */
	template<typename Iterator>
	Iterator partition(const Iterator begin_it, const Iterator end_it)
	{
		Iterator high_it = end_it - 1; // end_it the element after the last one
		auto pivot = *high_it;
		auto i_it = begin_it;
		auto j_it = begin_it;

		while (j_it < high_it)
		{
			if (*j_it < pivot)
			{
				swap_value(i_it, j_it);

				++i_it;
			}
			++j_it;
		}

		*high_it = *i_it;
		*i_it = pivot;

		return i_it;
	}

	template<typename Iterator>
	void quick_sort(const Iterator begin_it, const Iterator end_it)
	{
		if (begin_it < end_it)
		{
			Iterator pivot_it = partition(begin_it, end_it);

			quick_sort(begin_it, pivot_it);
			quick_sort(++pivot_it, end_it);
		}
	}

	
	/*
	 *	BIT RADIX SORT
	 *
	 *  MSD (Most Significant Digit) sorting
	 *
	 *	avg time: O(k n)
	 *
	 */
	template<typename Iterator>
	void bit_sort(const Iterator begin_it, const Iterator end_it, const uint bit_index, const bool sign_bit = false)
	{
		if (begin_it == end_it)
		{
			return;
		}

		Iterator it_zero = begin_it;
		Iterator it_one = end_it;

		while (it_zero < it_one)
		{
			// getting the bit value
			bool zero_val = ((*it_zero) & (1u << bit_index)) == 0u;

			if (!sign_bit && !zero_val || sign_bit && zero_val )
			{
				--it_one;
				swap_value(it_zero, it_one);
			}
			else
			{
				++it_zero;		
			}
		}

		if (bit_index > 0)
		{
			Iterator boundary_it = it_one;
			bit_sort(begin_it, boundary_it, bit_index - 1);

			if (boundary_it < end_it - 1)
			{
				bit_sort(boundary_it, end_it, bit_index - 1);
			}
		}
	}

	template<typename Iterator>
	void bit_radix_sort(const Iterator begin_it, const Iterator end_it, const bool consider_sign = true)
	{
		if (begin_it == end_it)
		{
			return;
		}

		const auto num_bits = sizeof(*begin_it) * CHAR_BIT;
		uint bit_index = num_bits - 1;

		bit_sort(begin_it, end_it, bit_index, consider_sign);
	}


	// --------------------------------------------- SEARCH ---------------------------------------------
	

	/*
	 *	BINARY SEARCH
	 *
	 *	avg time: O(log n)
	 *
	 */
	template<typename Container>
	typename Container::const_iterator binary_search(const Container& items, const typename Container::value_type& target)
	{
		const auto size = items.size();

		if (size == 0)
		{
			return items.end();
		}

		auto start = items.begin();
		auto end = items.end() - 1;

		while (start <= end)
		{
			auto middle = start + (end - start) / 2;

			auto middle_val = *middle;

			if (middle_val == target)
			{
				return middle;
			}
			else if (target < middle_val)
			{
				end = middle - 1;
			}
			else
			{
				start = middle + 1;
			}
		}

		return items.end();
	}

} // namespace bsc