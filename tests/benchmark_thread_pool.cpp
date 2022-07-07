#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include <vector>

#include "catch.hpp"
#include "thread_pool.hpp"

using namespace std;

TEST_CASE("Thread Pool Benchmarks (8 threads)", "[benchmark][thread_pool]") {
	ThreadPool<8> tp;
	auto f = [](int num) { for (int i = 0; i < 10; ++i) { ++num; } };

	BENCHMARK("Thread Pool") {
		for (int index = 0; index < 8; ++index) {
			tp.push(bind(f, 0));
		}
		tp.finish_jobs();
	};
	tp.stop();

	BENCHMARK("New Threads") {
		vector<thread> threads(8);
		for (int index = 0; index < 8; ++index) {
			threads[index] = thread(f, 0);
		}
		for (int index = 0; index < 8; ++index) {
			threads[index].join();
		}
	};
}
