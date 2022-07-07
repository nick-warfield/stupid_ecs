#include "catch.hpp"
#include "thread_pool.hpp"
#include <atomic>

using namespace std;

TEST_CASE("Thread Pool completes jobs", "[thread_pool]") {
	ThreadPool<4, 1000> tp;
	atomic<int> counter = 0;
	int jobs = 1000;
	for (int i = 0; i < jobs; ++i) {
		REQUIRE(tp.push_no_wait([&counter]() { counter++; }));
	}
	tp.finish_jobs();
	REQUIRE(counter.load() == jobs);

	counter = 0;
	jobs = 8;
	for (int i = 0; i < jobs; ++i) {
		tp.push([&counter]() { sleep(1); counter++; });
	}
	tp.finish_jobs();
	REQUIRE(counter.load() == jobs);
}

TEST_CASE("Thread Pool clears queued jobs", "[thread_pool]") {
	ThreadPool<4, 1000> tp;
	atomic<int> counter = 0;
	int jobs = 1000;
	for (int i = 0; i < jobs; ++i) {
		REQUIRE(tp.push_no_wait([&counter]() { sleep(1); counter++; }));
	}
	tp.clear_jobs();
	REQUIRE(counter.load() < jobs);
}

TEST_CASE("Thread Pool can start & stop", "[thread_pool]") {
	ThreadPool<4, 1000> tp;
	atomic<int> counter = 0;
	int jobs = 1000;

	tp.stop();
	for (int i = 0; i < jobs; ++i) {
		REQUIRE(tp.push_no_wait([&counter]() { counter++; }));
	}

	REQUIRE(counter.load() == 0);
	tp.finish_jobs();
	REQUIRE(counter.load() == 0);
	tp.clear_jobs();
	REQUIRE(counter.load() == 0);
	tp.start();
	tp.finish_jobs();
	REQUIRE(counter.load() == 0);

	tp.stop();
	for (int i = 0; i < jobs; ++i) {
		REQUIRE(tp.push_no_wait([&counter]() { counter++; }));
	}
	tp.start();
	tp.finish_jobs();
	REQUIRE(counter.load() == jobs);
}
