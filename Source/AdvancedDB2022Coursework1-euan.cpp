#include "AdvancedDB2022Coursework1.hpp"
#include <iostream>
#include <cstring>

void printVal(const int &type, const AttributeValue val);
void printTuple(const Tuple &t);
void printTable(const Relation *relation);
void printHashTable(const HashTable* ht);
HT_entry* createEntry(AttributeValue key, Tuple &data);
void freeEntry(HT_entry* entry);
void freeTable(HashTable* ht);
HashTable* createHashTable(int size);
size_t hashValue(AttributeValue const& value);

void DBMSImplementationForMarks::loadData(Relation const *large1,
					  Relation const *large2, // NOLINT(bugprone-easily-swappable-parameters)
					  Relation const *small)
{

	this->large1 = large1;
	this->large2 = large2;
	this->small = small;
}

bool isAEqual(const int &type, const Tuple &t1, const Tuple &t2, const Tuple &t3) {
	switch (type) {
	case 0:
		return getLongValue(t1[0]) == getLongValue(t2[0]) && getLongValue(t2[0]) == getLongValue(t3[0]);
	case 1:
		return getdoubleValue(t1[0]) == getdoubleValue(t2[0]) && getdoubleValue(t2[0]) == getdoubleValue(t3[0]);
	default:
		return getStringValue(t1[0]) == getStringValue(t2[0]) && getStringValue(t2[0]) == getStringValue(t3[0]);
	}
}

long DBMSImplementationForMarks::runQuery(long threshold) {
	long sum = 0;
	std::cout << "Joining\n\n";
	std::cout << "Large 1:\n";
	printTable(large1);
	std::cout << "Large 2:\n";
	printTable(large2);
	std::cout << "Small:\n";
	printTable(small);
	std::cout << "\n";
	joinTotal(threshold);
	return sum;
}

void DBMSImplementationForMarks::hashSmallTable() {
	// Allocated twice the needed space
    hashTable = createHashTable(2 * small->size());
    // HashTable* ht = createHashTable(26);

	std::cout << "Starting the hash...\n";

	// Hash all tuples in the table provided
	for (auto t : *small) {
		if (hashTable->size == hashTable->count) {
			throw std::invalid_argument("Hash table has run out of space");
		}

		// std::cout << "Making entry\n";
		HT_entry *entry = createEntry(t[0], t);
		// std::cout << "Hashing key\n";
		size_t index = hashValue(t[0]) % hashTable->size;
		// std::cout << "Hashed\n";

		size_t step = 1;
		while (hashTable->items[index] != NULL) {
			index = (index + step) % hashTable->size;
			step *= 2;
		}
		hashTable->items[index] = entry;
		hashTable->count++;
	}
}

void DBMSImplementationForMarks::hashJoin() {
	for (auto &t : *joinedTable) {
		findMatches(t);
	}

	freeTable(hashTable);
}

void DBMSImplementationForMarks::joinTotal(long threshold) {
	
	std::cout << "Merging two large\n";

	for (auto &t1 : *large1) {
		for (auto &t2 : *large2) {
			int aType1 = getAttributeValueType(t1[0]);
			int aType2 = getAttributeValueType(t2[0]);
			if (aType1 != aType2) {
				continue;
			}

			if (isAEqual(aType1, t1, t2, t1)) {
				joinedTable->push_back(mergeTwoTuples(t1, t2));
			}
		}
	}

	std::cout << "First join results:\n";
	printTable(joinedTable);
	std::cout << "\n";

	std::cout << "Hashing Small Table\n";
	hashSmallTable();
	printHashTable(hashTable);
	std::cout << "\n";

	std::cout << "Joining\n";
	hashJoin();

	std::cout << "\nHash join results:\n";
	printTable(finalTable);

	// int sum = 0;
	// for (auto &t : *finalTable) {
	// 	if (getLongValue(t[1]) > threshold) {
	// 		sum += getLongValue(t[2]);
	// 	}
	// }

	// std::cout << "Final sum: " << sum;
}

void printVal(const int &type, const AttributeValue val) {
	// Print the value of an AttributeValue
	switch (type) {
	case 0:
		std::cout << getLongValue(val);
		break;
	case 1:
		std::cout << getdoubleValue(val);
		break;
	default:
		std::cout << getStringValue(val);
	}
}

void printTuple(const Tuple &t) {
	// Print the tuple as space separated values
	printVal(getAttributeValueType(t[0]), t[0]);
	std::cout << "\t";
	printVal(getAttributeValueType(t[1]), t[1]);
	std::cout << "\t";
	printVal(getAttributeValueType(t[2]), t[2]);
	std::cout << "\n";
}

void printTable(const Relation *relation) {
	// Print a whole table
	for (auto &t : *relation) {
		std::cout << "tupling\n";
		printTuple(t);
		std::cout << "tupling done\n";
	}
}

void printHashTable(const HashTable* ht) {
	// Print a whole hash table
	std::cout << "HASH TABLE:\n";
	for (int i=0; i<ht->size; i++) {
		std::cout << i << ": ";
        if (ht->items[i] != NULL) {
			printTuple(ht->items[i]->value);
		} else {
			std::cout << "NULL\n";
		}
	}
}

HT_entry* createEntry(AttributeValue key, Tuple &data) {
	// Make an entry for the hash table 
	// std::cout << "Allocating\n";
	HT_entry* entry = (HT_entry*) malloc (sizeof(HT_entry));
	// std::cout << "Setting key\n";
	entry->key = key;
	// std::cout << "Setting data\n";
	entry->value = data;
	// std::cout << "Done\n";
	return entry;
}

void freeEntry(HT_entry* entry) {
	free(entry);
}

void freeTable(HashTable* ht) {
	// Free a hash table and all its entries
	for (auto i=0; i < ht->size; i++) {
		HT_entry* entry = ht->items[i];
		if (entry != NULL) {
			freeEntry(entry);
		}
	}

	free(ht->items);
	free(ht);
}

HashTable* createHashTable(int size) {
	// Allocate space for a hash table
    HashTable* table = (HashTable*) malloc (sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = (HT_entry**) calloc (table->size, sizeof(HT_entry*));
    for (int i=0; i<table->size; i++) {
		// Set all values to null to start with
        table->items[i] = NULL;
	}
    return table;
} 

size_t hashValue(AttributeValue const& value) {
    if (getAttributeValueType(value) == 0) {
        // long
		long num = getLongValue(value);
		num = ((num >> 16) ^ num) * 0x45d9f3b;
		num = ((num >> 16) ^ num) * 0x45d9f3b;
		return ((num >> 16) ^ num);
    } else if (getAttributeValueType(value) == 1) {
        // double
		double d = getdoubleValue(value);
		uint64_t u;
		memcpy(&u, &d, sizeof(d));
		return hashValue((long) u);
    } 
    // c-string
	// Hashing based on Bezout's Identity
	const char *str = getStringValue(value);
	int prime_1 = 54059;
	int prime_2 = 76963;
	int prime_3 = 86969;
	int hash_val = 37;
	unsigned h = hash_val;
	while (*str) {
		hash_val = (hash_val * prime_1) ^ (str[0] * prime_2);
		str++;
	}
	return hash_val;
}

Tuple DBMSImplementationForMarks::mergeTwoTuples(const Tuple &t1, const Tuple &t2) {
	// std::cout << "Merging\n";
	// printTuple(t1);
	AttributeValue a = t1[0];
	bool longVals = getAttributeValueType(t2[1]) == 0;
	long b = longVals ? getLongValue(t1[1]) + getLongValue(t2[1]) : getdoubleValue(t1[1]) + getdoubleValue(t2[1]);
	long c = longVals ? getLongValue(t1[2]) * getLongValue(t2[2]) : getdoubleValue(t1[2]) * getdoubleValue(t2[2]);
	std::cout << getAttributeValueType(a) << " " << getAttributeValueType(b) << " " << getAttributeValueType(c) << "\n";
	std::cout << getLongValue(a) << " " << b << " " << c << "\n";
	// std::cout << "Merged\n" ;
	return Tuple{a, b, c};
}

void DBMSImplementationForMarks::findMatches(Tuple &t) {
	// Linear probing to find all the entries in the hash table with the same key (i.e. small.a)
	size_t index = hashValue(t[0]) % hashTable->size;
	size_t step = 1;
	while (hashTable->items[index] != NULL) {
		HT_entry *entry = hashTable->items[index];
		if (entry->key == t[0]) {
			auto joined = mergeTwoTuples(t, entry->value);
			joinedTable->push_back(joined);
		}
		index = (index + step) % hashTable->size;
		step *= 2;
	}

	// std::cout << results.size() << " items found \n";
}

int main() {
	// auto a = Relation{Tuple{1L, 20L, 3L}, Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}, Tuple{5L, 20L, 3L}, Tuple{9L, 20L, 3L}, Tuple{3L, 20L, 3L}}; //
    // auto b = Relation{Tuple{1L, 29L, 3L}, Tuple{5L, 20L, 3L}, Tuple{3L, 20L, 3L}, Tuple{9L, 20L, 3L}, Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}}; //
    // auto c = Relation{Tuple{2L, 17L, 3L}, Tuple{3L, 20L, 3L}, Tuple{2L, 17L, 3L}}; //, Tuple{3L, 20L, 3L}, Tuple{1L, 20L, 3L}};

	// auto a = Relation{Tuple{"one", 20L, 3L}, Tuple{"two", 20L, 3L}};
	// auto b = Relation{Tuple{"one", 20L, 3L}, Tuple{"three", 20L, 3L}};
	// auto c = Relation{Tuple{"five", 20L, 3L}, Tuple{"one", 20L, 3L}, Tuple{"two", 2L, 5L}};

	// auto a = Relation{Tuple{1L, 20L, 3L}, Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}, Tuple{5L, 20L, 3L}};
	// auto b = Relation{Tuple{1L, 29L, 3L}, Tuple{5L, 20L, 3L}, Tuple{3L, 20L, 3L}, Tuple{9L, 20L, 3L}};
	// auto c = Relation{Tuple{2L, 17, 3L}, Tuple{3L, 20L, 3L}, Tuple{2L, 17, 3L}, Tuple{3L, 20L, 3L}, Tuple{1L, 20L, 3L}};

	auto a = Relation{Tuple{1L, 20L, 3L}};
	auto b = Relation{Tuple{1L, 20L, 3L}};
	auto c = Relation{Tuple{1L, 20L, 3L}};


	DBMSImplementationForMarks query;
	query.loadData(&a, &b, &c);
	query.runQuery();

	// int sum = joinTotal(a, b, c, 9);
	// std::cout << "\nResult is: " << sum << "\n";

    return 0;
}