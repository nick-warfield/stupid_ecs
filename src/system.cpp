

// bool System::is_alive(const Entity& id) {
// 	if (id.index >= m_generation.size()) return false;
// 
// 	return (m_component[id.index] & COMPONENT_ALIVE) == COMPONENT_ALIVE
// 		&& m_generation[id.index] == id.generation;
// }
// bool System::has_component(const Entity& id, component_flag flag) {
// 	if (id.index >= m_generation.size()) return false;
// 	return (m_component[id.index] & flag) == flag;
// }
// 
// std::optional<System::Item> System::get(const Entity &id) {
// 	if (!is_alive(id)) return {};
// 
// 	Item i;
// 	if (has_component(id, COMPONENT_01_FLAG))
// 		i.data1 = std::ref(m_data1[id.index]);
// 	if (has_component(id, COMPONENT_02_FLAG))
// 		i.data2 = std::ref(m_data2[id.index]);
// 
// 	return i;
// }
