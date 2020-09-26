#include <ModS/Injector.hpp>
#include <boost/dll.hpp>
#include <boost/range/adaptors.hpp>
#include <functional>

namespace ModS {

class Injector::Private {
public:
	std::map<std::filesystem::path, boost::dll::shared_library>       modules;
	std::unordered_map<std::string, std::shared_ptr<AbstractFactory>> factories;
	std::unordered_map<std::string, std::shared_ptr<void>>            shareds;

	std::shared_ptr<AbstractModule> module(std::filesystem::path path) const {
		return modules.at(path).get<std::shared_ptr<AbstractModule>>("ModSModule");
	}
};

Injector::Injector() : p(std::make_shared<Private>()) {
	signalFactoryRegistered.connect([this](std::shared_ptr<AbstractFactory> fac) { onFactoryRegistered(fac); });
}

bool Injector::addDynamicObject(std::filesystem::path filepath) {
	filepath = std::filesystem::canonical(filepath);
	using namespace boost::dll;
	shared_library library(
	    filepath.string(), load_mode::load_with_altered_search_path | load_mode::rtld_lazy | load_mode::rtld_local);
	if (!library.has("ModSModule"))
		return false;
	if (p->modules.count(filepath))
		return false;

	p->modules[filepath] = library;
	auto module          = p->module(filepath);
	module->injector     = this;
	module->signalFactoryRegistered.connect([this](std::shared_ptr<AbstractFactory> fac) { onFactoryRegistered(fac); });
	module->bindTypes();
	return true;
}

void Injector::initialize(std::filesystem::path filepath) {
	filepath = std::filesystem::canonical(filepath);
	p->module(filepath)->initialize();
}

void Injector::initialize() {
	for (auto pair : p->modules)
		initialize(pair.first);
}

void Injector::finalize(std::filesystem::path filepath) {
	filepath = std::filesystem::canonical(filepath);
	p->module(filepath)->finalize();
}

void Injector::finalize() {
	for (auto pair : p->modules)
		finalize(pair.first);
	p->shareds.clear();
}

std::shared_ptr<void> Injector::shared(std::string name) {
	if (p->shareds.count(name))
		return p->shareds.at(name);

	auto uni         = unique(name);
	p->shareds[name] = uni.toSharedPtr<void>();
	return p->shareds[name];
}

Pointer Injector::unique(std::string name) {
	if (!p->factories.count(name))
		throw TypeMissing("Type '" + name + "' is not registered. Probably shared object was not loaded.");

	return p->factories.at(name)->create(this);
}

bool Injector::sharedObjectFilter(const std::filesystem::path &path) {
#ifdef __linux__
	return path.extension() == ".so";
#else
	return path.extension() == ".dll";
#endif
}

std::vector<std::string> Injector::boundTypes() const {
	auto types = p->factories | boost::adaptors::map_keys;
	return {types.begin(), types.end()};
}

std::vector<std::string> Injector::dependencies(const std::string &type) const {
	return p->factories.at(type)->dependencies();
}

void Injector::onFactoryRegistered(std::shared_ptr<AbstractFactory> fac) {
	p->factories[fac->interfaceName()] = fac;
}

} // namespace ModS
