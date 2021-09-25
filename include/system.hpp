#pragma once

#include <optional>
#include <vector>
#include <stack>
#include <memory>

#include "component.hpp"
#include "entity.hpp"

template <typename T>
void assign_or_push(std::vector<T>& vec, T item, size_t index) {
	if (index >= vec.size())
		vec.push_back(item);
	else
		vec[index] = item;
}

template<typename ...T>
using Item = ComponentConfig<
	std::reference_wrapper<T>...>;

template <typename T>
struct SystemHelper;

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
		auto next = m_generation.size();
		if (!m_next_alloc.empty()) {
			next = m_next_alloc.top();
			m_next_alloc.pop();
			m_generation[next]++;
		} else {
			m_generation.push_back(0);
		}

		auto flags = SystemHelper<SystemData<T...>>::make(m_data, cc, next);
		assign_or_push<component_flag>(m_component, COMPONENT_ALIVE | flags, next);
		return Entity(m_generation[next], next);
	}

	void erase(const Entity& id) {
		if (m_generation[id.index] != id.generation) return;
		m_component[id.index] &= ~COMPONENT_ALIVE;
		m_next_alloc.push(id.index);
	}

	std::optional<Item<T...>> get(const Entity &id) {
		if (!is_alive(id)) return {};
		return SystemHelper<SystemData<T...>>::get(m_data, m_component[id.index], id.index);
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
