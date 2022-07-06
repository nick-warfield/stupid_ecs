#pragma once

#include <functional>
#include <thread>
#include "atomic_queue.hpp"

template <uint THREAD_COUNT>
class ThreadPool
{
	typedef void(*work)(void);

	bool should_terminate = false;
	std::thread threads[THREAD_COUNT];
	AtomicQueue<work, 1000> job_queue;

	void thread_loop(void) {
		while (true) {
			while (job_queue.is_empty() && !should_terminate) { }
			if (should_terminate) { return; }
			auto job = job_queue.pop();
			if (job.has_value()) { (*job)(); }
		}
	}

public:
	ThreadPool() {
		for (uint i = 0; i < THREAD_COUNT; ++i) {
			threads[i] = std::thread(thread_loop);
		}
	}
	~ThreadPool() {
		should_terminate = true;
		for (uint i = 0; i < THREAD_COUNT; ++i) {
			threads[i].join();
		}
	}

	void queue(const std::function<void()>& job) {
		job_queue.push(*job.target<work>());
	}
};
