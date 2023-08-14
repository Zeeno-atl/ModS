#pragma once
#ifndef _MODS_ABSTRACT_ROUTE_HPP
#	define _MODS_ABSTRACT_ROUTE_HPP

#	include <cstdint>
#	include <memory>
#	include <string>

namespace ModS {
class AbstractRoute {
public:
	virtual ~AbstractRoute() = default;

	virtual std::string  interfaceName() const      = 0;
	virtual std::string  implementationName() const = 0;
	virtual std::int32_t priority() const           = 0;

	/*
	 * This is a function that casts implementation to its interface (pointer can be shifted)
	 * It is important in case that implementation has its interface on non-first inheritance place,
	 * so after creation of the instance, its pointer needs to be shifted to obtain proper
	 * pointer to its interface.
	 */
	virtual std::shared_ptr<void> forwardCast(std::shared_ptr<void> implementation) const = 0;
	virtual void*                 forwardCast(void* implementation) const                 = 0;
};
} // namespace ModS

#endif
