module;

#include <fstream>
#include <iostream>
#include <boost/optional.hpp>

export module stupid_ecs.component;

namespace secs {

namespace details {
template<uint I, typename T>
struct GetIndex;

template<typename A, typename T>
struct GetType;
}

export template<typename...>
struct ComponentConfig { };

export template<typename T, typename... R>
struct ComponentConfig<T, R...> {
	ComponentConfig() :
		item(boost::none),
		tail() { }

	ComponentConfig(const ComponentConfig<T, R...> &other) :
		item(other.item),
		tail(other.tail) { }

	ComponentConfig(
			boost::optional<T> value,
			const ComponentConfig<R...> &other) :
		item(value),
		tail(other) { }

	ComponentConfig(
			T value,
			R... rest) :
		item(value),
		tail(rest...) { }

	template <uint Index>
	auto& get() {
		return details::GetIndex<Index, ComponentConfig<T, R...>>::get(*this);
	}

	template <typename Type>
	auto& get() {
		return details::GetType<Type, ComponentConfig<T, R...>>::get(*this);
	}

	boost::optional<T> item;
	ComponentConfig<R...> tail;
};

export template <typename...T>
ComponentConfig(T...) -> ComponentConfig<T...>;

template <typename Head, typename... Tail>
struct details::GetIndex<0, ComponentConfig<Head, Tail...>> {
	static boost::optional<Head>& get(ComponentConfig<Head, Tail...>& cc) {
		return cc.item;
	}
};

template <uint Index, typename Head, typename... Tail>
struct details::GetIndex<Index, ComponentConfig<Head, Tail...>> {
	static auto& get(ComponentConfig<Head, Tail...>& cc) {
		return GetIndex<Index - 1, ComponentConfig<Tail...>>::get(cc.tail);
	}
};

template <typename Head, typename... Tail>
struct details::GetType<Head, ComponentConfig<Head, Tail...>> {
	static boost::optional<Head>& get(ComponentConfig<Head, Tail...>& cc) {
		return cc.item;
	}
};

template <typename Type, typename Head, typename... Tail>
struct details::GetType<Type, ComponentConfig<Head, Tail...>> {
	static auto& get(ComponentConfig<Head, Tail...>& cc) {
		return GetType<Type, ComponentConfig<Tail...>>::get(cc.tail);
	}
};

}

