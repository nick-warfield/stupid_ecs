#pragma once

#include <atomic>
#include <functional>
#include <thread>
#include <boost/optional.hpp>

template <typename T>
class AtomicQueue
{
	std::atomic<int> count;
	std::atomic<T*> head, tail;

public:
	bool is_empty();

	// gives a job that no other thread has, or gives nothing
	boost::optional<T> pop();
	void push(T);
};

template <uint THREAD_COUNT>
class ThreadPool
{
	typedef void(*work)(void);

	bool should_terminate = false;
	std::thread threads[THREAD_COUNT];
	AtomicQueue<work> job_queue;

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
