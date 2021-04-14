#include "FooClass.hpp"

#include "../main/KernelService.hpp"
#include <iostream>

FooClass::FooClass(std::shared_ptr<KernelService> service) : service(service) {
	std::cout << "FooClass() constructor" << std::endl;
}

FooClass::~FooClass() {
	std::cout << "FooClass() destructor" << std::endl;
}

int FooClass::answer() {
	return service->answer() / 2;
}
