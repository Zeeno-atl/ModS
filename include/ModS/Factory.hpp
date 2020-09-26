#pragma once
#ifndef _MODS_FACTORY_HPP
#define _MODS_FACTORY_HPP

#	include <ModS/AbstractFactory.hpp>
#	include <ModS/reflection.hxx>

namespace ModS {

template<typename Interface, typename Implementation = Interface>
class Factory : public AbstractFactory {
public:
	Pointer create(AbstractInjector *inj) override { return construct(inj, refl::as_tuple<Implementation>()); }

	std::string              implementationName() const override { return pretty_name<Implementation>(); }
	std::string              interfaceName() const override { return pretty_name<Interface>(); }
	std::vector<std::string> dependencies() const override { return dependencyNames(refl::as_tuple<Implementation>()); }

private:
	template<typename X>
	struct Pull {};

	template<typename X>
	struct Pull<std::shared_ptr<X>> {
		static std::shared_ptr<X> pull(AbstractInjector *inj) {
			return std::static_pointer_cast<X>(inj->shared(pretty_name<X>()));
		}
	};

	template<typename X>
	struct Pull<std::unique_ptr<X>> {
		static std::unique_ptr<X> pull(AbstractInjector *inj) {
			return std::unique_ptr<X>(inj->unique(pretty_name<X>()));
		}
	};

	template<typename... Args>
	static Pointer construct(AbstractInjector *inj, std::tuple<Args...>) {
		Pointer p;
		p.value   = new Implementation((Pull<Args>::pull(inj))...);
		p.deleter = &destroy;
		return p;
	}

	static void destroy(void *ptr) { delete reinterpret_cast<Implementation *>(ptr); }

	template<typename... Args>
	std::vector<std::string> dependencyNames(std::tuple<>) const {
		return {};
	}

	template<typename... Args>
	std::vector<std::string> dependencyNames(std::tuple<Args...>) const {
		return {(pretty_name<Args>(), ...)};
	}
};

} // namespace ModS

#endif // FACTORY_HPP
