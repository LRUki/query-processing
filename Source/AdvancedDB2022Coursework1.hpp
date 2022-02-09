#ifndef ADVANCEDDB2022COURSEWORK1_H
#define ADVANCEDDB2022COURSEWORK1_H

#include <array>
#include <cstdlib>
#include <tuple>
#include <variant>
#include <vector>
// YOU MAY NOT ADD ANY OTHER INCLUDES!!!
using AttributeValue = std::variant<long, double, char const *>;
using Tuple = std::vector<AttributeValue>;
using Relation = std::vector<Tuple>;

/**
 * 0 islong, 1 is double, 2 is a c-string
 */
inline size_t getAttributeValueType(AttributeValue const &value) { return value.index(); }
inline long getLongValue(AttributeValue const &value) { return std::get<long>(value); }
inline double getDoubleValue(AttributeValue const &value) { return std::get<double>(value); }
inline char const *getStringValue(AttributeValue const &value) { return std::get<char const *>(value); }
inline size_t getNumberOfValuesInTuple(Tuple const &t) { return t.size(); }
inline size_t getNumberOfTuplesInRelation(Relation const &t) { return t.size(); }

/**
 * DBMS shall implement the following query in the query function:
 *
 * select sum(large1.c * large2.c * small.c) from large1, large2, small where
 * large1.a = large2.a and large2.a = small.a and large1.b + large2.b + small.b
 * > 9;
 */
class DBMSImplementationForMarks { // you may edit anything inside this class
  // but nothing else
  const Relation *large1, *large2, *small;
  std::vector<const Tuple *> sortedL1Indices, sortedL2Indices;
  std::vector<const Tuple *> hashTable;

public:
  void loadData(Relation const *large1,
                Relation const *large2, // NOLINT(bugprone-easily-swappable-parameters)
                Relation const *small); // NOLINT(bugprone-easily-swappable-parameters)

  long runQuery(long threshold = 9);

private:
  void sortLarges();
  void buildHashTable();
};

class DBMSImplementationForCompetition : public DBMSImplementationForMarks {
public:
  static constexpr char const *teamName =
      nullptr; // set this to your team name if you mean to compete
};

#endif /* ADVANCEDDB2022COURSEWORK1_H */
