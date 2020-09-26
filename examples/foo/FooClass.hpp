#ifndef FOOCLASS_HPP
#define FOOCLASS_HPP

#include <memory>

#include "IFooClass.hpp"

class KernelService;

class FooClass : public IFooClass {
public:
	FooClass(std::shared_ptr<KernelService> service);
	~FooClass();

	int answer() override;

	std::shared_ptr<KernelService> service;
};

#endif // FOOCLASS_HPP
