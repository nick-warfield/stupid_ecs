#include <atomic>
#include <algorithm>
#include <thread>
#include <vector>
#include <queue>
#include <random>
#include <boost/optional/optional_io.hpp>

#include "catch.hpp"
#include "atomic_queue.hpp"

using namespace std;

const int SIZE = 10000;

TEST_CASE("Atomic Queue single threaded", "[atomic_queue]") {
	AtomicQueue<int, SIZE> atomic_queue;
	REQUIRE(atomic_queue.is_empty());
	REQUIRE_FALSE(atomic_queue.is_full());

	vector<int> nums;
	nums.reserve(SIZE);
	auto rng = default_random_engine();
	for (int i = 0; i < SIZE; ++i) {
		nums.push_back(rng() % 100000);
		atomic_queue.push(nums.back());
	}
	REQUIRE_FALSE(atomic_queue.is_empty());
	REQUIRE(atomic_queue.is_full());

	for (int i = 0; i < SIZE; ++i) {
		auto item = atomic_queue.pop();
		REQUIRE(item.has_value());
		REQUIRE(nums[i] == *item);
	}
	REQUIRE(atomic_queue.is_empty());
	REQUIRE_FALSE(atomic_queue.is_full());
}

TEST_CASE("Atomic Queue multiple consumers", "[atomic_queue]") {
	AtomicQueue<int, SIZE> atomic_queue;
	REQUIRE(atomic_queue.is_empty());
	REQUIRE_FALSE(atomic_queue.is_full());

	vector<int> nums;
	nums.reserve(SIZE);
	auto rng = default_random_engine();
	for (int i = 0; i < SIZE; ++i) {
		nums.push_back(rng() % 100000);
		atomic_queue.push(nums.back());
	}
	REQUIRE_FALSE(atomic_queue.is_empty());
	REQUIRE(atomic_queue.is_full());

	atomic<int> count(0);
	auto f = [&count, &atomic_queue]() {
		auto item = atomic_queue.pop();
		REQUIRE(item.has_value());
		count++;
	};

	int thread_count = GENERATE(5, 100);
	while (!atomic_queue.is_empty()) {
		vector<thread> threads;
		threads.reserve(thread_count);
		for (int i = 0; i < thread_count; ++i) {
			threads.push_back(thread(f));
		}
		for (int i = 0; i < thread_count; ++i) {
			threads[i].join();
		}
	}
	REQUIRE(count.load() == (int)nums.size());
	REQUIRE(atomic_queue.is_empty());
	REQUIRE_FALSE(atomic_queue.is_full());
}

TEST_CASE("Atomic Queue multiple producers", "[atomic_queue]") {
	AtomicQueue<int, SIZE> atomic_queue;
	REQUIRE(atomic_queue.is_empty());
	REQUIRE_FALSE(atomic_queue.is_full());

	vector<int> nums;
	nums.reserve(SIZE);
	auto rng = default_random_engine();
	for (int i = 0; i < SIZE; ++i) {
		nums.push_back(rng() % 100000);
	}

	auto f = [nums, &atomic_queue](int start) {
		for (int i = 0; i < 100; ++i) {
			REQUIRE(atomic_queue.push(nums[start + i]));
		}
	};

	int thread_count = GENERATE(5, 100);
	int start = 0;
	while (!atomic_queue.is_full()) {
		vector<thread> threads;
		threads.reserve(thread_count);
		for (int i = 0; i < thread_count; ++i) {
			threads.push_back(thread(f, start));
			start += 100;
		}
		for (int i = 0; i < thread_count; ++i) {
			threads[i].join();
		}
	}
	REQUIRE_FALSE(atomic_queue.is_empty());
	REQUIRE(atomic_queue.is_full());
}
