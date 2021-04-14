#include "FooModule.hpp"
#include "FooClass.hpp"

#include "IFooClass.hpp"

MODS_MODULE(FooModule)

void FooModule::bindTypes() {
	//Register FooClass to be provided appwide

	publishInterfaces<IFooClass>();
	publishImplementations<FooClass>();
	routeInterfaces<FooClass, IFooClass>();
	// or simply
	// bind<IFooClass, FooClass>();
}

void FooModule::startService() {
	//make FooClass to exist until finalize is called
	service = shared<IFooClass>();
}

void FooModule::stopService() {
	service.reset();
}
