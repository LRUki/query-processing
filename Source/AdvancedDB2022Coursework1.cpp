#include "AdvancedDB2022Coursework1.hpp"

#include <iostream>

// TODO: remove these at the end
void printAttributeValue(AttributeValue const &v);
void printTuple(Tuple const &tuple);
void printTuplePs(Tuples &tuple);
void printRelation(const Relation &relation);

void DBMSImplementationForMarks::loadData(
    Relation const *large1,
    Relation const *large2,  // NOLINT(bugprone-easily-swappable-parameters)
    Relation const *small) {
    //**** sort large1, large2 and store the pointers ****  //
    const size_t large1Size = getNumberOfTuplesInRelation(*large1);
    const size_t large2Size = getNumberOfTuplesInRelation(*large2);

    DirectTuples sortedLarge1;
    DirectTuples sortedLarge2;
    sortedLarge1.reserve(large1Size);
    sortedLarge2.reserve(large2Size);
    join.reserve(small->size());

    for (size_t i = 0; i < max<size_t>(large1Size, large2Size); i++) {
        if (i < large1Size) {
            Tuple const &t = large1->at(i);
            // filter out nullptr
            if (getAttributeValueType(t[0]) != 2 ||
                getStringValue(t[0]) != nullptr) {
                sortedLarge1.push_back(t);
            }
        }
        if (i < large2Size) {
            Tuple const &t = large2->at(i);
            // filter out nullptr
            if (getAttributeValueType(t[0]) != 2 ||
                getStringValue(t[0]) != nullptr) {
                sortedLarge2.push_back(t);
            }
        }
    }
    if (sortedLarge1.size() > 0) {
        qsort(&sortedLarge1[0], sortedLarge1.size(), sizeof(Tuple *),
              compareTuples);
    }
    if (sortedLarge2.size() > 0) {
        qsort(&sortedLarge2[0], sortedLarge2.size(), sizeof(Tuple *),
              compareTuples);
    }

    //**** build hashtable for `small` ****//
    DirectTuples smallHashTable(small->size() * 8, DEFAULT);
    buildHashTable(smallHashTable, *small);

    //** join all three relations **//
    size_t l1 = 0;
    size_t l2 = 0;
    // iterate through sorted `large1` and `large2`
    while (l1 < sortedLarge1.size() && l2 < sortedLarge2.size()) {
        Tuple const &t1 = sortedLarge1[l1];
        Tuple const &t2 = sortedLarge2[l2];

        switch (compareAttributeValues(&t1[0], &t2[0])) {
            case -1:
                l1++;
                break;
            case 1:
                l2++;
                break;
            default:
                // add to `join` when matched
                DirectTuples matches;
                searchHashTable(smallHashTable, t1[0], matches);

                for (Tuple const t3 : matches) {
                    long bs = getLongValue(t1[1]) + getLongValue(t2[1]) +
                              getLongValue(t3.at(1));
                    long cs = getLongValue(t1[2]) * getLongValue(t2[2]) *
                              getLongValue(t3.at(2));
                    join.push_back({bs, cs});
                }
                l1++;
                l2++;
                break;
        }
    }
}

long DBMSImplementationForMarks::runQuery(long threshold) {
    long sum = 0;
    for (BC bc : join) {
        if (bc[0] > threshold) {
            sum += bc[1];
        }
    }
    return sum;
}

/**
 * @brief build a hash table for relation, assumes the size of `hashTable`
 *        is much larger than that of `relation`
 *
 * @param hashTable
 * @param relation
 */
void DBMSImplementationForMarks::buildHashTable(DirectTuples &hashTable,
                                                Relation const &relation) {
    for (size_t i = 0; i < relation.size(); i++) {
        Tuple const &t = relation[i];
        if (getAttributeValueType(t[0]) == 2 &&
            (getStringValue(t[0]) == nullptr)) {
            continue;
        }
        size_t index = hashAttributeValue(t[0]) % hashTable.size();
        size_t d = 1;
        while (hashTable[index] != DEFAULT) {
            index = (index + d) % hashTable.size();
            d *= 2;
        }
        hashTable[index] = t;
    }
}

/**
 * @brief Quadratic probe into the hashTable given the key and add to `result`
 *
 * @param hashTable
 * @param key
 * @param result
 */
void DBMSImplementationForMarks::searchHashTable(DirectTuples &hashTable,
                                                 AttributeValue const &key,
                                                 DirectTuples &result) {
    size_t index = hashAttributeValue(key) % hashTable.size();
    size_t d = 1;
    while (hashTable[index] != DEFAULT) {
        if (compareAttributeValues(&hashTable[index].at(0), &key) == 0) {
            result.push_back(hashTable[index]);
        }
        index = (index + d) % hashTable.size();
        d *= 2;
    }
}

// *************** private helper functions *************** //

/**
 * @brief compares strings lexicographically
 *
 * @param s1
 * @param s2
 * @return 0 if s1 == s2, -1 if s1 < s2, 1 if s1 > s2
 */
int DBMSImplementationForMarks::compareStrings(const char *s1, const char *s2) {
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    while (*p1 && *p1 == *p2) ++p1, ++p2;

    return (*p1 > *p2) - (*p2 > *p1);
}

/**
 * @brief compares AttributeValues
 *
 * @param p1 pointer to AttributeValue
 * @param p2 pointer to AttributeValue
 * @return 0 if v1 == v2, returns <0 if p1 goes before p2, >0 if v2 goes before
 * v1
 */
int DBMSImplementationForMarks::compareAttributeValues(const void *p1,
                                                       const void *p2) {
    AttributeValue &v1 = *(AttributeValue *)p1;
    AttributeValue &v2 = *(AttributeValue *)p2;

    if (getAttributeValueType(v1) == getAttributeValueType(v2) &&
        getAttributeValueType(v1) != 0) {
        if (getAttributeValueType(v1) == 1) {
            // double
            return roundDouble(getDoubleValue(v1)) -
                   roundDouble(getDoubleValue(v2));
        } else {
            // string
            return compareStrings(getStringValue(v1), getStringValue(v2));
        }
    }

    return (v1 < v2) ? -1 : ((v1 > v2) ? 1 : 0);
}

/**
 * @brief compares Tuples based on the first value `a`
 *
 * @param p1 pointer to a pointer to Tuple
 * @param p2 pointer to AttributeValue
 * @return 0 if t1 == t2, returns <0 if t1 goes before t2, >0 if t2 goes before
 * t1
 */
int DBMSImplementationForMarks::compareTuples(const void *p1, const void *p2) {
    Tuple *t1 = *(Tuple **)p1;
    Tuple *t2 = *(Tuple **)p2;
    return compareAttributeValues(&t1->at(0), &t2->at(0));
}
/**
 * @brief  return the hash of `AttributeValue`
 *
 * @param value AttributeValue to hash
 * @return size_t
 */
size_t DBMSImplementationForMarks::hashAttributeValue(
    AttributeValue const &value) {
    if (getAttributeValueType(value) != 2) {
        // for double, cast double to long
        long num = (getAttributeValueType(value) == 0)
                       ? getLongValue(value)
                       : (long)roundDouble(getDoubleValue(value));

        num = ((num >> 16) ^ num) * 0x45d9f3b;
        num = ((num >> 16) ^ num) * 0x45d9f3b;
        return ((num >> 16) ^ num);
    }
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

// TODO: remove these at the end
void printAttributeValue(AttributeValue const &v) {
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

void printTuple(Tuple const &tuple) {
    for (const AttributeValue &v : tuple) {
        printAttributeValue(v);
    }
    std::cout << std::endl;
}

void printTuplePs(Tuples &tuple) {
    std::cout << "=======================" << std::endl;
    for (size_t i = 0; i < tuple.size(); i++) {
        if (tuple[i] == nullptr) {
            continue;
        }
        std::cout << i << ": ";
        printTuple(*(tuple[i]));
    }
    std::cout << "=======================" << std::endl;
}

void printRelation(Relation const &relation) {
    std::cout << "==============" << std::endl;
    for (Tuple const &t : relation) {
        printTuple(t);
    }
    std::cout << "==============" << std::endl;
}