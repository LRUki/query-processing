#include "AdvancedDB2022Coursework1.hpp"
#include <iostream>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ROUND(x) (int)(x + 0.5)

void printTuple(Tuple const &tuple);
void printRelation(const Relation &relation);
int strCmp(const char *s1, const char *s2);
int compareVariants(const void *p1, const void *p2);
int compareTuple(const void *p1, const void *p2);

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
			Tuple *tt = (Tuple *)&large1->at(i);
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

	if (large1Tuples.size() > 0)
	{
		qsort(&large1Tuples[0], large1Tuples.size(), sizeof(Tuple *), compareTuple);
	}
	if (large2Tuples.size() > 0)
	{
		qsort(&large2Tuples[0], large2Tuples.size(), sizeof(Tuple *), compareTuple);
	}

	//**** build hashtable for `small` ****//

	// preallocate memory and set to nullptr
	smallHashTable = std::vector<Tuple const *>(small->size(), nullptr);
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

		switch (compareVariants(&t1[0], &t2[0]))
		{
		case -1:
			l1Idx++;
			break;
		case 1:
			l2Idx++;
			break;
		default:
			// t1 and t2 matches, serach small
			//  TODO: use `searchHashTable` instead
			for (Tuple const &t3 : *this->small)
			{
				if (compareVariants(&t1[0], &t3[0]) == 0)
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

	return sum;
}

// TODO: implement these functions
void DBMSImplementationForMarks::buildHashTable(std::vector<Tuple const *> &hashTable, Relation const &relation) {}
std::vector<Tuple const *> DBMSImplementationForMarks::searchHashTable(std::vector<Tuple const *> &hashTable, AttributeValue &key) {}

// ************************** helper functions ************************** //

inline void swap(Tuple *t1, Tuple *t2);
int strCmp(const char *s1, const char *s2);
int partition(std::vector<Tuple const *> &ts, int const &left, int const &right);

// returns <0 if p1 goes before p2, >0 if p2 goes before p1, 0 if equal
int compareVariants(const void *p1, const void *p2)
{
	AttributeValue &v1 = *(AttributeValue *)p1;
	AttributeValue &v2 = *(AttributeValue *)p2;

	if (getAttributeValueType(v1) == getAttributeValueType(v2) && getAttributeValueType(v1) != 0)
	{
		if (getAttributeValueType(v1) == 1)
		{
			// double
			return ROUND(getDoubleValue(v1)) - ROUND(getDoubleValue(v2));
		}
		else
		{
			// string
			return strCmp(getStringValue(v1), getStringValue(v2));
		}
	}

	return (v1 < v2) ? -1 : ((v1 > v2) ? 1 : 0);
}

int compareTuple(const void *p1, const void *p2)
{
	Tuple *t1 = *(Tuple **)p1;
	Tuple *t2 = *(Tuple **)p2;
	return compareVariants(&t1->at(0), &t2->at(0));
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