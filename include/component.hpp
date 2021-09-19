#pragma once

#include <fstream>
#include <optional>

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

