#include <ModS/Injector.hpp>
#include <iostream>
#include <thread>

#include "../foo/IFooClass.hpp"
#include "KernelService.hpp"

using namespace std;

class KernelServiceImpl : public KernelService {
public:
	int answer() override {
		return 42;
	}
};

int main() {
	ModS::Injector injector;

	injector.bind<KernelService, KernelServiceImpl>();

	try {
		injector.addDynamicObjectDirectory(std::filesystem::path("../foo/"), &ModS::Injector::sharedObjectFilter);
		//injector.addDynamicObjectDirectory(std::filesystem::path("../bar_module"));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	injector.startService();

	auto x = injector.shared<IFooClass>();

	std::cout << "Ultimate answer is: " << x->answer() << std::endl;

	std::cout << std::endl;
	std::cout << "Here are all the interfaces" << std::endl;
	for (auto type : injector.interfaces()) {
		std::cout << ", " << type;
	}
	std::cout << std::endl;

	std::cout << "Here are all the implementations" << std::endl;
	for (auto type : injector.implementations()) {
		std::cout << ", " << type;
	}
	std::cout << std::endl;

	std::cout << "Here are all the dependencies" << std::endl;
	for (auto type : injector.implementations()) {
		std::cout << "\t" << type << ":";
		for (auto dep : injector.implementationDependencies(type))
			std::cout << " " << dep;
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Here are all the routes" << std::endl;
	for (auto [iface, impl] : injector.routes()) {
		std::cout << "\t" << iface << " -> " << impl << std::endl;
	}
	std::cout << std::endl;

	//do stuff
	std::this_thread::sleep_for(std::chrono::seconds(3));

	injector.stopService();

	return 0;
}
