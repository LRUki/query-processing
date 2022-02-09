#include "AdvancedDB2022Coursework1.hpp"
#include <iostream>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void printTuple(const Tuple &tuple);
void printRelation(const Relation &relation);
void quickSort(std::vector<const Tuple *> &ts);
int strCmp(const char *s1, const char *s2);

void DBMSImplementationForMarks::loadData(Relation const *large1,
					  Relation const *large2, // NOLINT(bugprone-easily-swappable-parameters)
					  Relation const *small) {
	this->large1 = large1;
	this->large2 = large2;
	this->small = small;

	// initialize sortedL1Indices, sortedL12ndices
	// and sort them based on `a`
	sortLarges();

	//build hash table
	buildHashTable();
}

long DBMSImplementationForMarks::runQuery(long threshold) {
	int sum = 0;
	int l1Idx = 0;
	int l2Idx = 0;

	int l1Size = sortedL1Indices.size();
	int l2Size = sortedL2Indices.size();

	while (l1Idx < l1Size && l2Idx < l2Size) 	{

		const Tuple t1 = *sortedL1Indices[l1Idx];
		const Tuple t2 = *sortedL2Indices[l2Idx];
		if (getAttributeValueType(t1[0]) == getAttributeValueType(t2[0]) && getAttributeValueType(t1[0]) == 2) 		{
			switch (strCmp(getStringValue(t1[0]), getStringValue(t2[0]))) 			{
			case -1:
				l1Idx++;
				break;
			case 1:
				l2Idx++;
				break;
			default:
				//TODO: use hash instead
				for (const Tuple &t3 : *this->small) {
					if (getAttributeValueType(t3[0]) == 2 && strCmp(getStringValue(t1[0]), getStringValue(t3[0])) == 0) {
						if (getLongValue(t1[1]) + getLongValue(t2[1]) + getLongValue(t3[1]) > threshold) {
							sum += getLongValue(t1[2]) * getLongValue(t2[2]) * getLongValue(t3[2]);
						}
					}
				}
				l1Idx++;
				l2Idx++;
				break;
			}
		}
		else {
			if (t1[0] < t2[0]) {
				l1Idx++;
			} else if (t1[0] > t2[0]) {
				l2Idx++;
			} else {
				//TODO: use hash instead
				for (const Tuple &t3 : *this->small) {
					if (getAttributeValueType(t1[0]) == getAttributeValueType(t3[0])) {

						if (getAttributeValueType(t1[0]) == 0 && getLongValue(t1[0]) == getLongValue(t3[0]) ||
						    getAttributeValueType(t1[0]) == 1 && getDoubleValue(t1[0]) == getDoubleValue(t3[0])) {
							if (getLongValue(t1[1]) + getLongValue(t2[1]) + getLongValue(t3[1]) > threshold) {
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

void DBMSImplementationForMarks::sortLarges() {
	int l1Size = getNumberOfTuplesInRelation(*this->large1);
	int l2Size = getNumberOfTuplesInRelation(*this->large2);

	sortedL1Indices.reserve(l1Size);
	sortedL2Indices.reserve(l2Size);

	//initialize
	for (int i = 0; i < MAX(l1Size, l2Size); i++) {
		if (i < l1Size) {
			const Tuple *t = &this->large1->at(i);
			//filter out nullptr
			if (getAttributeValueType(t->at(0)) != 2 || getStringValue(t->at(0)) != nullptr) {
				sortedL1Indices.emplace_back(t);
			}
		}
		if (i < l2Size) {
			const Tuple *t = &this->large2->at(i);
			//filter out nullptr
			if (getAttributeValueType(t->at(0)) != 2 || getStringValue(t->at(0)) != nullptr) {
				sortedL2Indices.emplace_back(t);
			}
		}
	}
	quickSort(sortedL1Indices);
	quickSort(sortedL2Indices);

	// 	sortedL1Indices.reserve(l1Size);
	// //initialize
	// for (int i = 0; i < l1Size; i++) {
	// 	const Tuple *t = &this->large1->at(i);
	// 	//filter out nullptr
	// 	if (getAttributeValueType(t->at(0)) != 2 || getStringValue(t->at(0)) != nullptr) {
	// 		sortedL1Indices.emplace_back(t);
	// 	}
	// }
	// quickSort(sortedL1Indices);

	// sortedL2Indices.reserve(l2Size);
	// //initialize
	// for (int i = 0; i < l2Size; i++) {
	// 	const Tuple *t = &this->large2->at(i);
	// 	//filter out nullptr
	// 	if (getAttributeValueType(t->at(0)) != 2 || getStringValue(t->at(0)) != nullptr) {
	// 		sortedL2Indices.emplace_back(t);
	// 	}
	// }
	// quickSort(sortedL2Indices);
}
//TODO: implement
void DBMSImplementationForMarks::buildHashTable() {}

// ************************** helper functions ************************** //

inline void swap(Tuple *t1, Tuple *t2);
int strCmp(const char *s1, const char *s2);
int partition(std::vector<const Tuple *> &ts, int &left, int &right);
/**
 * @brief sorts the vector of `Tuple*` in place based on the value of `a` in an ascending order.
 * 	  variants are stored in the order of long -> double -> string order
 * 
 * @param ts a vector of pointers to `Tuple`
 */
void quickSort(std::vector<const Tuple *> &ts) {
	if (ts.size() == 0)
		return;
	// TODO: replace array with vector to store on the heap if stackoverflows?
	int stack[ts.size()];

	int top = -1;
	stack[++top] = 0;
	stack[++top] = ts.size() - 1;

	while (top >= 0) {

		// Pop h and l
		int r = stack[top--];
		int l = stack[top--];

		// Set pivot element at its correct position
		// in sorted array
		int pivotIndex = partition(ts, l, r);

		// If there are elements on left side of pivot,
		// then push left side to stack
		if (l < pivotIndex - 1) {
			stack[++top] = l;
			stack[++top] = pivotIndex - 1;
		}

		// If there are elements on right side of pivot,
		// then push right side to stack
		if (pivotIndex + 1 < r) {
			stack[++top] = pivotIndex + 1;
			stack[++top] = r;
		}
	}
}

// void quickSort(std::vector<const Tuple *> &ts) {
// 	if (ts.size() == 0)
// 		return;
// 	// TODO: replace array with vector to store on the heap if stackoverflows?
// 	// int stack[ts.size()];
// 	std::vector<int> stack;

// 	// int top = -1;
// 	// stack[++top] = 0;
// 	// stack[++top] = ts.size() - 1;
// 	stack.emplace_back(0);
// 	stack.emplace_back(ts.size() - 1);

// 	while (stack.size()) {

// 		// Pop h and l
// 		// int r = stack[top--];
// 		// int l = stack[top--];
// 		int r = stack.back();
// 		stack.pop_back();
// 		int l = stack.back();
// 		stack.pop_back();

// 		// Set pivot element at its correct position
// 		// in sorted array
// 		int pivotIndex = partition(ts, l, r);

// 		// If there are elements on left side of pivot,
// 		// then push left side to stack
// 		if (l < pivotIndex - 1) {
// 			stack.emplace_back(l);
// 			stack.emplace_back(pivotIndex - 1);
// 		}

// 		// If there are elements on right side of pivot,
// 		// then push right side to stack
// 		if (pivotIndex + 1 < r) {
// 			stack.emplace_back(pivotIndex + 1);
// 			stack.emplace_back(r);
// 		}
// 	}
// }

inline void swap(Tuple *t1, Tuple *t2) {
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
int strCmp(const char *s1, const char *s2) {
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;

	while (*p1 && *p1 == *p2)
		++p1, ++p2;

	return (*p1 > *p2) - (*p2 > *p1);
}

int partition(std::vector<const Tuple *> &ts, int &left, int &right) {
	AttributeValue pivot = ts[right]->at(0);
	int pivotType = getAttributeValueType(ts[right]->at(0));
	int i = (left - 1);

	for (int j = left; j < right; j++) {
		bool shouldSwap = false;
		if (pivotType == 2 && getAttributeValueType(ts[j]->at(0)) == 2) {
			shouldSwap = strCmp(getStringValue(ts[j]->at(0)), getStringValue(pivot)) <= 0;
		} else {
			shouldSwap = ts[j]->at(0) <= pivot;
		}

		if (shouldSwap) {
			i++;
			swap(ts[i], ts[j]);
		}
	}
	swap(ts[i + 1], ts[right]);
	return (i + 1);
}

void printTuple(const Tuple &tuple) {
	for (const AttributeValue &v : tuple) {
		switch (getAttributeValueType(v)) {
		case 0:
			std::cout << getLongValue(v) << " ,";
			break;
		case 1:
			std::cout << getDoubleValue(v) << " ,";
			break;
		default:
			const char *s = getStringValue(v);
			if (s == nullptr) {
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

void printRelation(const Relation &relation) {
	std::cout << "==============" << std::endl;
	for (const Tuple &t : relation) {
		printTuple(t);
	}
	std::cout << "==============" << std::endl;
}