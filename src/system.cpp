#include "system.hpp"
#include "component.hpp"
#include "entity.hpp"

template <typename... T>
Entity System::make(ComponentConfig<T...> cc) {
	if (m_next_alloc.empty()) {
		m_generation.push_back(0);
		m_component.push_back(cc.bitmask() | COMPONENT_ALIVE);
		m_data1.push_back(cc.data1.value_or(0));
		m_data2.push_back(cc.data2.value_or(""));
		return Entity(0, m_generation.size() - 1, this);
	} else {
		auto next = m_next_alloc.top();
		m_next_alloc.pop();
		m_generation[next]++;
		m_component[next] = cc.bitmask() | COMPONENT_ALIVE;
		m_data1[next] = cc.data1.value_or(0);
		m_data2[next] = cc.data2.value_or("");
		return Entity(m_generation[next], next, this);
	}
}

void System::erase(const Entity &id) {
	if (m_generation[id.index] != id.generation) return;
	m_component[id.index] = m_component[id.index] & ~COMPONENT_ALIVE;
	m_next_alloc.push(id.index);
}

bool System::is_alive(const Entity& id) {
	if (id.index >= m_generation.size()) return false;

	return (m_component[id.index] & COMPONENT_ALIVE) == COMPONENT_ALIVE
		&& m_generation[id.index] == id.generation;
}
bool System::has_component(const Entity& id, component_flag flag) {
	if (id.index >= m_generation.size()) return false;
	return (m_component[id.index] & flag) == flag;
}

std::optional<System::Item> System::get(const Entity &id) {
	if (!is_alive(id)) return {};

	Item i;
	if (has_component(id, COMPONENT_01_FLAG))
		i.data1 = std::ref(m_data1[id.index]);
	if (has_component(id, COMPONENT_02_FLAG))
		i.data2 = std::ref(m_data2[id.index]);

	return i;
}
