#pragma once
#ifndef _MODS_ABSTRACT_IMPLEMENTATION_INFO_HPP
#	define _MODS_ABSTRACT_IMPLEMENTATION_INFO_HPP

#	include <ModS/AbstractInjector.hpp>
#	include <string>
#	include <vector>

namespace ModS {
class Injector;

class AbstractImplementationInfo {
public:
	virtual ~AbstractImplementationInfo() = default;

	virtual std::shared_ptr<void>    create(AbstractInjector* inj) = 0;
	virtual std::string              implementationName() const    = 0;
	virtual std::vector<std::string> dependencies() const          = 0;
	virtual bool                     isFactoryType() const         = 0;
};
} // namespace ModS

#endif
