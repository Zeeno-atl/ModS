#pragma once
#ifndef FOOMODULE_HPP
#	define FOOMODULE_HPP

#	include <ModS/Module.hpp>

class IFooClass;

class FooModule : public ModS::Module {
	void bindTypes() override;
	void initialize() override;
	void finalize() override;

	std::shared_ptr<IFooClass> service;
};

#endif // FOOMODULE_HPP
