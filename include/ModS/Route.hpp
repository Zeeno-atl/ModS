#pragma once
#ifndef _MODS_ROUTE_HPP
#	define _MODS_ROUTE_HPP

#	include <cstddef>
#	include <ModS/AbstractRoute.hpp>
#	include <ModS/Typename.hpp>

namespace ModS {

template<typename Interface, typename Implementation>
requires std::is_convertible_v<Implementation*, Interface*>
class Route : public AbstractRoute {
	std::int32_t _priority{};

public:
	Route(std::int32_t priority = 0) : _priority(priority) {
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

	std::shared_ptr<void> forwardCast(std::shared_ptr<void> implementation) const override {
		std::shared_ptr<Implementation> impl = std::static_pointer_cast<Implementation>(implementation);
		return std::static_pointer_cast<Interface>(impl);
	}

	void* forwardCast(void* implementation) const override {
		return static_cast<Interface*>(static_cast<Implementation*>(implementation));
	}
};

template<typename Interface, typename Implementation>
class FactoryRoute : public AbstractRoute {
	std::int32_t _priority{};

public:
	FactoryRoute(std::int32_t priority = 10) : _priority(priority) {
	}

	std::string implementationName() const override {
		return pretty_name<ImplementationFactory<Implementation>>();
	}

	std::string interfaceName() const override {
		return pretty_name<Interface>();
	}

	std::int32_t priority() const override {
		return _priority;
	}

	std::shared_ptr<void> forwardCast(std::shared_ptr<void> implementation) const override {
		std::shared_ptr<Implementation> impl = std::static_pointer_cast<Implementation>(implementation);
		return std::static_pointer_cast<Interface>(impl);
	}

	void* forwardCast(void* implementation) const override {
		return static_cast<Interface*>(static_cast<Implementation*>(implementation));
	}
};

template<>
class Route<std::nullptr_t, std::nullptr_t> : public AbstractRoute {
	std::string                    interface;
	std::string                    implementation;
	std::int32_t                   _priority{};
	std::shared_ptr<AbstractRoute> castable;

public:
	Route(std::shared_ptr<AbstractRoute> castable, std::string interface, std::string implementation, std::int32_t priority = 0)
	    : interface(std::move(interface)), implementation(std::move(implementation)), _priority(priority), castable(std::move(castable)) {
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

	std::shared_ptr<void> forwardCast(std::shared_ptr<void> ptr) const override {
		return castable->forwardCast(ptr);
	}

	void* forwardCast(void* impl) const override {
		return castable->forwardCast(impl);
	}
};

} // namespace ModS

#endif
