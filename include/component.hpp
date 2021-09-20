#pragma once

// Component could also be a recursive struct
// for the getter, it could be get<component_flag>()
// where we bitshift each step

#include <fstream>
#include <optional>

using component_flag = uint16_t;
const component_flag COMPONENT_ALIVE   = 0b0000000000000001;
const component_flag COMPONENT_01_FLAG = 0b0000000000000010;
const component_flag COMPONENT_02_FLAG = 0b0000000000000100;

template<component_flag F, typename T>
struct get_helper;

template<typename... T>
struct ComponentConfig { };

template<typename T, typename... R>
struct ComponentConfig<T, R...> {
	ComponentConfig() :
		component(std::nullopt),
		remainder() { }

	component_flag bitmask() {
		return COMPONENT_ALIVE
			| (component ? COMPONENT_01_FLAG : 0)
			| remainder.bitmask();
	}

	template <component_flag flag>
	auto get() {
		return get_helper<flag, ComponentConfig<T, R...>>::get(*this);
	}

	std::optional<T> component;
	ComponentConfig<R...> remainder;
};

template <typename T, typename... R>
struct get_helper<0, ComponentConfig<T, R...>> {
	static auto get(ComponentConfig<T, R...>&) {
		return std::nullopt;
	}
};

template <typename T, typename... R>
struct get_helper<1, ComponentConfig<T, R...>> {
	static auto get(ComponentConfig<T, R...>&) {
		return std::nullopt;
	}
};

template <typename T, typename... R>
struct get_helper<2, ComponentConfig<T, R...>> {
	static std::optional<T> get(ComponentConfig<T, R...>& cc) {
		return cc.component;
	}
};

template <component_flag F, typename T, typename... R>
struct get_helper<F, ComponentConfig<T, R...>> {
	static auto get(ComponentConfig<T, R...>& cc) {
		return get_helper<F/2, ComponentConfig<R...>>::get(cc.remainder);
	}
};

