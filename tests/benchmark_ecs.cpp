#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <random>
#include <string>
#include <unordered_set>

#include "catch.hpp"
#include "ecs.hpp"

using namespace std;
using namespace secs;

struct Position {
  float x, y, z;
  Position &operator=(const Position &) = default;
};
struct Velocity {
  float x, y, z;
  Velocity &operator=(const Velocity &) = default;
};

#define TYPES int, uint, string, Position, Velocity

TEST_CASE("ComponentConfig Get Benchmark", "[benchmark][component]") {
  ComponentConfig<int, int, int, int, int, int, int, int, int, int, int, int,
                  int, int, int, int, int, int, int, int, int, int, int, int,
                  int, int, int, int, int, int, int, int, int, int, int, int,
                  int, int, int, int, int, int, int, int, int, int, int, int,
                  int, int>
      cc(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0);

  BENCHMARK("cc.get<0>()") { return *cc.get<0>(); };

  BENCHMARK("cc.get<49>()") { return *cc.get<49>(); };
};

TEST_CASE("System Make Benchmark", "[benchmark][system][make]") {
  BENCHMARK_ADVANCED("System<>::make() cold")
  (Catch::Benchmark::Chronometer meter) {
    System<> sys;
    ComponentConfig<> cc;
    meter.measure([&sys, &cc] { sys.make(cc); });
  };

  BENCHMARK_ADVANCED("System<>::make() hot")
  (Catch::Benchmark::Chronometer meter) {
    System<> sys;
    ComponentConfig<> cc;
    for (int i = 0; i < 2100; ++i)
      sys.make(cc);
    meter.measure([&sys, &cc] { sys.make(cc); });
  };

  BENCHMARK_ADVANCED("System<>::make() realloc")
  (Catch::Benchmark::Chronometer meter) {
    System<> sys;
    ComponentConfig<> cc;
    auto e = sys.make(cc);
    sys.erase(e);
    meter.measure([&sys, &cc] { sys.make(cc); });
  };

  BENCHMARK_ADVANCED("System<TYPES>::make() cold")
  (Catch::Benchmark::Chronometer meter) {
    System<TYPES> sys;
    ComponentConfig<TYPES> cc;
    meter.measure([&sys, &cc] { sys.make(cc); });
  };

  BENCHMARK_ADVANCED("System<TYPES>::make() hot")
  (Catch::Benchmark::Chronometer meter) {
    System<TYPES> sys;
    ComponentConfig<TYPES> cc;
    for (int i = 0; i < 2100; ++i)
      sys.make(cc);
    meter.measure([&sys, &cc] { sys.make(cc); });
  };

  BENCHMARK_ADVANCED("System<TYPES>::make() realloc")
  (Catch::Benchmark::Chronometer meter) {
    System<TYPES> sys;
    ComponentConfig<TYPES> cc;
    auto e = sys.make(cc);
    sys.erase(e);
    meter.measure([&sys, &cc] { sys.make(cc); });
  };
};

TEST_CASE("System Erase Benchmark", "[benchmark][system][erase]") {
  BENCHMARK_ADVANCED("System<>::erase()")
  (Catch::Benchmark::Chronometer meter) {
    System<> sys;
    ComponentConfig<> cc;
    auto e = sys.make(cc);
    meter.measure([&sys, &e] { sys.erase(e); });
  };

  BENCHMARK_ADVANCED("System<TYPES>::erase()")
  (Catch::Benchmark::Chronometer meter) {
    System<TYPES> sys;
    ComponentConfig<TYPES> cc;
    auto e = sys.make(cc);
    meter.measure([&sys, &e] { sys.erase(e); });
  };
};

TEST_CASE("System Get Benchmark", "[benchmark][system][get]") {
  System<TYPES> sys;
  ComponentConfig<TYPES> cc;
  cc.get<0>() = 0;
  cc.get<1>() = 10;
  cc.get<2>() = "Hello World";
  cc.get<3>() = Position{1.1, -2.2, -3.3};
  cc.get<4>() = Velocity{-1.1, 2.2, 3.14};

  vector<Entity> ent;
  for (int i = 0; i < 500; i++) {
    ent.push_back(sys.make(cc));
  }

  BENCHMARK("System::get<int>") { return sys.get<0>(ent[30]); };
  BENCHMARK("System::get<uint>") { return sys.get<1>(ent[101]); };
  BENCHMARK("System::get<string>") { return sys.get<2>(ent[7]); };
  BENCHMARK("System::get<Position>") { return sys.get<3>(ent[302]); };
  BENCHMARK("System::get<Velocity>") { return sys.get<4>(ent[420]); };
  BENCHMARK("System::get all") { return sys[ent[169]]; };
};

TEST_CASE("System Iteration Benchmark", "[benchmark][system][iteration]") {
  auto rng = default_random_engine();
  auto num_of_items = GENERATE(10, 100, 1000, 5000);
  unordered_set<int> sparse_pack;
  while (sparse_pack.size() < num_of_items * 0.2) {
    sparse_pack.insert(rng() % num_of_items);
  }

  System<TYPES> sys;
  ComponentConfig<TYPES> cc;
  vector<Entity> ent;

  for (auto i = 0; i < num_of_items; ++i) {
    cc.get<int>() =
        i < .2 * num_of_items ? boost::make_optional(i) : boost::none;
    cc.get<uint>() =
        sparse_pack.count(i) ? boost::make_optional(i) : boost::none;

    cc.get<string>() =
        rng() % 100 < 50 ? boost::make_optional("Hello World") : boost::none;

    cc.get<Position>() = rng() % 100 < 90
                             ? boost::make_optional(Position{1, 2, 3})
                             : boost::none;

    cc.get<Velocity>() = rng() % 100 < 90
                             ? boost::make_optional(Velocity{3, 2, 1})
                             : boost::none;

    ent.push_back(sys.make(cc));
  }

  auto name = "Iterate over " + to_string(num_of_items) + " items";
  BENCHMARK(name.c_str()) {
    auto count = 0;
    for (auto item : sys.iter()) {
      (void)(item);
      count++;
    }
    return count;
  };

  name =
      "Iterate over Dense Packing of " + to_string(num_of_items / 5) + " ints";
  BENCHMARK(name.c_str()) {
    auto count = 0;
    for (auto [i] : sys.iter<int>()) {
      (void)(i);
      count++;
    }
    return count;
  };

  name =
      "Iterate over Sparse Packing of " + to_string(num_of_items / 5) + " ints";
  BENCHMARK(name.c_str()) {
    auto count = 0;
    for (auto [i] : sys.iter<uint>()) {
      (void)(i);
      count++;
    }
    return count;
  };

  BENCHMARK("Iterate over Position & Velocity with Filter & 81\% hit rate") {
    auto count = 0;
    for (auto [pos, vel] : sys.iter<Position, Velocity>()) {
      (void)(pos);
      (void)(vel);
      count++;
    }
    return count;
  };

  BENCHMARK("Iterate over Position & Velocity with Item & 81\% hit rate") {
    auto count = 0;
    for (auto item : sys.iter()) {
      if (item.get<Position &>() && item.get<Velocity &>()) {
        (void)(item.get<Position &>());
        (void)(item.get<Velocity &>());
        count++;
      }
    }
    return count;
  };
};
