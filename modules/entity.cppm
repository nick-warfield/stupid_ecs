module;

#include <cstddef>
#include <optional>

export module stupid_ecs.entity;

namespace secs {

export struct Entity {
	const std::size_t generation;
	const std::size_t index;

	Entity& operator=(const Entity &) {
		return *this;
	}

	Entity(const Entity &other)
		: generation(other.generation), index(other.index) { }

private:
	template <typename... T>
	friend class System;

	template <typename... T>
	friend struct Filter;

	Entity(std::size_t gen, std::size_t index)
		: generation(gen), index(index) { }
};

}

