#pragma once
#ifndef FOOMODULE_HPP
#	define FOOMODULE_HPP

#	include <ModS/Module.hpp>

class IFooClass;

class FooModule : public ModS::Module {
	void bindTypes() override;

	void startService() override;
	void stopService() override;

	// If you want to have something running all the time
	std::shared_ptr<IFooClass> service;
};

#endif // FOOMODULE_HPP
