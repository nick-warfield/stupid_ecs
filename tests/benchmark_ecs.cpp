#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "ecs.hpp"

#include <string>

using namespace std;
using namespace secs;

struct Position {
	float x, y, z;
};
struct Velocity {
	float x, y, z;
};

#define TYPES int, string, Position, Velocity 

TEST_CASE("ComponentConfig Get Benchmark", "[benchmark][component]") {
	ComponentConfig<
		int, int, int, int, int,
		int, int, int, int, int,
		int, int, int, int, int,
		int, int, int, int, int,
		int, int, int, int, int,
		int, int, int, int, int,
		int, int, int, int, int,
		int, int, int, int, int,
		int, int, int, int, int,
		int, int, int, int, int>
	cc(
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	);

	int i = 1;
	BENCHMARK("cc.get<0>()") {
		i = *cc.get<0>();
	};

	int j = 1;
	BENCHMARK("cc.get<49>()") {
		j = *cc.get<49>();
	};
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
		for (int i = 0; i < 100; ++i)
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
		for (int i = 0; i < 100; ++i)
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
	cc.get<1>() = "Hello World";
	cc.get<2>() = Position{ 1.1, -2.2, -3.3 };
	cc.get<3>() = Velocity{ -1.1, 2.2, 3.14 };

	vector<Entity> ent;
	for (int i = 0; i < 500; i++) {
		ent.push_back(sys.make(cc));
	}

	int i0;
	BENCHMARK("System::get<int>") {
		i0 = *sys.get<int>(ent[30]);
	};
	string i1;
	BENCHMARK("System::get<string>") {
		i1 = *sys.get<string>(ent[7]);
	};
	Position i2;
	BENCHMARK("System::get<Position>") {
		i2 = *sys.get<Position>(ent[302]);
	};
	Velocity i3;
	BENCHMARK("System::get<Velocity>") {
		i3 = *sys.get<Velocity>(ent[420]);
	};
	Item<TYPES> i4;
	BENCHMARK("System::get all") {
		i4 = *sys[ent[169]];
	};
};
