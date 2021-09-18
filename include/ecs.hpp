#pragma once

#include <fstream>
#include <vector>
#include <stack>
#include <functional>

struct Entity {
	Entity(std::size_t gen, std::size_t index)
		: generation(gen), index(index) { }
	const std::size_t generation;
	const std::size_t index;
};

using comp_flags = unsigned short;
struct ComponentConfig {
	// just sets everything to nullopt
	ComponentConfig();
	// use yaml files for configs
	ComponentConfig(std::ifstream);

	// use a function so users don't need to pass in every
	// component they want at once
	comp_flags bitfield();

	// optional<name>
	// optional<position>
	// optional<velocity>
	// and so on, write a macro for this
};

class System {
public:
	System();
	Entity create_entity(ComponentConfig);
	void remove_entity(const Entity &);
	void for_each(comp_flags, std::is_function<void>);

private:
	std::stack<std::size_t> m_next_alloc;
	std::vector<std::size_t> m_generation;
	std::vector<comp_flags> m_component;

	// vec<name>
	// vec<position>
	// vec<velocity>
	// and so on, write a macro for this
};
