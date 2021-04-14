#pragma once
#ifndef _MODS_ABSTRACTINJECTOR_H
#	define _MODS_ABSTRACTINJECTOR_H

#	include <ModS/Pointer.hpp>
#	include <memory>
#	include <string>

namespace ModS {
class AbstractInjector {
public:
	[[nodiscard]] virtual std::shared_ptr<void> shared(const std::string_view typeName) = 0;
	[[nodiscard]] virtual Pointer               unique(const std::string_view typeName) = 0;
};

} // namespace ModS

#endif
