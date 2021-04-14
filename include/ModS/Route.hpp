#pragma once
#ifndef _MODS_ROUTE_HPP
#	define _MODS_ROUTE_HPP

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
} // namespace ModS

#endif
