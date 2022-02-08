#include "../Source/AdvancedDB2022Coursework1.hpp"
#define CATCH_CONFIG_MAIN
#include "../Source/AdvancedDB2022Coursework1.hpp"
#include <catch2/catch.hpp>
using namespace std;

TEMPLATE_TEST_CASE("Single Tuple", "", DBMSImplementationForMarks,
                   DBMSImplementationForCompetition)
{
  TestType instance;
  auto [a, b, c] = tuple{Relation{Tuple{1L, 20L, 3L}}, //
                         Relation{Tuple{1L, 17L, 3L}}, //
                         Relation{Tuple{1L, 29L, 3L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 27);
}

TEMPLATE_TEST_CASE("Single Tuple String Join", "", DBMSImplementationForMarks,
                   DBMSImplementationForCompetition)
{
  TestType instance;
  auto [a, b, c] = tuple{Relation{Tuple{"one", 20L, 3L}}, //
                         Relation{Tuple{"one", 17L, 4L}}, //
                         Relation{Tuple{"one", 29L, 5L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 60);
}

TEMPLATE_TEST_CASE("Two Tuple String/Int Join", "", DBMSImplementationForMarks,
                   DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] = tuple{Relation{Tuple{1L, 20L, 6L}, Tuple{"one", 20L, 3L}}, //
                               Relation{Tuple{1L, 20L, 7L}, Tuple{"one", 17L, 4L}}, //
                               Relation{Tuple{1L, 20L, 8L}, Tuple{"one", 29L, 5L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 396);
}

TEMPLATE_TEST_CASE("No matches when joining int and float", "", DBMSImplementationForMarks,
                   DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] = tuple{Relation{Tuple{1L, 20L, 6L}, Tuple{"one", 20L, 3L}}, //
                               Relation{Tuple{1.0, 20L, 7L}, Tuple{1.0, 17L, 4L}},  //
                               Relation{Tuple{1L, 20L, 8L}, Tuple{"one", 29L, 5L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 0);
}

TEMPLATE_TEST_CASE("No matches when joining int and nullptr", "", DBMSImplementationForMarks,
                   DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] = tuple{Relation{Tuple{1L, 20L, 6L}, Tuple{"one", 20L, 3L}},        //
                               Relation{Tuple{nullptr, 20L, 7L}, Tuple{nullptr, 17L, 4L}}, //
                               Relation{Tuple{1L, 20L, 8L}, Tuple{"one", 29L, 5L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 0);
}

TEMPLATE_TEST_CASE("Some matches when joining int and nullptr", "", DBMSImplementationForMarks,
                   DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] = tuple{Relation{Tuple{1L, 20L, 6L}, Tuple{"one", 20L, 3L}},   //
                               Relation{Tuple{1L, 20L, 7L}, Tuple{nullptr, 17L, 4L}}, //
                               Relation{Tuple{1L, 20L, 8L}, Tuple{"one", 29L, 5L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 336);
}

TEMPLATE_TEST_CASE("Duplicates in small table", "", DBMSImplementationForMarks,
                   DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] = tuple{Relation{Tuple{1L, 20L, 6L}, Tuple{"one", 20L, 3L}},   //
                               Relation{Tuple{1L, 20L, 7L}, Tuple{nullptr, 17L, 4L}}, //
                               Relation{Tuple{1L, 20L, 8L}, Tuple{1L, 29L, 5L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 546);
}

TEMPLATE_TEST_CASE("Small Database", "", DBMSImplementationForMarks,
                   DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] =
      tuple{Relation{Tuple{1L, 20L, 3L}, Tuple{2L, 20L, 3L}, Tuple{5L, 20L, 3L}}, //
            Relation{Tuple{2L, 17L, 3L}, Tuple{5L, 20L, 3L}, Tuple{8L, 20L, 3L}}, //
            Relation{Tuple{1L, 29L, 3L}, Tuple{5L, 20L, 3L}, Tuple{2L, 20L, 3L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 54);
}

TEMPLATE_TEST_CASE("Larger Database", "", DBMSImplementationForMarks,
                   DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] = tuple{
      Relation{Tuple{1L, 20L, 3L}, Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}, Tuple{5L, 20L, 3L},
               Tuple{9L, 20L, 3L}, Tuple{3L, 20L, 3L}}, //
      Relation{Tuple{1L, 29L, 3L}, Tuple{5L, 20L, 3L}, Tuple{3L, 20L, 3L}, Tuple{9L, 20L, 3L},
               Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}},                    //
      Relation{Tuple{2L, 17L, 3L}, Tuple{5L, 20L, 3L}, Tuple{8L, 20L, 3L}} //
  };
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 81);
}

TEMPLATE_TEST_CASE("Larger Database with duplicates in small table", "", DBMSImplementationForMarks,
                   DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] =
      tuple{Relation{Tuple{1L, 20L, 3L}, Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}, Tuple{5L, 20L, 3L},
                     Tuple{9L, 20L, 3L}, Tuple{3L, 20L, 3L}}, //
            Relation{Tuple{1L, 29L, 3L}, Tuple{5L, 20L, 3L}, Tuple{3L, 20L, 3L}, Tuple{9L, 20L, 3L},
                     Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}}, //
            Relation{Tuple{2L, 17L, 3L}, Tuple{3L, 20L, 3L}, Tuple{2L, 17L, 3L}, Tuple{3L, 20L, 3L},
                     Tuple{1L, 20L, 3L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 135);
}

TEMPLATE_TEST_CASE("Larger Database with duplicates in small table and custom threshold", "",
                   DBMSImplementationForMarks, DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] =
      tuple{Relation{Tuple{1L, 20L, 3L}, Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}, Tuple{5L, 20L, 3L},
                     Tuple{9L, 20L, 3L}, Tuple{3L, 20L, 3L}}, //
            Relation{Tuple{1L, 29L, 3L}, Tuple{5L, 20L, 3L}, Tuple{3L, 20L, 3L}, Tuple{9L, 20L, 3L},
                     Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}}, //
            Relation{Tuple{2L, 17L, 3L}, Tuple{3L, 20L, 3L}, Tuple{2L, 17L, 3L}, Tuple{3L, 20L, 3L},
                     Tuple{1L, 20L, 3L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery(100) == 0);
}

TEMPLATE_TEST_CASE("Larger Database with duplicates in small table and different custom threshold",
                   "", DBMSImplementationForMarks, DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] =
      tuple{Relation{Tuple{1L, 20L, 3L}, Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}, Tuple{5L, 20L, 3L},
                     Tuple{9L, 20L, 3L}, Tuple{3L, 20L, 3L}}, //
            Relation{Tuple{1L, 29L, 3L}, Tuple{5L, 20L, 3L}, Tuple{3L, 20L, 3L}, Tuple{9L, 20L, 3L},
                     Tuple{2L, 20L, 3L}, Tuple{8L, 20L, 3L}}, //
            Relation{Tuple{2L, 17L, 3L}, Tuple{3L, 20L, 3L}, Tuple{2L, 17L, 3L}, Tuple{3L, 20L, 3L},
                     Tuple{1L, 20L, 3L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery(60) == 27);
}

TEMPLATE_TEST_CASE("Larger Database with differnt types in the first column",
                   "", DBMSImplementationForMarks, DBMSImplementationForCompetition)
{
  TestType instance;
  auto const [a, b, c] =
      tuple{Relation{
                Tuple{nullptr, 20L, 3L},
                Tuple{9.9, 20L, 3L},
                Tuple{"zyx", 20L, 3L},
                Tuple{"abc", 120L, 3L},
                Tuple{"aaa", 120L, 3L},
                Tuple{380L, 20L, 3L},
                Tuple{214L, 20L, 3L},
                Tuple{9.3, 20L, 13L},
                Tuple{3L, 20L, 7L},
            }, //
            Relation{Tuple{1L, 29L, 3L}, Tuple{"zyx", 20L, 3L}, Tuple{3L, 20L, 3L}, Tuple{9L, 20L, 3L},
                     Tuple{9.3, 20L, 3L}, Tuple{9.9, 20L, 3L}}, //
            Relation{Tuple{"zyx", 20L, 3L}, Tuple{2L, 17L, 3L}, Tuple{3L, 20L, 3L},
                     Tuple{9.3, 20L, 3L}}};
  instance.loadData(&a, &b, &c);

  REQUIRE(instance.runQuery() == 117 + 27 + 63);
}