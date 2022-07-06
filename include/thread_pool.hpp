#pragma once

#include <atomic>
#include <functional>
#include <thread>
#include <boost/optional.hpp>

template <typename T, uint BUFFER_SIZE>
class AtomicQueue
{
	std::atomic<int> write_head = 0, read_head = 0, read_tail = 0;
	T buffer[BUFFER_SIZE];

	inline int next(int index) {
		return (index + 1) % BUFFER_SIZE;
	}

public:
	inline bool is_empty() { return read_head == read_tail;  }
	inline bool is_full()  { return read_head == write_head; }

	// gives a job that no other thread has, or gives nothing
	boost::optional<T> pop() {
		int current_read, next_read;
		do {
			current_read = read_head.load();
			next_read = next(current_read);
			if (current_read == read_tail) { return boost::none; }
		} while (!std::atomic_compare_exchange_strong(&read_head, &current_read, next_read));

		return buffer[current_read];
	}

	// returns false if the the queue is full
	bool push(T item) {
		int current_write, next_write;
		do {
			current_write = write_head.load();
			next_write = next(current_write);
			if (next_write == read_head) { return false; }
		} while (!std::atomic_compare_exchange_strong(&write_head, &current_write, next_write));

		// current_write is reserved, no other thread can touch this
		buffer[current_write] = item;

		read_tail.store(next(read_tail));
		return true;
	}
};

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
