#pragma once

#include <fstream>
#include <vector>
#include <stack>
#include <optional>
#include <memory>

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

using component_flags = uint16_t;
const component_flags COMP_ALIVE = 0b0000000000000001;
const component_flags COMP_DATA1 = 0b0000000000000010;
const component_flags COMP_DATA2 = 0b0000000000000100;

struct ComponentConfig {
	// just sets everything to nullopt
	ComponentConfig() : data1(std::nullopt), data2(std::nullopt) { }
	// use yaml files for configs
	ComponentConfig(std::ifstream);

	component_flags bitmask();

	std::optional<int> data1;
	std::optional<std::string> data2;

	// optional<name>
	// optional<position>
	// optional<velocity>
	// and so on, write a macro for this
};

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
