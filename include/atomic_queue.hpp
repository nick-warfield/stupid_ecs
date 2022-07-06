#pragma once

#include <atomic>
#include <boost/optional.hpp>

template <typename T, uint BUFFER_SIZE>
class AtomicQueue
{
	std::atomic<int> write_head = 0, read_head = 0, read_tail = 0;
	T buffer[BUFFER_SIZE + 1];

	inline int next(int index) {
		return (index + 1) % (BUFFER_SIZE + 1);
	}

public:
	inline bool is_empty() { return read_head == read_tail;  }
	inline bool is_full()  { return next(write_head) == read_head; }

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

