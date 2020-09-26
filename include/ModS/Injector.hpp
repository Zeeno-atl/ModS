#pragma once
#ifndef _MODS_INJECTOR_HPP
#define _MODS_INJECTOR_HPP

#	include <ModS/Factory.hpp>
#	include <ModS/AbstractModule.hpp>
#	include <ModS/Typename.hpp>
#	include <Signal/Signal.hpp>
#	include <concepts>
#	include <filesystem>

namespace ModS {

class TypeMissing : public std::runtime_error {
public:
	using runtime_error::runtime_error;
};

class Injector : public AbstractInjector {
	class Private;
	std::shared_ptr<Private> p;

public:
	Injector();

	bool addDynamicObject(std::filesystem::path);

	template<std::predicate<std::filesystem::path> P = std::function<bool(const std::filesystem::path &)>>
	void addDynamicObjectDirectory(std::filesystem::path path, P filter = &sharedObjectFilter) {
		for (auto f : std::filesystem::directory_iterator(path))
			if (filter(f))
				addDynamicObject(f);
	}

	void initialize(std::filesystem::path filepath);
	void initialize();
	void finalize(std::filesystem::path filepath);
	void finalize();

	template<typename T>
	[[nodiscard]] std::shared_ptr<T> shared() {
		return std::static_pointer_cast<T>(shared(pretty_name<T>()));
	}

	template<typename T>
	[[nodiscard]] Pointer unique() {
		return unique(pretty_name<T>());
	}

	[[nodiscard]] std::shared_ptr<void> shared(std::string typeName) override;
	[[nodiscard]] Pointer               unique(std::string typeName) override;

	template<typename Interface, typename Implementation = Interface>
	void bind() {
		std::shared_ptr<Factory<Interface, Implementation>> fac = std::make_shared<Factory<Interface, Implementation>>();
		signalFactoryRegistered(fac);
	}

	static bool sharedObjectFilter(const std::filesystem::path &path);

protected:
	Zeeno::Signal<std::shared_ptr<AbstractFactory>> signalFactoryRegistered;

	void onFactoryRegistered(std::shared_ptr<AbstractFactory>);
};

} // namespace ModS

#endif
