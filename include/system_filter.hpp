#pragma once

#include <tuple>
#include <vector>

#include "system.hpp"

namespace secs
{
template <typename... U>
struct Filter;

template <typename... T>
struct Filter<System<T...>> {
	Filter(System<T...> &sys) : m_ptr(&sys) {}

	// this should really be a part of SystemHelper
	static details::bitmask mask(details::SystemData<T...> &)
	{
		return details::ENTITY_ALIVE;
	}

	// this should really be a part of SystemHelper
	static std::tuple<> get(details::SystemData<T...> &, const size_t &)
	{
		return std::make_tuple<>();
	}

	struct Iterator {
		Iterator(size_t index, System<T...> *ptr) :
			m_index(index),
			m_mask(details::ENTITY_ALIVE),
			m_ptr(ptr)
		{
		}

		Item<T...> operator*() const
		{
			return *(*m_ptr)[Entity(m_ptr->m_generation[m_index], m_index)];
		}

		bool operator==(const Iterator &other) const
		{
			return m_index == other.m_index && m_mask == other.m_mask;
		}
		bool operator!=(const Iterator &other) const
		{
			return m_index != other.m_index || m_mask != other.m_mask;
		}

		Iterator &operator++()
		{
			do {
				m_index++;
			} while (m_index < m_ptr->m_component.size()
					 && (m_ptr->m_component[m_index] & m_mask) != m_mask);
			return *this;
		}

	   private:
		std::size_t m_index;
		const details::bitmask m_mask;
		System<T...> *const m_ptr;
	};

	const Iterator begin() const
	{
		size_t index = 0;
		while (index < m_ptr->m_component.size()
			   && (m_ptr->m_component[index] & details::ENTITY_ALIVE)
						  != details::ENTITY_ALIVE) {
			index++;
		}

		return Iterator(index, m_ptr);
	}

	const Iterator end() const
	{
		return Iterator(m_ptr->m_component.size(), m_ptr);
	}

   private:
	System<T...> *m_ptr;
};

template <typename Data1, typename... DataN, typename... T>
struct Filter<System<T...>, Data1, DataN...> {
	Filter(System<T...> &sys) :
		m_ptr(&sys),
		m_mask(Filter<System<T...>, Data1, DataN...>::mask(sys.m_data))
	{
	}

	// this should really be a part of SystemHelper
	static details::bitmask mask(details::SystemData<T...> &data)
	{
		return details::SystemGetter<Data1, details::SystemData<T...>>::
					   get_flag(data)
			   | Filter<System<T...>, DataN...>::mask(data);
	}

	// this should really be a part of SystemHelper
	static std::tuple<Data1 &, DataN &...> get(
			details::SystemData<T...> &data,
			const size_t index)
	{
		return std::tuple_cat(
				std::tuple<Data1 &>(details::SystemGetter<
									Data1,
									details::SystemData<T...>>::get_data(data)
											[index]),
				Filter<System<T...>, DataN...>::get(data, index));
	}

	struct Iterator {
		Iterator(size_t index, details::bitmask mask, System<T...> *ptr) :
			m_index(index),
			m_mask(mask),
			m_ptr(ptr)
		{
		}

		std::tuple<Data1 &, DataN &...> operator*() const
		{
			return Filter<System<T...>, Data1, DataN...>::get(
					m_ptr->m_data,
					m_index);
		}

		bool operator==(const Iterator &other) const
		{
			return m_index == other.m_index && m_mask == other.m_mask;
		}
		bool operator!=(const Iterator &other) const
		{
			return m_index != other.m_index || m_mask != other.m_mask;
		}

		Iterator &operator++()
		{
			do {
				m_index++;
			} while (m_index < m_ptr->m_component.size()
					 && (m_ptr->m_component[m_index] & m_mask) != m_mask);
			return *this;
		}

	   private:
		std::size_t m_index;
		const details::bitmask m_mask;
		System<T...> *const m_ptr;
	};

	const Iterator begin() const
	{
		size_t index = 0;
		while (index < m_ptr->m_component.size()
			   && (m_ptr->m_component[index] & m_mask) != m_mask) {
			index++;
		}

		return Iterator(index, m_mask, m_ptr);
	}

	const Iterator end() const
	{
		return Iterator(m_ptr->m_component.size(), m_mask, m_ptr);
	}

   private:
	System<T...> *const m_ptr;
	const details::bitmask m_mask;
};

}  // namespace secs
