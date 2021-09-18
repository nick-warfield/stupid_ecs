#include "ecs.hpp"

System::System() {
	m_next_alloc.push(0);
}

Entity System::create_entity(ComponentConfig cc) {
	if (m_next_alloc.empty()) {
		m_generation.push_back(0);
		m_component.push_back(cc.bitfield());
		return Entity(0, m_generation.size() - 1);
	} else {
		auto next = m_next_alloc.top();
		m_next_alloc.pop();
		m_generation[next]++;
		m_component[next] = cc.bitfield();
		return Entity(m_generation[next], next);
	}
}

void System::remove_entity(const Entity &id) {
}

