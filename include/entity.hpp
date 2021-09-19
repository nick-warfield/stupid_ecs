#pragma once
#include <cstddef>

struct Entity {
	Entity(std::size_t gen, std::size_t index)
		: generation(gen), index(index) { }
	Entity(const Entity &) = default;
	Entity& operator=(const Entity &) {
		return *this;
	}

	const std::size_t generation;
	const std::size_t index;
};

