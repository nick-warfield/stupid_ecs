#pragma once
#include <cstddef>
#include <optional>
#include "system.hpp"

struct Entity {
	Entity(std::size_t gen, std::size_t index, System* system)
		: generation(gen), index(index), m_ptr(system) { }
	Entity(const Entity &) = default;

	Entity& operator=(const Entity &) {
		return *this;
	}

	std::optional<System::Item> operator*() {
		return m_ptr ? m_ptr->get(*this) : std::nullopt;
	}

	std::optional<System::Item> operator->() {
		return m_ptr ? m_ptr->get(*this) : std::nullopt;
	}

	const std::size_t generation;
	const std::size_t index;

private:
	System * const m_ptr;
};

