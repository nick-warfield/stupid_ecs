#pragma once

#include <fstream>
#include <optional>
#include <iostream>

using component_flag = uint16_t;
const component_flag COMPONENT_ALIVE   = 0b1000000000000000;
const component_flag COMPONENT_01_FLAG = 0b0000000000000001;
const component_flag COMPONENT_02_FLAG = 0b0000000000000010;
const component_flag COMPONENT_03_FLAG = 0b0000000000000100;

template<component_flag F, typename T>
struct GetHelper;

template<typename... T>
struct ComponentConfig {
	component_flag bitmask() { return 0; }
};

template<typename T, typename... R>
struct ComponentConfig<T, R...> {
	ComponentConfig() :
		component(std::nullopt),
		remainder() { }

	ComponentConfig(const ComponentConfig<T, R...> &other)
		: component(other.component),
		remainder(other.remainder) { }

	ComponentConfig(std::optional<T> value, const ComponentConfig<R...> &other)
		: component(value),
		remainder(other) { }

	component_flag bitmask() {
		return component.has_value()
			| (remainder.bitmask() << 1);
	}

	template <component_flag flag>
	auto& get() {
		return GetHelper<flag, ComponentConfig<T, R...>>::get(*this);
	}

	std::optional<T> component;
	ComponentConfig<R...> remainder;
};

// 0 case -> nullopt (mostly to avoid crashes)
template <typename T, typename... R>
struct GetHelper<0, ComponentConfig<T, R...>> {
	static auto& get(ComponentConfig<T, R...>&) {
		return std::nullopt;
	}
};

// 1 case -> what we looking for
template <typename T, typename... R>
struct GetHelper<1, ComponentConfig<T, R...>> {
	static std::optional<T>& get(ComponentConfig<T, R...>& cc) {
		return cc.component;
	}
};

// recursive case
template <component_flag F, typename T, typename... R>
struct GetHelper<F, ComponentConfig<T, R...>> {
	static auto& get(ComponentConfig<T, R...>& cc) {
		return GetHelper<(F >> 1), ComponentConfig<R...>>::get(cc.remainder);
	}
};
