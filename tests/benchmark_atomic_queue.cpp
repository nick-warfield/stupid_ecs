#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include <thread>
#include <vector>

#include "catch.hpp"
#include "atomic_queue.hpp"
#include "thread_pool.hpp"

using namespace std;

// it's not possible to measure push and pop independently
// without putting the queue in an invalid testing state
TEST_CASE("Atomic Queue Benchmarks", "[benchmark][atomic_queue]") {
	AtomicQueue<int, 100> aq;
	BENCHMARK("push & pop 1 item") {
		aq.push(0);
		aq.pop();
	};

	while (!aq.is_empty()) { aq.pop(); }
	BENCHMARK("push & pop 100 items single producer, single consumer") {
		while (!aq.is_full()) { aq.push(0); }
		while (!aq.is_empty()) { aq.pop(); }
	};

	ThreadPool<10> tp;

	while (!aq.is_empty()) { aq.pop(); }
	BENCHMARK("push & pop 100 items multi producer, single consumer") {
		for (int i = 0; i < 10; ++i) {
			tp.push([&aq]() { for (int i = 0; i < 10; ++i) { aq.push(0); } });
		}
		tp.finish_jobs();
		while (!aq.is_empty()) { aq.pop(); }
	};

	while (!aq.is_empty()) { aq.pop(); }
	BENCHMARK("push & pop 100 items single producer, multi consumer") {
		while (!aq.is_full()) { aq.push(0); }
		for (int i = 0; i < 10; ++i) {
			tp.push([&aq]() { for (int i = 0; i < 10; ++i) { aq.pop(); } });
		}
		tp.finish_jobs();
	};
}
