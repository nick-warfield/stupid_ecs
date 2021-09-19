#include "component.hpp"

component_flags ComponentConfig::bitmask() {
	return COMP_ALIVE
		| (data1 ? COMP_DATA1 : 0)
		| (data2 ? COMP_DATA2 : 0);
}

