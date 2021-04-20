#pragma once
#ifndef _MODS_IMPLEMENTATION_INFO_HPP
#	define _MODS_IMPLEMENTATION_INFO_HPP

#	include <ModS/AbstractImplementationInfo.hpp>
#	include <ModS/reflection.hxx>

namespace ModS {

template<typename Implementation>
class ImplementationInfo : public AbstractImplementationInfo {
public:
	Pointer create(AbstractInjector* inj) override {
		return construct(inj, refl::as_tuple<Implementation>());
	}

	std::string implementationName() const override {
		return pretty_name<Implementation>();
	}

	std::vector<std::string> dependencies() const override {
		return pretty_names(refl::as_tuple<Implementation>());
	}

private:
	template<typename X>
	struct Pull {
	};

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
	static Pointer construct(AbstractInjector* inj, std::tuple<Args...>) {
		Pointer p;
#	ifndef __llvm__
		p.value = new Implementation((Pull<Args>::pull(inj))...);
#	endif
		p.deleter = &destroy;
		return p;
	}

	static void destroy(void* ptr) {
		delete reinterpret_cast<Implementation*>(ptr);
	}
};

template<typename Interface>
class ImplementationFactory : public AbstractImplementationInfo {
	using Factory = std::function<std::shared_ptr<Interface>()>;
	Factory factory;
public:
	ImplementationFactory(Factory factory)
		: factory(factory) {
	}

	Pointer create(AbstractInjector* inj) override {
		Pointer ret;
		auto    sharedPtr = factory();
		ret.value         = sharedPtr.get();
		return ret;
	}

	std::string implementationName() const override {
		return pretty_name<ImplementationFactory<Interface>>();
	}

	std::vector<std::string> dependencies() const override {
		return {};
	}
};

} // namespace ModS

#endif // FACTORY_HPP
