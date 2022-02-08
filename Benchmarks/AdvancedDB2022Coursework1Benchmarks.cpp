#include "../Source/AdvancedDB2022Coursework1.hpp"
#include <algorithm>
#include <array>
#include <benchmark/benchmark.h>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <variant>
using namespace std;

template <typename Relation, bool MessUpData> class DataGenerator {
private:
  std::random_device rd;
  std::mt19937 gen;
  std::geometric_distribution<long> valueDistribution;
  std::normal_distribution<> relativeSizeDistribution{9, 3};

  using Tuple = typename Relation::value_type;

public:
  DataGenerator() : gen(rd()) {}

  std::array<Relation, 3> generate(size_t scaleFactor) {
    Relation small;
    small.reserve(scaleFactor * 100 / 95);
    Relation large1;
    large1.reserve(9 * scaleFactor * 100 / 95);
    Relation large2;
    large2.reserve(9 * scaleFactor * 100 / 95);
    long id = 0;
    for(auto i = 0U; i < scaleFactor; i++) {
      auto const numberOfLargeTableTuplesForThisTupleInSmallTable = relativeSizeDistribution(gen);
      for(auto j = 0U; j < numberOfLargeTableTuplesForThisTupleInSmallTable; j++) {
        large1.emplace_back(
            Tuple{id += valueDistribution(gen), valueDistribution(gen), valueDistribution(gen)});
        large2.emplace_back(Tuple{id += valueDistribution(gen), valueDistribution(gen),
                                  valueDistribution(gen), valueDistribution(gen)});
      }
      small.emplace_back(
          Tuple{id + valueDistribution(gen), valueDistribution(gen), valueDistribution(gen)});
    }

    std::shuffle(small.begin(), small.end(), gen);
    std::shuffle(large1.begin(), large1.end(), gen);
    std::shuffle(large2.begin(), large2.end(), gen);
    static set<string> heap;
    if constexpr(MessUpData) {
      auto messItUp = [this](auto& tuple) {
        static auto messyDistribution = std::geometric_distribution<>(0.8);
        tuple.at(0) = [&](auto old) -> AttributeValue {
          switch(messyDistribution(gen)) {
          case 0:
            return old;
          case 1:
            return heap.insert(to_string(get<long>(old))).first->c_str();
          case 2:
            return double(get<long>(old));
          }
          return nullptr;
        }(tuple.at(0));
      };
      std::for_each(large1.begin(), large1.end(), messItUp);
      std::for_each(large2.begin(), large2.end(), messItUp);
      std::for_each(small.begin(), small.end(), messItUp);
    }
    return {move(large1), move(large2), move(small)};
  }
};

auto const smallDataSetSize = 1021;
auto const largeDataSetSize = 1000003;

template <typename DBMSImplementation, bool MessUpData = true>
static void Load(benchmark::State& state) {
  auto generator = DataGenerator<Relation, MessUpData>();
  auto const [large1, large2, small] = generator.generate(state.range(0));
  Relation const empty1;
  Relation const empty2;
  Relation const empty3;
  DBMSImplementation instance;
  for(auto _ : state) {
    instance.loadData(&large1, &large2, &small);
    instance.loadData(&empty1, &empty2, &empty3);
    benchmark::DoNotOptimize(instance);
  }
}

BENCHMARK_TEMPLATE(Load, DBMSImplementationForMarks)
    ->Range(smallDataSetSize, largeDataSetSize)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(Load, DBMSImplementationForCompetition)
    ->Range(smallDataSetSize, largeDataSetSize)
    ->Unit(benchmark::kMillisecond);

template <typename DBMSImplementation, bool MessUpData = true>
static void Join(benchmark::State& state) {
  auto generator = DataGenerator<Relation, MessUpData>();
  auto const [large1, large2, small] = generator.generate(state.range(0));
  Relation const empty1;
  Relation const empty2;
  Relation const empty3;
  DBMSImplementation instance;
  instance.loadData(&large1, &large2, &small);
  for(auto _ : state) {
    auto sum = instance.runQuery();
    benchmark::DoNotOptimize(sum);
    state.counters["Result"] = sum;
  }
}
BENCHMARK_TEMPLATE(Join, DBMSImplementationForMarks)
    ->Range(smallDataSetSize, largeDataSetSize)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(Join, DBMSImplementationForCompetition)
    ->Range(smallDataSetSize, largeDataSetSize)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
