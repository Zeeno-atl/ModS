#pragma once
#ifndef _MODS_MODULE_HPP
#	define _MODS_MODULE_HPP

#	include <ModS/AbstractModule.hpp>
#	include <memory>
#	include <ModS/Factory.hpp>

namespace ModS {
class Module : public AbstractModule {
protected:
	template<typename Interface, typename Implementation = Interface>
	void bind() {
		std::shared_ptr<Factory<Interface, Implementation>> fac = std::make_shared<Factory<Interface, Implementation>>();
		signalFactoryRegistered(fac);
	}
};
} // namespace ModS

#endif
