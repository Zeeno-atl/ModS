#pragma once
#ifndef _MODS_IMPLEMENTATION_INFO_HPP
#	define _MODS_IMPLEMENTATION_INFO_HPP

#	include <ModS/AbstractImplementationInfo.hpp>
#	include <ModS/reflection.hxx>

namespace ModS {

template<typename Implementation>
class ImplementationInfo : public AbstractImplementationInfo {
public:
	std::shared_ptr<void> create(AbstractInjector* inj) override {
		return construct(inj, refl::as_tuple<Implementation>());
	}

	std::string implementationName() const override {
		return pretty_name<Implementation>();
	}

	std::vector<std::string> dependencies() const override {
		return pretty_names(refl::as_tuple<Implementation>());
	}

	bool isFactoryType() const override {
		return false;
	}

private:
	template<typename X>
	struct Pull {};

	template<typename X>
	struct Pull<std::shared_ptr<X>> {
		static std::shared_ptr<X> pull(AbstractInjector* inj) {
			return std::static_pointer_cast<X>(inj->shared(pretty_name<X>()));
		}
	};

	template<typename X>
	struct Pull<std::unique_ptr<X>> {
		static std::unique_ptr<X> pull(AbstractInjector* inj) {
			return std::unique_ptr<X>(inj->unique(pretty_name<X>()));
		}
	};

	template<typename... Args>
	static std::shared_ptr<void> construct(AbstractInjector* inj, std::tuple<Args...>) {
		std::shared_ptr<void> ptr;
#	ifndef __llvm__
		ptr = std::make_shared<Implementation>((Pull<Args>::pull(inj))...);
#	endif
		return ptr;
	}
};

template<typename Implementation>
class ImplementationFactory : public AbstractImplementationInfo {
	using Factory = std::function<std::shared_ptr<Implementation>()>;
	Factory factory;

public:
	using value_type = Implementation;

	ImplementationFactory(Factory factory) : factory(factory) {
	}

	std::shared_ptr<void> create(AbstractInjector* inj) override {
		return factory();
	}

	std::string implementationName() const override {
		return pretty_name<ImplementationFactory<Implementation>>();
	}

	std::vector<std::string> dependencies() const override {
		return {};
	}

	bool isFactoryType() const override {
		return true;
	}
};

} // namespace ModS

#endif // FACTORY_HPP
