#pragma once
#ifndef _MODS_ROUTE_HPP
#	define _MODS_ROUTE_HPP

#	include <cstddef>
#	include <ModS/AbstractRoute.hpp>
#	include <ModS/Typename.hpp>

namespace ModS {

template<typename Interface, typename Implementation>
class Route : public AbstractRoute {
public:
	std::string implementationName() const override {
		return pretty_name<Implementation>();
	}

	std::string interfaceName() const override {
		return pretty_name<Interface>();
	}
};

template<>
class Route<std::nullptr_t, std::nullptr_t> : public AbstractRoute {
	std::string interface, implementation;

public:
	Route(std::string interface, std::string implementation) : interface(std::move(interface)), implementation(std::move(implementation)) {
	}

	std::string implementationName() const override {
		return implementation;
	}

	std::string interfaceName() const override {
		return interface;
	}
};

} // namespace ModS

#endif
