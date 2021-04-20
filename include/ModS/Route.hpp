#pragma once
#ifndef _MODS_ROUTE_HPP
#	define _MODS_ROUTE_HPP

#	include <cstddef>
#	include <ModS/AbstractRoute.hpp>
#	include <ModS/Typename.hpp>

namespace ModS {

template<typename Interface, typename Implementation>
class Route : public AbstractRoute {
	std::int32_t _priority{};
public:
	Route(std::int32_t priority = 0)
		: _priority(priority) {
	}

	std::string implementationName() const override {
		return pretty_name<Implementation>();
	}

	std::string interfaceName() const override {
		return pretty_name<Interface>();
	}

	std::int32_t priority() const override {
		return _priority;
	}
};

template<>
class Route<std::nullptr_t, std::nullptr_t> : public AbstractRoute {
	std::string  interface, implementation;
	std::int32_t _priority{};
public:
	Route(std::string interface, std::string implementation, std::int32_t priority = 0)
		: interface(std::move(interface)), implementation(std::move(implementation)), _priority(priority) {
	}

	std::string implementationName() const override {
		return implementation;
	}

	std::string interfaceName() const override {
		return interface;
	}

	std::int32_t priority() const override {
		return _priority;
	}
};

} // namespace ModS

#endif
