#pragma once
#ifndef _MODS_ABSTRACTMODULE_H
#	define _MODS_ABSTRACTMODULE_H

#	include <ModS/AbstractImplementationInfo.hpp>
#	include <ModS/AbstractInterfaceInfo.hpp>
#	include <ModS/AbstractRoute.hpp>
#	include <ModS/AbstractInjector.hpp>
#	include <ModS/Typename.hpp>
#	include <Signal/Signal.hpp>
#	include <memory>

#	ifndef MODS_EXPORT
#		ifdef _MSC_VER
#			define MODS_EXPORT __declspec(dllexport)
#		else
#			define MODS_EXPORT __attribute__((visibility("default")))
#		endif
#	endif

namespace ModS {
class MODS_EXPORT AbstractModule {
	friend class Injector;

protected:
	virtual void bindTypes() = 0;
	virtual void startService() {
	}
	virtual void stopService() {
	}

	Zeeno::Signal<std::shared_ptr<AbstractImplementationInfo>> signalImplementationRegistered;
	Zeeno::Signal<std::shared_ptr<AbstractInterfaceInfo>>      signalInterfaceRegistered;
	Zeeno::Signal<std::shared_ptr<AbstractRoute>>              signalRouteRegistered;

	template<typename T>
	[[nodiscard]] std::shared_ptr<T> shared() {
		return std::static_pointer_cast<T>(injector->shared(pretty_name<T>()));
	}

	template<typename T>
	[[nodiscard]] std::shared_ptr<T> shared(const std::string_view implementation) {
		return std::static_pointer_cast<T>(injector->shared(pretty_name<T>(), implementation));
	}

private:
	AbstractInjector* injector{};
};
} // namespace ModS

#endif
