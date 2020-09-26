#include <ModS/Injector.hpp>
#include <iostream>
#include <thread>

#include "../foo/IFooClass.hpp"
#include "KernelService.hpp"

using namespace std;

class KernelServiceImpl : public KernelService {
public:
	int answer() override { return 42; }
};

int main() {
	ModS::Injector injector;

	injector.bind<KernelService, KernelServiceImpl>();

	try {
		injector.addDynamicObjectDirectory(std::filesystem::path("../foo"), &ModS::Injector::sharedObjectFilter);
		//injector.addDynamicObjectDirectory(std::filesystem::path("../bar_module"));
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	injector.initialize();

	auto x = injector.shared<IFooClass>();

	std::cout << "Ultimate answer is: " << x->answer() << std::endl;

	//do stuff
	std::this_thread::sleep_for(std::chrono::seconds(3));

	injector.finalize();

	return 0;
}
