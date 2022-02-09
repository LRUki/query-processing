#include "AdvancedDB2022Coursework1.hpp"
#include <iostream>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void printTuple(Tuple const &tuple);
void printRelation(const Relation &relation);
void quickSort(std::vector<Tuple const *> &ts);
int strCmp(const char *s1, const char *s2);

void DBMSImplementationForMarks::loadData(Relation const *large1,
					  Relation const *large2, // NOLINT(bugprone-easily-swappable-parameters)
					  Relation const *small)
{
	//**** sort large1, large2 by `a` ****//

	int large1Size = getNumberOfTuplesInRelation(*large1);
	int large2Size = getNumberOfTuplesInRelation(*large2);
	// preallocate memory
	large1Tuples.reserve(large1Size);
	large2Tuples.reserve(large2Size);
	// initalize `large1Tuples` and `large2Tuples` by assign pointers to the tuple
	for (int i = 0; i < MAX(large1Size, large2Size); i++)
	{
		if (i < large1Size)
		{
			Tuple const &t = large1->at(i);
			// filter out nullptr
			if (getAttributeValueType(t[0]) != 2 || getStringValue(t[0]) != nullptr)
			{
				large1Tuples.emplace_back(&t);
			}
		}
		if (i < large2Size)
		{
			Tuple const &t = large2->at(i);
			// filter out nullptr
			if (getAttributeValueType(t[0]) != 2 || getStringValue(t[0]) != nullptr)
			{
				large2Tuples.emplace_back(&t);
			}
		}
	}
	quickSort(large1Tuples);
	quickSort(large2Tuples);

	//**** build hashtable for `small` ****//

	// preallocate memory
	smallHashTable.reserve(small->size() * 2);
	// build the hashTable
	buildHashTable(smallHashTable, *small);

	// TODO: remove this after hashtable implemented
	this->small = small;
}

long DBMSImplementationForMarks::runQuery(long threshold)
{
	int sum = 0;
	int l1Idx = 0;
	int l2Idx = 0;

	int large1Size = large1Tuples.size();
	int large2Size = large2Tuples.size();

	while (l1Idx < large1Size && l2Idx < large2Size)
	{

		Tuple const &t1 = *large1Tuples[l1Idx];
		Tuple const &t2 = *large2Tuples[l2Idx];
		if (getAttributeValueType(t1[0]) == getAttributeValueType(t2[0]) && getAttributeValueType(t1[0]) == 2)
		{
			switch (strCmp(getStringValue(t1[0]), getStringValue(t2[0])))
			{
			case -1:
				l1Idx++;
				break;
			case 1:
				l2Idx++;
				break;
			default:
				// TODO: use `searchHashTable` instead
				for (Tuple const &t3 : *this->small)
				{
					if (getAttributeValueType(t3[0]) == 2 && strCmp(getStringValue(t1[0]), getStringValue(t3[0])) == 0)
					{
						if (getLongValue(t1[1]) + getLongValue(t2[1]) + getLongValue(t3[1]) > threshold)
						{
							sum += getLongValue(t1[2]) * getLongValue(t2[2]) * getLongValue(t3[2]);
						}
					}
				}
				l1Idx++;
				l2Idx++;
				break;
			}
		}
		else
		{
			if (t1[0] < t2[0])
			{
				l1Idx++;
			}
			else if (t1[0] > t2[0])
			{
				l2Idx++;
			}
			else
			{
				// TODO: use `searchHashTable` instead
				for (Tuple const &t3 : *this->small)
				{
					if (getAttributeValueType(t1[0]) == getAttributeValueType(t3[0]))
					{

						if (getAttributeValueType(t1[0]) == 0 && getLongValue(t1[0]) == getLongValue(t3[0]) ||
						    getAttributeValueType(t1[0]) == 1 && getDoubleValue(t1[0]) == getDoubleValue(t3[0]))
						{
							if (getLongValue(t1[1]) + getLongValue(t2[1]) + getLongValue(t3[1]) > threshold)
							{
								sum += getLongValue(t1[2]) * getLongValue(t2[2]) * getLongValue(t3[2]);
							}
						}
					}
				}
				l1Idx++;
				l2Idx++;
			}
		}
	}

	return sum;
}

// TODO: implement these functions
void DBMSImplementationForMarks::buildHashTable(std::vector<Tuple const *> &hashTable, Relation const &relation) {}
std::vector<Tuple const *> DBMSImplementationForMarks::searchHashTable(std::vector<Tuple const *> &hashTable, AttributeValue &key) {}

// ************************** helper functions ************************** //

inline void swap(Tuple *t1, Tuple *t2);
int strCmp(const char *s1, const char *s2);
int partition(std::vector<Tuple const *> &ts, int const &left, int const &right);

/**
 * @brief sorts the vector of `Tuple*` in place based on the value of `a` in an ascending order.
 * 	  variants are stored in the order of long -> double -> string order
 *
 * @param ts a vector of pointers to `Tuple`
 */
void quickSort(std::vector<Tuple const *> &ts)
{
	if (ts.size() == 0)
		return;

	std::vector<int> stack;
	stack.reserve(ts.size());

	stack.emplace_back(0);
	stack.emplace_back(ts.size() - 1);

	while (stack.size())
	{

		int r = stack.back();
		stack.pop_back();
		int l = stack.back();
		stack.pop_back();

		// Set pivot element at its correct position
		// in sorted array
		int pivotIndex = partition(ts, l, r);

		// If there are elements on left side of pivot,
		// then push left side to stack
		if (l < pivotIndex - 1)
		{
			stack.emplace_back(l);
			stack.emplace_back(pivotIndex - 1);
		}

		// If there are elements on right side of pivot,
		// then push right side to stack
		if (pivotIndex + 1 < r)
		{
			stack.emplace_back(pivotIndex + 1);
			stack.emplace_back(r);
		}
	}
}

inline void swap(Tuple *t1, Tuple *t2)
{
	Tuple *t = t1;
	t1 = t2;
	t2 = t;
}

/**
 * @brief compares string lexicographically
 *
 * @param s1
 * @param s2
 * @return 0 if s1 == s2, -1 if s1 < s2, 1 if s1 > s2
 */
int strCmp(const char *s1, const char *s2)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;

	while (*p1 && *p1 == *p2)
		++p1, ++p2;

	return (*p1 > *p2) - (*p2 > *p1);
}

int partition(std::vector<Tuple const *> &ts, int const &left, int const &right)
{
	AttributeValue pivot = ts[right]->at(0);
	int pivotType = getAttributeValueType(ts[right]->at(0));
	int i = (left - 1);

	for (int j = left; j < right; j++)
	{
		bool shouldSwap = false;
		if (pivotType == 2 && getAttributeValueType(ts[j]->at(0)) == 2)
		{
			shouldSwap = strCmp(getStringValue(ts[j]->at(0)), getStringValue(pivot)) <= 0;
		}
		else
		{
			shouldSwap = ts[j]->at(0) <= pivot;
		}

		if (shouldSwap)
		{
			i++;
			swap(ts[i], ts[j]);
		}
	}
	swap(ts[i + 1], ts[right]);
	return (i + 1);
}

void printTuple(Tuple const &tuple)
{
	for (const AttributeValue &v : tuple)
	{
		switch (getAttributeValueType(v))
		{
		case 0:
			std::cout << getLongValue(v) << " ,";
			break;
		case 1:
			std::cout << getDoubleValue(v) << " ,";
			break;
		default:
			const char *s = getStringValue(v);
			if (s == nullptr)
			{
				std::cout << "null"
					  << " ,";
				break;
			}
			std::cout << getStringValue(v) << " ,";
			break;
		}
	}
	std::cout << std::endl;
}

void printRelation(Relation const &relation)
{
	std::cout << "==============" << std::endl;
	for (Tuple const &t : relation)
	{
		printTuple(t);
	}
	std::cout << "==============" << std::endl;
}