#include <iostream>
#include "ecs.hpp"

component_flags ComponentConfig::bitmask() {
	return COMP_ALIVE
		| (data1 ? COMP_DATA1 : 0)
		| (data2 ? COMP_DATA2 : 0);
}

Entity System::make(ComponentConfig cc) {
	if (m_next_alloc.empty()) {
		m_generation.push_back(0);
		m_component.push_back(cc.bitmask() | COMP_ALIVE);
		m_data1.push_back(cc.data1.value_or(0));
		m_data2.push_back(cc.data2.value_or(""));
		return Entity(0, m_generation.size() - 1);
	} else {
		auto next = m_next_alloc.top();
		m_next_alloc.pop();
		m_generation[next]++;
		m_component[next] = cc.bitmask() | COMP_ALIVE;
		m_data1[next] = cc.data1.value_or(0);
		m_data2[next] = cc.data2.value_or("");
		return Entity(m_generation[next], next);
	}
}

void System::erase(const Entity &id) {
	if (m_generation[id.index] != id.generation) return;
	m_component[id.index] = m_component[id.index] & ~COMP_ALIVE;
	m_next_alloc.push(id.index);
}

bool System::is_alive(const Entity& id) {
	if (id.index >= m_generation.size()) return false;

	return (m_component[id.index] & COMP_ALIVE) == COMP_ALIVE
		&& m_generation[id.index] == id.generation;
}
bool System::has_component(const Entity& id, component_flags flag) {
	if (id.index >= m_generation.size()) return false;
	return (m_component[id.index] & flag) == flag;
}

std::optional<System::Item> System::get(const Entity &id) {
	if (!is_alive(id)) return {};
	Item i;
	if (has_component(id, COMP_DATA1))
		i.data1 = m_data1[id.index];
	if (has_component(id, COMP_DATA2))
		i.data2 = m_data2[id.index];

	return i;
}
