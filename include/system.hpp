#pragma once

#include <optional>
#include <vector>
#include <stack>
#include <memory>

#include "component.hpp"

struct Entity;

class System {
public:
	Entity make(ComponentConfig);
	void erase(const Entity &);

	bool is_alive(const Entity&);
	bool has_component(const Entity&, component_flags);

	struct Item {
		std::optional<std::reference_wrapper<int>> data1;
		std::optional<std::reference_wrapper<std::string>> data2;
	};
	std::optional<Item> get(const Entity &);

private:
	std::stack<std::size_t> m_next_alloc;
	std::vector<std::size_t> m_generation;
	std::vector<component_flags> m_component;

	std::vector<int> m_data1;
	std::vector<std::string> m_data2;

	// vec<name>
	// vec<position>
	// vec<velocity>
	// and so on, write a macro for this
};
