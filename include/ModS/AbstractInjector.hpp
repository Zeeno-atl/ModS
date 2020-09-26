#pragma once
#ifndef _MODS_ABSTRACTINJECTOR_H
#	define _MODS_ABSTRACTINJECTOR_H

#	include <ModS/Pointer.hpp>
#	include <memory>
#	include <string>

namespace ModS {
class AbstractInjector {
public:
	virtual std::shared_ptr<void> shared(std::string) = 0;
	virtual Pointer               unique(std::string) = 0;
};

} // namespace ModS

#endif
