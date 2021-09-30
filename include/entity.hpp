#pragma once
#include <cstddef>
#include <optional>

namespace secs {

struct Entity {
	Entity(const Entity &other)
		: generation(other.generation), index(other.index) { }
	Entity(std::size_t gen, std::size_t index)
		: generation(gen), index(index) { }

	Entity& operator=(const Entity &) {
		return *this;
	}

	const std::size_t generation;
	const std::size_t index;
};

}

