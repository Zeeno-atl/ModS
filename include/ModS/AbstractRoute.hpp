#pragma once
#ifndef _MODS_ABSTRACT_ROUTE_HPP
#	define _MODS_ABSTRACT_ROUTE_HPP

#	include <memory>
#	include <string>
#	include <vector>

namespace ModS {
class AbstractRoute {
public:
	virtual ~AbstractRoute() = default;

	virtual std::string interfaceName() const      = 0;
	virtual std::string implementationName() const = 0;
};
} // namespace ModS

#endif
