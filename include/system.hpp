#pragma once

#include <boost/optional.hpp>
#include <memory>
#include <stack>
#include <vector>

#include "component.hpp"
#include "entity.hpp"

namespace secs
{
namespace details
{
	using bitmask              = uint16_t;
	const bitmask ENTITY_ALIVE = 0b1000000000000000;

	template <typename T>
	void assign_or_push(std::vector<T> &vec, T item, size_t index)
	{
		if (index >= vec.size())
			vec.push_back(item);
		else
			vec[index] = item;
	}

	template <typename T>
	struct SystemHelper;

	template <typename... T>
	struct SystemData;

	template <typename A, typename T>
	struct SystemGetType;
}  // namespace details

template <typename... T>
using Item = ComponentConfig<T &...>;

template <typename...>
struct Filter;

template <>
struct details::SystemData<> {
};

template <typename Head, typename... Tail>
struct details::SystemData<Head, Tail...> {
	std::vector<Head> data;
	SystemData<Tail...> tail;

	template <typename U>
	std::vector<U> &get_data();
};

template <typename... T>
class System
{
	template <typename...>
	friend struct Filter;

	std::stack<std::size_t> m_next_alloc;
	std::vector<std::size_t> m_generation;
	std::vector<details::bitmask> m_component;
	details::SystemData<T...> m_data;

	template <typename Type>
	auto &get_data()
	{
		return details::GetType<Type, details::SystemData<T...>>::get_data(
				m_data);
	}

   public:
	Entity make(ComponentConfig<T...> &cc)
	{
		auto next = m_generation.size();
		if (!m_next_alloc.empty()) {
			next = m_next_alloc.top();
			m_next_alloc.pop();
			m_generation[next]++;
		} else {
			m_generation.push_back(0);
		}

		auto flags = details::SystemHelper<details::SystemData<T...>>::make(
				m_data,
				cc,
				next);

		details::assign_or_push<details::bitmask>(
				m_component,
				details::ENTITY_ALIVE | flags,
				next);

		return Entity(m_generation[next], next);
	}

	void erase(const Entity &id)
	{
		if (!is_alive(id))
			return;
		m_component[id.index] &= ~details::ENTITY_ALIVE;
		m_next_alloc.push(id.index);
	}

	boost::optional<Item<T...>> operator[](const Entity &id)
	{
		if (!is_alive(id))
			return {};
		return details::SystemHelper<details::SystemData<T...>>::get(
				m_data,
				m_component[id.index],
				id.index);
	}

	template <typename Type>
	auto get(const Entity &id)
	{
		return is_alive(id) ? details::
							   SystemGetType<Type, details::SystemData<T...>>::
									   get(m_data,
										   m_component[id.index],
										   id.index)
							: boost::none;
	}

	template <typename... U>
	auto iter()
	{
		return Filter<System<T...>, U...>(*this);
	}

	bool is_alive(const Entity &id)
	{
		return m_generation[id.index] == id.generation
			   && m_component[id.index] & details::ENTITY_ALIVE;
	}
};

template <>
struct details::SystemHelper<details::SystemData<>> {
	static details::bitmask
	make(SystemData<> &, const ComponentConfig<> &, const size_t &)
	{
		return 0;
	}

	static Item<> get(SystemData<> &, const details::bitmask &, const size_t &)
	{
		return Item<>();
	}
};

template <typename Head, typename... Tail>
struct details::SystemHelper<details::SystemData<Head, Tail...>> {
	static details::bitmask make(
			SystemData<Head, Tail...> &data,
			const ComponentConfig<Head, Tail...> &cc,
			const size_t &index)
	{
		assign_or_push(data.data, cc.item.value_or(Head()), index);
		auto b = SystemHelper<SystemData<Tail...>>::make(
						 data.tail,
						 cc.tail,
						 index)
				 << 1;
		return b | cc.item.has_value();
	}

	static Item<Head, Tail...> get(
			SystemData<Head, Tail...> &data,
			const details::bitmask &bits,
			const size_t &index)
	{
		auto value = bits & 1 ? boost::optional<Head &>(data.data[index])
							  : boost::none;
		auto tail  = SystemHelper<SystemData<Tail...>>::get(
                data.tail,
                bits >> 1,
                index);
		return Item<Head, Tail...>(value, tail);
	}
};

template <typename Head, typename... Tail>
struct details::SystemGetType<Head, details::SystemData<Head, Tail...>> {
	static auto get(
			details::SystemData<Head, Tail...> &data,
			const details::bitmask &bits,
			const std::size_t &index)
	{
		return bits & 1 ? boost::optional<Head &>(data.data[index])
						: boost::none;
	}

	static std::vector<Head> &get_data(details::SystemData<Head, Tail...> &data)
	{
		return data.data;
	}

	static details::bitmask get_flag(const details::SystemData<Head, Tail...> &)
	{
		return 1;
	}
};

template <typename Type, typename Head, typename... Tail>
struct details::SystemGetType<Type, details::SystemData<Head, Tail...>> {
	static auto get(
			details::SystemData<Head, Tail...> &data,
			const details::bitmask &bits,
			const std::size_t &index)
	{
		return SystemGetType<Type, SystemData<Tail...>>::get(
				data.tail,
				bits >> 1,
				index);
	}

	static std::vector<Type> &get_data(details::SystemData<Head, Tail...> &data)
	{
		return SystemGetType<Type, SystemData<Tail...>>::get_data(data.tail);
	}

	static details::bitmask get_flag(
			const details::SystemData<Head, Tail...> &data)
	{
		return SystemGetType<Type, SystemData<Tail...>>::get_flag(data.tail)
			   << 1;
	}
};

}  // namespace secs
