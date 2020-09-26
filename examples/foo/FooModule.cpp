#include "FooModule.hpp"
#include "FooClass.hpp"

__attribute__((visibility("default"))) std::shared_ptr<FooModule> ModSModule = std::make_shared<FooModule>();

void FooModule::bindTypes() {
	//Register FooClass to be provided appwide
	bind<IFooClass, FooClass>();
}

void FooModule::initialize() {
	//make FooClass to exist until finalize is called
	service = shared<IFooClass>();
}

void FooModule::finalize() {
	service.reset();
}
