#pragma once
#ifndef _MODS_MODULE_HPP
#	define _MODS_MODULE_HPP

#	include <memory>
#	include <ModS/AbstractModule.hpp>
#	include <ModS/InterfaceInfo.hpp>
#	include <ModS/ImplementationInfo.hpp>
#	include <ModS/Route.hpp>

#	ifndef MODS_INSTANCE_NAME
#		define MODS_INSTANCE_NAME makeModSModule
#		define MODS_INSTANCE_NAME_STRING "makeModSModule"
#	endif

#	ifndef MODS_EXPORT
#		ifdef _MSC_VER
#			define MODS_EXPORT __declspec(dllexport)
#		else
#			define MODS_EXPORT __attribute__((visibility("default")))
#		endif
#	endif

#	define MODS_MODULE(TYPE) MODS_EXPORT std::shared_ptr<ModS::AbstractModule> MODS_INSTANCE_NAME = std::make_shared<TYPE>();

namespace ModS {
class MODS_EXPORT Module : public AbstractModule {
protected:
	template<typename Interface, typename Implementation = Interface>
	void bind() {
		publishInterface<Interface>();
		publishImplementation<Implementation>();
		routeInterfaces<Implementation, Interface>();
	}

	template<typename... Interfaces>
	void publishInterfaces() {
		auto dummy = {publishInterface<Interfaces>()...};
	}

	template<typename... Implementations>
	void publishImplementations() {
		auto dummy = {publishImplementation<Implementations>()...};
	}

	template<typename Implementation>
	void routeInterfaces(int priority = 0) {
	}

	template<typename Implementation, typename Interface, typename... Interfaces>
	void routeInterfaces(int priority = 0) {
		using R = Route<Interface, Implementation>;
		signalRouteRegistered(std::make_shared<R>(priority));

		routeInterfaces<Implementation, Interfaces...>(priority);
	}

	template<typename Interface, typename Implementation>
	std::enable_if_t<std::is_base_of_v<Interface, Implementation>> routeToFactory(
		std::function<std::shared_ptr<Implementation>()> factory,
		std::int32_t                                     priority = 1) {
		signalInterfaceRegistered(std::make_shared<InterfaceInfo<Interface>>());
		signalImplementationRegistered(std::make_shared<ImplementationFactory<Implementation>>(factory));
		signalRouteRegistered(std::make_shared<FactoryRoute<Interface, Implementation>>(priority));
	}

private:
	template<typename Interface>
	bool publishInterface() {
		std::shared_ptr<InterfaceInfo<Interface>> interface = std::make_shared<InterfaceInfo<Interface>>();
		signalInterfaceRegistered(interface);
		return true;
	}

	template<typename Implementation>
	bool publishImplementation() {
		std::shared_ptr<ImplementationInfo<Implementation>> implementation = std::make_shared<ImplementationInfo<Implementation>>();
		signalImplementationRegistered(implementation);
		return true;
	}
};
} // namespace ModS

#endif
