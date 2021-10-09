#pragma once

#include <boost/optional.hpp>
#include <stack>
#include <tuple>
#include <vector>

namespace secs
{
struct Entity;

template <typename...>
struct ComponentConfig;

template <typename... T>
ComponentConfig(T...) -> ComponentConfig<T...>;

// just used as the output of system[entity]
// probably makes more sense to just return a tuple
template <typename... T>
using Item = ComponentConfig<T &...>;

template <typename...>
class System;

template <typename...>
struct Filter;

namespace detail
{
	using bitmask              = uint16_t;
	const bitmask ENTITY_ALIVE = 0b1000000000000000;

	template <typename... T>
	struct SystemData;
	template <>
	struct SystemData<> {
	};
	template <typename Head, typename... Tail>
	struct SystemData<Head, Tail...> {
		std::vector<Head> data;
		SystemData<Tail...> tail;
	};

	template <typename T>
	void assign_or_push(std::vector<T> &vec, T item, size_t index)
	{
		if (index >= vec.size())
			vec.push_back(item);
		else
			vec[index] = item;
	}

	template <uint I, typename T>
	struct GetIndex;

	template <typename A, typename T>
	struct GetType;

	template <typename T>
	struct SystemHelper {
		// unrolls SystemData & ComponentConfig to write Components to
		// SystemData additionally, generates bitmask based on given Components
		template <typename... U>
		static bitmask
		make(SystemData<U...> &, const ComponentConfig<U...> &, const size_t &);

		template <typename... U>
		static auto &get(SystemData<U...> &, const bitmask &, const size_t &);
	};
}  // namespace detail

struct Entity {
	const std::size_t generation;
	const std::size_t index;

	Entity &operator=(const Entity &) { return *this; }

	Entity(const Entity &other) :
		generation(other.generation),
		index(other.index)
	{
	}

   private:
	template <typename... T>
	friend class System;

	template <typename... T>
	friend struct Filter;

	Entity(std::size_t gen, std::size_t index) : generation(gen), index(index)
	{
	}
};

template <>
struct ComponentConfig<> {
};
template <typename T, typename... R>
struct ComponentConfig<T, R...> {
	ComponentConfig() : item(boost::none), tail() {}

	ComponentConfig(const ComponentConfig<T, R...> &other) :
		item(other.item),
		tail(other.tail)
	{
	}

	ComponentConfig(
			boost::optional<T> value,
			const ComponentConfig<R...> &other) :
		item(value),
		tail(other)
	{
	}

	ComponentConfig(T value, R... rest) : item(value), tail(rest...) {}

	template <uint Index>
	auto &get()
	{
		return detail::GetIndex<Index, ComponentConfig<T, R...>>::get(*this);
	}

	template <typename Type>
	auto &get()
	{
		return detail::GetType<Type, ComponentConfig<T, R...>>::get(*this);
	}

	boost::optional<T> item;
	ComponentConfig<R...> tail;
};

template <typename... T>
class System
{
	template <typename...>
	friend struct Filter;

	std::stack<std::size_t> m_next_alloc;
	std::vector<std::size_t> m_generation;
	std::vector<detail::bitmask> m_component;
	detail::SystemData<T...> m_data;

	template <typename Type>
	auto &get_data()
	{
		return detail::GetType<Type, detail::SystemData<T...>>::get_data(
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

		auto flags = detail::SystemHelper<detail::SystemData<T...>>::make(
				m_data,
				cc,
				next);

		detail::assign_or_push<detail::bitmask>(
				m_component,
				detail::ENTITY_ALIVE | flags,
				next);

		return Entity(m_generation[next], next);
	}

	void erase(const Entity &id)
	{
		if (!is_alive(id))
			return;
		m_component[id.index] &= ~detail::ENTITY_ALIVE;
		m_next_alloc.push(id.index);
	}

	boost::optional<Item<T...>> operator[](const Entity &id)
	{
		if (!is_alive(id))
			return {};
		return detail::SystemHelper<detail::SystemData<T...>>::get(
				m_data,
				m_component[id.index],
				id.index);
	}

	template <typename Type>
	auto get(const Entity &id)
	{
		return is_alive(id)
					   ? detail::GetType<Type, detail::SystemData<T...>>::
							   get(m_data, m_component[id.index], id.index)
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
			   && m_component[id.index] & detail::ENTITY_ALIVE;
	}
};

template <typename... T>
struct Filter<System<T...>> {
	Filter(System<T...> &sys) : m_ptr(&sys) {}

	// this should really be a part of SystemHelper
	static detail::bitmask mask(detail::SystemData<T...> &)
	{
		return detail::ENTITY_ALIVE;
	}

	// this should really be a part of SystemHelper
	static std::tuple<> get(detail::SystemData<T...> &, const size_t &)
	{
		return std::make_tuple<>();
	}

	struct Iterator {
		Iterator(size_t index, System<T...> *ptr) :
			m_index(index),
			m_mask(detail::ENTITY_ALIVE),
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
		const detail::bitmask m_mask;
		System<T...> *const m_ptr;
	};

	const Iterator begin() const
	{
		size_t index = 0;
		while (index < m_ptr->m_component.size()
			   && (m_ptr->m_component[index] & detail::ENTITY_ALIVE)
						  != detail::ENTITY_ALIVE) {
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
	static detail::bitmask mask(detail::SystemData<T...> &data)
	{
		return detail::GetType<Data1, detail::SystemData<T...>>::get_flag(
					   data)
			   | Filter<System<T...>, DataN...>::mask(data);
	}

	// this should really be a part of SystemHelper
	static std::tuple<Data1 &, DataN &...> get(
			detail::SystemData<T...> &data,
			const size_t index)
	{
		return std::tuple_cat(
				std::tuple<Data1 &>(
						detail::GetType<Data1, detail::SystemData<T...>>::
								get_data(data)[index]),
				Filter<System<T...>, DataN...>::get(data, index));
	}

	struct Iterator {
		Iterator(size_t index, detail::bitmask mask, System<T...> *ptr) :
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
		const detail::bitmask m_mask;
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
	const detail::bitmask m_mask;
};

namespace detail
{
	template <>
	struct SystemHelper<SystemData<>> {
		static bitmask
		make(SystemData<> &, const ComponentConfig<> &, const size_t &)
		{
			return 0;
		}

		static Item<> get(SystemData<> &, const bitmask &, const size_t &)
		{
			return Item<>();
		}
	};

	template <typename Head, typename... Tail>
	struct SystemHelper<SystemData<Head, Tail...>> {
		static bitmask make(
				SystemData<Head, Tail...> &data,
				const ComponentConfig<Head, Tail...> &cc,
				const size_t &index)
		{
			auto mask = SystemHelper<SystemData<Tail...>>::make(
								data.tail,
								cc.tail,
								index)
						<< 1;
			assign_or_push(data.data, cc.item.value_or(Head()), index);
			return mask | cc.item.has_value();
		}

		static Item<Head, Tail...> get(
				SystemData<Head, Tail...> &data,
				const bitmask &bits,
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
	struct GetIndex<0, ComponentConfig<Head, Tail...>> {
		static boost::optional<Head> &get(ComponentConfig<Head, Tail...> &cc)
		{
			return cc.item;
		}
	};

	template <uint Index, typename Head, typename... Tail>
	struct GetIndex<Index, ComponentConfig<Head, Tail...>> {
		static auto &get(ComponentConfig<Head, Tail...> &cc)
		{
			return GetIndex<Index - 1, ComponentConfig<Tail...>>::get(cc.tail);
		}
	};

	template <typename Head, typename... Tail>
	struct GetType<Head, ComponentConfig<Head, Tail...>> {
		static boost::optional<Head> &get(ComponentConfig<Head, Tail...> &cc)
		{
			return cc.item;
		}
	};

	template <typename Type, typename Head, typename... Tail>
	struct GetType<Type, ComponentConfig<Head, Tail...>> {
		static auto &get(ComponentConfig<Head, Tail...> &cc)
		{
			return GetType<Type, ComponentConfig<Tail...>>::get(cc.tail);
		}
	};

	template <typename Head, typename... Tail>
	struct GetType<Head, SystemData<Head, Tail...>> {
		static auto get(
				SystemData<Head, Tail...> &data,
				const bitmask &bits,
				const std::size_t &index)
		{
			return bits & 1 ? boost::optional<Head &>(data.data[index])
							: boost::none;
		}

		static std::vector<Head> &get_data(SystemData<Head, Tail...> &data)
		{
			return data.data;
		}

		static bitmask get_flag(const SystemData<Head, Tail...> &) { return 1; }
	};

	template <typename Type, typename Head, typename... Tail>
	struct GetType<Type, SystemData<Head, Tail...>> {
		static auto get(
				SystemData<Head, Tail...> &data,
				const bitmask &bits,
				const std::size_t &index)
		{
			return GetType<Type, SystemData<Tail...>>::get(
					data.tail,
					bits >> 1,
					index);
		}

		static std::vector<Type> &get_data(SystemData<Head, Tail...> &data)
		{
			return GetType<Type, SystemData<Tail...>>::get_data(data.tail);
		}

		static bitmask get_flag(const SystemData<Head, Tail...> &data)
		{
			return GetType<Type, SystemData<Tail...>>::get_flag(data.tail)
				   << 1;
		}
	};

}  // namespace detail

}  // namespace secs
