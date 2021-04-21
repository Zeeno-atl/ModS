#pragma once
#ifndef _MODS_INJECTOR_HPP
#	define _MODS_INJECTOR_HPP

#	include <ModS/Module.hpp>
#	include <ModS/Typename.hpp>
#	include <Signal.hpp>
#	include <concepts>
#	include <filesystem>
#	include <stdexcept>
#	include <ranges>

namespace ModS {

class TypeMissing : public std::runtime_error {
public:
	using runtime_error::runtime_error;
};

class RecursiveDependency : public std::runtime_error {
public:
	using runtime_error::runtime_error;
};

class RecursiveRouting : public std::runtime_error {
public:
	using runtime_error::runtime_error;
};

class Injector : public AbstractInjector {
	class Private;
	std::shared_ptr<Private> p;

public:
	Injector();
	~Injector() override;

	bool addDynamicObject(std::filesystem::path);

	template<std::predicate<std::filesystem::path> P = std::function<bool(const std::filesystem::path&)>>
	void addDynamicObjectDirectory(std::filesystem::path path, P filter = &sharedObjectFilter) {
		for (auto f : std::filesystem::directory_iterator(path)) {
			if (filter(f)) {
				addDynamicObject(f);
			}
		}
	}

	void startService(std::filesystem::path filepath);
	void startService();
	void stopService(std::filesystem::path filepath);
	void stopService();

	void clearShareds();

	template<typename T>
	[[nodiscard]] std::shared_ptr<T> shared() {
		return std::static_pointer_cast<T>(shared(pretty_name<T>()));
	}

	template<typename T>
	[[nodiscard]] Pointer unique() {
		return unique(pretty_name<T>());
	}

	[[nodiscard]] std::shared_ptr<void> shared(const std::string_view typeName) override;
	[[nodiscard]] Pointer               unique(const std::string_view typeName) override;

	template<typename Interface, typename Implementation = Interface>
	void bind() {
		publishInterface<Interface>();
		publishImplementation<Implementation>();
		routeInterfaces<Implementation, Interface>();
	}

	template<typename... Interfaces>
	void publishInterfaces() {
		auto dummy = {publishInterface<Interfaces>()...};
	}

	template<typename... Implementations>
	void publishImplementations() {
		auto dummy = {publishImplementation<Implementations>()...};
	}

	template<typename Interface, typename Implementation>
	void routeToFactory(std::function<std::shared_ptr<Implementation>()> factory, std::int32_t priority = 1) {
		signalInterfaceRegistered(std::make_shared<InterfaceInfo<Interface>>());
		signalImplementationRegistered(std::make_shared<ImplementationFactory<Interface, Implementation>>(factory));
		signalRouteRegistered(std::make_shared<Route<Interface, ImplementationFactory<Interface, Implementation>>>(priority));
	}

	template<typename Implementation>
	void routeInterfaces() {
	}

	template<typename Implementation, typename Interface, typename... Interfaces>
	void routeInterfaces() {
		using R = Route<Interface, Implementation>;
		signalRouteRegistered(std::make_shared<R>());

		routeInterfaces<Implementation, Interfaces...>();
	}

	inline void routeInterfaces(const std::string& implementation, const std::vector<std::string>& interfaces) {
		for (const auto& iface : interfaces) {
			signalRouteRegistered(std::make_shared<Route<std::nullptr_t, std::nullptr_t>>(iface, implementation, 1));
		}
	}

	template<typename Interface>
	bool publishInterface() {
		std::shared_ptr<InterfaceInfo<Interface>> interface = std::make_shared<InterfaceInfo<Interface>>();
		signalInterfaceRegistered(interface);
		return true;
	}

	template<typename Implementation>
	bool publishImplementation() {
		std::shared_ptr<ImplementationInfo<Implementation>> implementation = std::make_shared<ImplementationInfo<Implementation>>();
		signalImplementationRegistered(implementation);
		return true;
	}

	static bool sharedObjectFilter(const std::filesystem::path& path);

	std::vector<std::string>                                        interfaces() const override;
	std::vector<std::string>                                        implementations() const override;
	std::vector<std::string>                                        implementationDependencies(const std::string_view implementation) const override;
	std::vector<std::tuple<std::string, std::string, std::int32_t>> routes() const override;
        
	Zeeno::Signal<std::shared_ptr<AbstractImplementationInfo>> signalImplementationRegistered;
	Zeeno::Signal<std::shared_ptr<AbstractInterfaceInfo>>      signalInterfaceRegistered;
	Zeeno::Signal<std::shared_ptr<AbstractRoute>>              signalRouteRegistered;

	std::shared_ptr<AbstractImplementationInfo> route(const std::string_view iface) const;

protected:
	void onImplementationRegistered(const std::shared_ptr<AbstractImplementationInfo>&);
	void onInterfaceRegistered(const std::shared_ptr<AbstractInterfaceInfo>&);
	void onRouteRegistered(const std::shared_ptr<AbstractRoute>&);
};

} // namespace ModS

#endif
