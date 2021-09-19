#include "ecs.hpp"

Entity System::make(ComponentConfig cc) {
	if (m_next_alloc.empty()) {
		m_generation.push_back(0);
		m_component.push_back(cc.bitmask() & COMP_ALIVE);
		return Entity(0, m_generation.size() - 1);
	} else {
		auto next = m_next_alloc.top();
		m_next_alloc.pop();
		m_generation[next]++;
		m_component[next] = cc.bitmask() & COMP_ALIVE;
		return Entity(m_generation[next], next);
	}
}

void System::erase(const Entity &id) {
	if (m_generation[id.index] != id.generation) return;
	m_component[id.index] = m_component[id.index] & ~COMP_ALIVE;
	m_next_alloc.push(id.index);
}

bool System::is_alive(const Entity& id) {
	return (m_component[id.index] & COMP_ALIVE) == COMP_ALIVE
		&& m_generation[id.index] == id.generation;
}
bool System::has_component(const Entity& id, component_flags flag) {
	return (m_component[id.index] & flag) == flag;
}

std::optional<System::Item> System::operator[](const Entity &id) {
	if (!is_alive(id)) return {};

	Item i;
	if (has_component(id, COMP_DATA1))
		i.data1 = std::make_unique<int>(m_data1[id.index]);
	if (has_component(id, COMP_DATA2))
		i.data2 = std::make_unique<std::string>(m_data2[id.index]);

	return i;
}
