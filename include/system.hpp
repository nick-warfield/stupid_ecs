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
	Entity make(ComponentConfig<>) {
		if (m_next_alloc.empty()) {
			m_generation.push_back(0);
			m_component.push_back(COMPONENT_ALIVE);
			return Entity(0, m_generation.size() - 1);
		} else {
			auto next = m_next_alloc.top();
			m_next_alloc.pop();
			m_generation[next]++;
			m_component[next] = COMPONENT_ALIVE;
			return Entity(m_generation[next], next);
		}
	}

	void erase(const Entity &id) {
		if (m_generation[id.index] != id.generation) return;
		m_component[id.index] &= ~COMPONENT_ALIVE;
		m_next_alloc.push(id.index);
	}
};


template <typename T, typename ...R>
class System<T, R...> {
public:
	Entity make(ComponentConfig<T, R...> cc) {
		Entity e = m_tail.make(cc.remainder);
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

	bool is_alive(const Entity&);
	bool has_component(const Entity&, component_flag);

	struct Item {
		std::optional<std::reference_wrapper<int>> data1;
		std::optional<std::reference_wrapper<std::string>> data2;
	};
	std::optional<Item> get(const Entity &);

private:
	std::vector<T> m_data;
	System<R...> m_tail;
};
