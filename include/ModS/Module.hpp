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
class Module : public AbstractModule {
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
	void routeInterfaces() {
	}

	template<typename Implementation, typename Interface, typename... Interfaces>
	void routeInterfaces() {
		using R = Route<Interface, Implementation>;
		signalRouteRegistered(std::make_shared<R>());

		routeInterfaces<Implementation, Interfaces...>();
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
