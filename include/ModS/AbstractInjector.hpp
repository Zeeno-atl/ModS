#pragma once
#ifndef _MODS_ABSTRACTINJECTOR_H
#	define _MODS_ABSTRACTINJECTOR_H

#	include <ModS/Pointer.hpp>
#	include <memory>
#	include <string>
#	include <vector>

namespace ModS {
class AbstractInjector {
public:
	[[nodiscard]] virtual std::shared_ptr<void> shared(const std::string_view typeName) = 0;
	[[nodiscard]] virtual Pointer               unique(const std::string_view typeName) = 0;

	virtual std::vector<std::string>                         interfaces() const                                                      = 0;
	virtual std::vector<std::string>                         implementations() const                                                 = 0;
	virtual std::vector<std::string>                         implementationDependencies(const std::string_view implementation) const = 0;
	virtual std::vector<std::pair<std::string, std::string>> routes() const                                                          = 0;
};

} // namespace ModS

#endif
