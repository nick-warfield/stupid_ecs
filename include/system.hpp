#pragma once

#include <optional>
#include <vector>
#include <stack>
#include <memory>

#include "component.hpp"
#include "entity.hpp"

template<typename ...T>
using Item = ComponentConfig<
	std::reference_wrapper<T>...>;

template <typename... T>
struct SystemData;

template <>
struct SystemData<> { };
	
template <typename Head, typename ...Tail>
struct SystemData<Head, Tail...> {
	std::vector<Head> data;
	SystemData<Tail...> tail;
};

template <typename... T>
class System {
	std::stack<std::size_t> m_next_alloc;
	std::vector<std::size_t> m_generation;
	std::vector<component_flag> m_component;
	SystemData<T...> m_data;

public:
	Entity make(ComponentConfig<T...>& cc) {
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

	std::optional<Item<T...>> get(const Entity &id) {
	}

	bool is_alive(const Entity &id) {
		return m_generation[id.index] == id.generation
			&& m_component[id.index] & COMPONENT_ALIVE;
	}
};

	}

	}

	}
		auto value = bits & 1
			: std::nullopt;
	}
};
