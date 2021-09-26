#pragma once

#include <fstream>
#include <optional>
#include <iostream>

template<uint I, typename T>
struct GetIndex;

template<typename A, typename T>
struct GetType;

template<typename...>
struct ComponentConfig { };

template<typename T, typename... R>
struct ComponentConfig<T, R...> {
	ComponentConfig() :
		item(std::nullopt),
		tail() { }

	ComponentConfig(const ComponentConfig<T, R...> &other) :
		item(other.item),
		tail(other.tail) { }

	ComponentConfig(
			std::optional<T> value,
			const ComponentConfig<R...> &other) :
		item(value),
		tail(other) { }

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
