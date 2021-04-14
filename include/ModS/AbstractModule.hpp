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
#	include <iostream>

namespace ModS {
class AbstractModule {
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
	std::shared_ptr<T> shared() {
		std::clog << "Getting shared: " << pretty_name<T>() << std::endl;
		return std::static_pointer_cast<T>(injector->shared(pretty_name<T>()));
	}

	template<typename T>
	Pointer unique() {
		return injector->unique(pretty_name<T>());
	}

private:
	AbstractInjector* injector{};
};
} // namespace ModS

#endif
