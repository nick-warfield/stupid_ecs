#pragma once

#include <fstream>
#include <optional>
#include <iostream>

using component_flag = uint16_t;
const component_flag COMPONENT_ALIVE   = 0b1000000000000000;
const component_flag COMPONENT_01_FLAG = 0b0000000000000001;
const component_flag COMPONENT_02_FLAG = 0b0000000000000010;
const component_flag COMPONENT_03_FLAG = 0b0000000000000100;

template<uint I, typename T>
struct GetIndex;

template<typename A, typename T>
struct GetType;

template<typename...>
struct ComponentConfig {
	component_flag bitmask() { return 0; }
};

template<typename T, typename... R>
struct ComponentConfig<T, R...> {
	ComponentConfig() :
		item(std::nullopt),
		tail() { }

	ComponentConfig(const ComponentConfig<T, R...> &other) :
		item(other.item),
		tail(other.tail) { }

	ComponentConfig(std::optional<T> value, const ComponentConfig<R...> &other)
		: item(value),
		tail(other) { }

	component_flag bitmask() {
		return item.has_value()
			| (tail.bitmask() << 1);
	}

	template <uint Index>
	auto& get() {
		return GetIndex<Index, ComponentConfig<T, R...>>::get(*this);
	}

	template <typename Type>
	auto& get() {
		return GetType<Type, ComponentConfig<T, R...>>::get(*this);
	}

	std::optional<T> item;
	ComponentConfig<R...> tail;
};

// 0 case -> what we looking for
template <typename Head, typename... Tail>
struct GetIndex<0, ComponentConfig<Head, Tail...>> {
	static std::optional<Head>& get(ComponentConfig<Head, Tail...>& cc) {
		return cc.item;
	}
};

// recursive case
template <uint Index, typename Head, typename... Tail>
struct GetIndex<Index, ComponentConfig<Head, Tail...>> {
	static auto& get(ComponentConfig<Head, Tail...>& cc) {
		return GetIndex<Index - 1, ComponentConfig<Tail...>>::get(cc.tail);
	}
};

template <typename Head, typename... Tail>
struct GetType<Head, ComponentConfig<Head, Tail...>> {
	static std::optional<Head>& get(ComponentConfig<Head, Tail...>& cc) {
		return cc.item;
	}
};

template <typename Type, typename Head, typename... Tail>
struct GetType<Type, ComponentConfig<Head, Tail...>> {
	static auto& get(ComponentConfig<Head, Tail...>& cc) {
		return GetType<Type, ComponentConfig<Tail...>>::get(cc.tail);
	}
};
