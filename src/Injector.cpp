#include <ModS/Injector.hpp>
#include <boost/dll.hpp>
#include <deque>
#include <functional>
#include <ranges>

namespace ModS {

class Injector::Private {
public:
	std::map<std::filesystem::path, boost::dll::shared_library>                  modules;
	std::unordered_map<std::string, std::shared_ptr<AbstractImplementationInfo>> implementations;
	std::unordered_map<std::string, std::shared_ptr<AbstractInterfaceInfo>>      interfaces;
	std::unordered_map<std::string, std::shared_ptr<AbstractRoute>>              routes;
	std::unordered_map<std::string, std::shared_ptr<void>>                       shareds;
	std::deque<std::string_view>                                                 creating;

	static std::string mangledFactoryName(const std::filesystem::path& path) {
		std::vector<std::string> symbols                = boost::dll::library_info(path.c_str()).symbols();
		const auto               getModuleFactorySymbol = std::find_if(symbols.cbegin(), symbols.cend(), [](const std::string& symbol) {
            return symbol.find(MODS_INSTANCE_NAME_STRING) != std::string::npos;
        });
		return getModuleFactorySymbol != symbols.end() ? std::string(*getModuleFactorySymbol) : "";
	}

	std::shared_ptr<AbstractModule> module(std::filesystem::path path) const {
		return modules.at(path).get<std::shared_ptr<AbstractModule>>(mangledFactoryName(path));
	}
};

Injector::Injector() : p(std::make_shared<Private>()) {
	signalImplementationRegistered.connect([this](std::shared_ptr<AbstractImplementationInfo> impl) { onImplementationRegistered(impl); });
	signalInterfaceRegistered.connect([this](std::shared_ptr<AbstractInterfaceInfo> iface) { onInterfaceRegistered(iface); });
	signalRouteRegistered.connect([this](std::shared_ptr<AbstractRoute> route) { onRouteRegistered(route); });
}

bool Injector::addDynamicObject(std::filesystem::path filepath) {
	filepath = std::filesystem::canonical(filepath);
	using namespace boost::dll;
	shared_library library;
	library.load(filepath.string(), load_mode::load_with_altered_search_path | load_mode::rtld_lazy | load_mode::rtld_local);

	if (!library.is_loaded())
		return false;
	if (!library.has(Private::mangledFactoryName(filepath)))
		return false;
	if (p->modules.contains(filepath))
		return false;

	p->modules[filepath] = std::move(library);
	auto module          = p->module(filepath);
	module->injector     = this;
	module->signalImplementationRegistered.connect([this](std::shared_ptr<AbstractImplementationInfo> impl) { signalImplementationRegistered(impl); });
	module->signalInterfaceRegistered.connect([this](std::shared_ptr<AbstractInterfaceInfo> iface) { signalInterfaceRegistered(iface); });
	module->signalRouteRegistered.connect([this](std::shared_ptr<AbstractRoute> route) { signalRouteRegistered(route); });
	module->bindTypes();
	return true;
}

void Injector::startService(std::filesystem::path filepath) {
	filepath = std::filesystem::canonical(filepath);
	p->module(filepath)->startService();
}

void Injector::startService() {
	for (auto pair : p->modules) {
		startService(pair.first);
	}
}

void Injector::stopService(std::filesystem::path filepath) {
	filepath = std::filesystem::canonical(filepath);
	p->module(filepath)->stopService();
}

void Injector::stopService() {
	for (auto pair : p->modules) {
		stopService(pair.first);
	}
}

std::shared_ptr<void> Injector::shared(const std::string_view name) {
	std::string sname{name};
	if (p->shareds.contains(sname)) {
		return p->shareds.at(sname);
	}

	auto uni          = unique(name);
	p->shareds[sname] = uni.toSharedPtr<void>();
	return p->shareds[sname];
}

Pointer Injector::unique(const std::string_view name) {
	std::shared_ptr<AbstractImplementationInfo> impl = route(name);

	p->creating.push_back(name);
	Pointer ptr = impl->create(this);
	p->creating.pop_back();
	return ptr;
}

bool Injector::sharedObjectFilter(const std::filesystem::path& path) {
#ifdef __linux__
	return path.extension() == ".so";
#else
	return path.extension() == ".dll";
#endif
}

std::vector<std::string> Injector::interfaces() const {
	auto types = p->interfaces | std::views::keys | std::views::common;
	return {types.begin(), types.end()};
}

std::vector<std::string> Injector::implementations() const {
	auto types = p->implementations | std::views::keys | std::views::common;
	return {types.begin(), types.end()};
}

std::vector<std::pair<std::string, std::string>> Injector::routes() const {
	auto rts = p->routes | std::views::values | std::views::transform([](std::shared_ptr<AbstractRoute> route) -> std::pair<std::string, std::string> {
		           return std::make_pair(route->interfaceName(), route->implementationName());
	           })
	           | std::views::common;
	return {rts.begin(), rts.end()};
}

std::vector<std::string> Injector::implementationDependencies(const std::string_view type) const {
	return p->implementations.at(std::string(type))->dependencies();
}

void Injector::onImplementationRegistered(std::shared_ptr<AbstractImplementationInfo> impl) {
	p->implementations[impl->implementationName()] = impl;
}

void Injector::onInterfaceRegistered(std::shared_ptr<AbstractInterfaceInfo> iface) {
	p->interfaces[iface->interfaceName()] = iface;
}

void Injector::onRouteRegistered(std::shared_ptr<AbstractRoute> route) {
	p->routes[route->interfaceName()] = route;
}

std::shared_ptr<AbstractImplementationInfo> Injector::route(const std::string_view iface) const {
	std::string             resolved{iface};
	std::deque<std::string> path;
	path.emplace_back(resolved);

	while (p->routes.contains(resolved)) {
		resolved = p->routes.at(resolved)->implementationName();

		if (std::find(path.cbegin(), path.cend(), resolved) != path.cend()) {
			throw RecursiveRouting("There is a recursive routing for interface '" + std::string(iface) + "'.");
		}

		path.emplace_back(resolved);
	}

	if (!p->implementations.contains(resolved)) {
		throw TypeMissing("Route ended at unregistered type '" + resolved + "'.");
	}

	return p->implementations.at(resolved);
}

} // namespace ModS
