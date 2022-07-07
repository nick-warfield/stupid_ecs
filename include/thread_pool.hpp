#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include <bitset>
#include "atomic_queue.hpp"

template <uint THREAD_COUNT, uint MAX_JOBS = 1000>
class ThreadPool
{
	typedef std::function<void(void)> work;
	enum pool_state {
		started,
		stopped,
		undefined
	};

	bool should_terminate;
	std::atomic<pool_state> p_state;
	std::thread threads[THREAD_COUNT];
	std::atomic<std::bitset<THREAD_COUNT>> is_thread_working;
	AtomicQueue<work, MAX_JOBS> job_queue;

	void set_flag(int index, bool value) {
		std::bitset<THREAD_COUNT> expect, flags;
		do {
			expect = is_thread_working.load();
			flags = expect;
			flags.set(index, value);
		} while (!is_thread_working.compare_exchange_strong(
					expect,
					flags)
		);
	}

	static void thread_loop(
			ThreadPool<THREAD_COUNT, MAX_JOBS>* tp,
			int id
	) {
		while (true) {
			while (
				tp->job_queue.is_empty()
				&& !tp->should_terminate)
			{ }

			if (tp->should_terminate) { return; }

			// Need to set working before acquiring job,
			// or else it's possible for finish_jobs()
			// to see an empty queue and no flags set
			tp->set_flag(id, true);
			auto job = tp->job_queue.pop();
			if (job.has_value()) { (*job)(); }
			tp->set_flag(id, false);
		}
	}

public:
	ThreadPool() {
		p_state = stopped;
		start();
	}
	~ThreadPool() {
		stop();
	}

	void push(work job) {
		while (!job_queue.push(job)) { }
	}

	bool push_no_wait(work job) {
		return job_queue.push(job);
	}

	void finish_jobs() {
		while (
			!job_queue.is_empty()
			&& p_state.load() == started
		) { }
		while (
			is_thread_working.load().any()
			&& p_state.load() == started
		) { }
	}

	// note: this won't stop any threads that are currently working
	void clear_jobs() {
		while (job_queue.pop().has_value()) { }
	}

	void start() {
		pool_state expect = stopped;
		do {
			if (p_state.load() == started) { return; }
		}
		while (!p_state.compare_exchange_strong(expect, undefined));

		should_terminate = false;
		for (uint i = 0; i < THREAD_COUNT; ++i) {
			threads[i] = std::thread(thread_loop, this, i);
			is_thread_working.load().reset();
		}
		p_state.store(started);
	}

	void stop() {
		pool_state expect = started;
		do {
			if (p_state.load() == stopped) { return; }
		}
		while (!p_state.compare_exchange_strong(
					expect,
					undefined)
		);

		should_terminate = true;
		for (uint i = 0; i < THREAD_COUNT; ++i) {
			threads[i].join();
			is_thread_working.load().reset();
		}
		p_state.store(stopped);
	}
};
