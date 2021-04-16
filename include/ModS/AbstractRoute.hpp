#pragma once
#ifndef _MODS_ABSTRACT_ROUTE_HPP
#	define _MODS_ABSTRACT_ROUTE_HPP

#	include <string>

namespace ModS {
class AbstractRoute {
public:
	virtual ~AbstractRoute() = default;

	virtual std::string interfaceName() const      = 0;
	virtual std::string implementationName() const = 0;
	virtual std::int32_t priority() const          = 0;
};
} // namespace ModS

#endif
