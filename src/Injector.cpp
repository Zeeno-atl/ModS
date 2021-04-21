#include <ModS/Injector.hpp>
#include <boost/dll.hpp>
#include <deque>
#include <functional>
#include <ranges>
#include <utility>

namespace ModS {

struct RouteCompare {
	bool operator()(const std::shared_ptr<AbstractRoute>& lhs, const std::shared_ptr<AbstractRoute>& rhs) const {
		return lhs->priority() > rhs->priority();
	}
};

class Injector::Private {
public:
	std::map<std::int32_t, std::vector<std::shared_ptr<AbstractRoute>>, std::greater<>> routes;
	std::unordered_map<std::string, std::shared_ptr<void>>                              shareds;
	std::unordered_map<std::string, std::shared_ptr<AbstractInterfaceInfo>>             interfaces;
	std::unordered_map<std::string, std::shared_ptr<AbstractImplementationInfo>>        implementations;
	std::deque<std::string_view>                                                        creating;
	std::vector<std::shared_ptr<Zeeno::Connection>>                                     connections;
	std::map<std::filesystem::path, boost::dll::shared_library>                         modules;

	static std::string mangledFactoryName(const std::filesystem::path& path) {
		std::vector<std::string> symbols                = boost::dll::library_info(path.c_str()).symbols();
		const auto               getModuleFactorySymbol = std::find_if(
			symbols.cbegin(),
			symbols.cend(),
			[](const std::string& symbol) {
				return symbol.find(MODS_INSTANCE_NAME_STRING) != std::string::npos;
			});
		return getModuleFactorySymbol != symbols.end() ? std::string(*getModuleFactorySymbol) : "";
	}

	std::shared_ptr<AbstractModule> module(const std::filesystem::path& path) const {
		return modules.at(path).get<std::shared_ptr<AbstractModule>>(mangledFactoryName(path));
	}
};

Injector::Injector()
	: p(std::make_shared<Private>()) {
	signalImplementationRegistered.connect(
		[this](std::shared_ptr<AbstractImplementationInfo> impl) {
			onImplementationRegistered(std::move(impl));
		});
	signalInterfaceRegistered.connect(
		[this](std::shared_ptr<AbstractInterfaceInfo> iface) {
			onInterfaceRegistered(std::move(iface));
		});
	signalRouteRegistered.connect(
		[this](std::shared_ptr<AbstractRoute> route) {
			onRouteRegistered(std::move(route));
		});
}

Injector::~Injector() {
	//important to reset connections before Private is destroyed
	signalImplementationRegistered.disconnectAll();
	signalInterfaceRegistered.disconnectAll();
	signalRouteRegistered.disconnectAll();

	for (auto c : p->connections) {
		c->disconnect();
	}
	p->connections.clear();

	clearShareds();
	p->routes.clear();
	p->interfaces.clear();
	p->implementations.clear();
	
	p.reset();
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
	p->connections.push_back(
		module->signalImplementationRegistered.connect(
			[this](std::shared_ptr<AbstractImplementationInfo> impl) {
				signalImplementationRegistered(std::move(impl));
			}));
	p->connections.push_back(
		module->signalInterfaceRegistered.connect(
			[this](std::shared_ptr<AbstractInterfaceInfo> iface) {
				signalInterfaceRegistered(std::move(iface));
			}));
	p->connections.push_back(
		module->signalRouteRegistered.connect(
			[this](std::shared_ptr<AbstractRoute> route) {
				signalRouteRegistered(std::move(route));
			}));
	module->bindTypes();
	return true;
}

void Injector::startService(std::filesystem::path filepath) {
	filepath = std::filesystem::canonical(filepath);
	p->module(filepath)->startService();
}

void Injector::startService() {
	for (const auto pair : p->modules) {
		startService(pair.first);
	}
}

void Injector::stopService(std::filesystem::path filepath) {
	filepath = std::filesystem::canonical(filepath);
	p->module(filepath)->stopService();
}

void Injector::stopService() {
	for (const auto pair : p->modules) {
		stopService(pair.first);
	}
}

void Injector::clearShareds() {
	p->shareds.clear();
}

std::shared_ptr<void> Injector::shared(const std::string_view name) {
	const std::string sname{name};
	if (p->shareds.contains(sname)) {
		return p->shareds.at(sname);
	}

	std::shared_ptr<AbstractImplementationInfo> impl = route(name);
	if (impl->isFactoryType()) {
		return impl->create();
	}

	const auto uni    = unique(name);
	p->shareds[sname] = uni.toSharedPtr<void>();
	return p->shareds[sname];
}

Pointer Injector::unique(const std::string_view name) {
	std::shared_ptr<AbstractImplementationInfo> impl = route(name);

	p->creating.push_back(name);
	const Pointer ptr = impl->create(this);
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
	const auto types = p->interfaces | std::views::keys | std::views::common;
	return {types.begin(), types.end()};
}

std::vector<std::string> Injector::implementations() const {
	const auto types = p->implementations | std::views::keys | std::views::common;
	return {types.begin(), types.end()};
}

std::vector<std::tuple<std::string, std::string, std::int32_t>> Injector::routes() const {
	/*auto rts = p->routes | std::views::values | std::views::join | std::views::transform([](std::shared_ptr<AbstractRoute> route) {
		           return std::make_tuple(route->interfaceName(), route->implementationName(), route->priority());
	           })
	           | std::views::common;
	return {rts.begin(), rts.end()};*/
	std::vector<std::tuple<std::string, std::string, std::int32_t>> rts;

	for (const auto& table : p->routes | std::views::values) {
		for (const auto& r : table) {
			rts.push_back(std::make_tuple(r->interfaceName(), r->implementationName(), r->priority()));
		}
	}

	return rts;
}

std::vector<std::string> Injector::implementationDependencies(const std::string_view type) const {
	return p->implementations.at(std::string(type))->dependencies();
}

void Injector::onImplementationRegistered(const std::shared_ptr<AbstractImplementationInfo>& impl) {
	p->implementations[impl->implementationName()] = impl;
}

void Injector::onInterfaceRegistered(const std::shared_ptr<AbstractInterfaceInfo>& iface) {
	p->interfaces[iface->interfaceName()] = iface;
}

void Injector::onRouteRegistered(const std::shared_ptr<AbstractRoute>& route) {
	p->routes[route->priority()].emplace_back(route);
}

std::shared_ptr<AbstractImplementationInfo> Injector::route(const std::string_view iface) const {
	std::string             resolved{iface};
	std::deque<std::string> path;
	path.emplace_back(resolved);

	bool found = true;
	while (found) {
		found = false;
		for (const auto& table : p->routes | std::views::values) {
			if (auto it = std::ranges::find_if(
				table,
				[&resolved](const std::shared_ptr<AbstractRoute>& r) {
					return r->interfaceName() == resolved;
				}); it != table.end()) {
				resolved = (*it)->implementationName();
				if (std::ranges::find(path, resolved) != path.end()) {
					throw RecursiveRouting("There is a recursive routing for interface '" + std::string(iface) + "'.");
				}
				path.emplace_back(resolved);
				found = true;
				break;
			}
		}

		if (!found && !p->implementations.contains(resolved)) {
			throw TypeMissing("Route ended at unregistered type '" + resolved + "'.");
		}
	}
	return p->implementations.at(resolved);
}

} // namespace ModS
