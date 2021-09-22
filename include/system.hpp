#pragma once

#include <optional>
#include <vector>
#include <stack>
#include <memory>

#include "component.hpp"
#include "entity.hpp"

struct Foo {
	int a;

	struct Bar {
		Foo& foo;
		int b;
		int data() { return foo.a + b; }
	};
};

// make this a recursive data structure
template <typename ...T>
class System {
};

template <>
class System<> {
	std::stack<std::size_t> m_next_alloc;
	std::vector<std::size_t> m_generation;
	std::vector<component_flag> m_component;

public:
	Entity make(ComponentConfig<> cc) { return _make_helper(cc, cc.bitmask()); }
	Entity _make_helper(ComponentConfig<>, const component_flag &flags) {
		if (m_next_alloc.empty()) {
			m_generation.push_back(0);
			m_component.push_back(COMPONENT_ALIVE | flags);
			return Entity(0, m_generation.size() - 1);
		} else {
			auto next = m_next_alloc.top();
			m_next_alloc.pop();
			m_generation[next]++;
			m_component[next] = COMPONENT_ALIVE | flags;
			return Entity(m_generation[next], next);
		}
	}

	void erase(const Entity &id) {
		if (m_generation[id.index] != id.generation) return;
		m_component[id.index] &= ~COMPONENT_ALIVE;
		m_next_alloc.push(id.index);
	}
	
	auto get(const Entity &id) {
		return _get_helper(id, 0);
	}
	std::optional<ComponentConfig<>> _get_helper(
			const Entity &id,
			component_flag) {
		if (is_alive(id)) {
			return ComponentConfig();
		} else {
			return {};
		}
	}

	component_flag get_bits(const Entity &id) {
		return m_component[id.index];
	}

	bool is_alive(const Entity &id) {
		return m_generation[id.index] == id.generation
			&& m_component[id.index] & COMPONENT_ALIVE;
	}

	bool has_component(const Entity &id, const component_flag &flags) {
		return m_generation[id.index] == id.generation
			&& m_component[id.index] & flags;
	}
};


template <typename T, typename ...R>
class System<T, R...> {
public:
	Entity make(ComponentConfig<T, R...> cc) { return _make_helper(cc, cc.bitmask()); }
	Entity _make_helper(ComponentConfig<T, R...> cc, const component_flag &flags) {
		Entity e = m_tail._make_helper(cc.remainder, flags);
		if (e.index == m_data.size()) {
			m_data.push_back(cc.component.value_or(T()));
		} else {
			m_data[e.index] = cc.component.value_or(T());
		}
		return e;
	}
	void erase(const Entity &id) {
		m_tail.erase(id);
	}

	bool is_alive(const Entity &id) {
		return m_tail.is_alive(id);
	}
	bool has_component(const Entity& id, const component_flag& flags) {
		return m_tail.has_component(id, flags);
	}

	std::optional<ComponentConfig<
		std::reference_wrapper<T>,
		std::reference_wrapper<R>...>>
	get(const Entity &id) {
		if (!is_alive(id)) return std::nullopt;
		return _get_helper(id, get_bits(id));
	}
	std::optional<ComponentConfig<
		std::reference_wrapper<T>,
		std::reference_wrapper<R>...>>
	_get_helper(const Entity &id, component_flag bits) {
		auto value = bits & 1
			? std::optional(std::ref(m_data[id.index]))
			: std::nullopt;
		auto tail = m_tail._get_helper(id, bits >> 1);

		ComponentConfig<
			std::reference_wrapper<T>,
			std::reference_wrapper<R>...>
		cc(value, tail.value());
		return cc;
	}

	component_flag get_bits(const Entity &id) {
		return m_tail.get_bits(id);
	}

private:
	std::vector<T> m_data;
	System<R...> m_tail;
};
