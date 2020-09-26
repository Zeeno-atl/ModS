#pragma once
#ifndef _MODS_ABSTRACTFACTORY_HPP
#	define _MODS_ABSTRACTFACTORY_HPP

#	include <ModS/AbstractInjector.hpp>
#	include <ModS/Pointer.hpp>
#	include <ModS/Typename.hpp>
#	include <memory>
#	include <string>
#	include <vector>

namespace ModS {
class Injector;

class AbstractFactory {
public:
	virtual ~AbstractFactory() = default;

	virtual Pointer                  create(AbstractInjector *inj) = 0;
	virtual std::string              interfaceName() const         = 0;
	virtual std::string              implementationName() const    = 0;
	virtual std::vector<std::string> dependencies() const          = 0;
};
} // namespace ModS

#endif
