#pragma once
#ifndef _MODS_ABSTRACT_INTERFACE_INFO_HPP
#	define _MODS_ABSTRACT_INTERFACE_INFO_HPP

#	include <string>

namespace ModS {
class AbstractInterfaceInfo {
public:
	virtual ~AbstractInterfaceInfo()          = default;
	virtual std::string interfaceName() const = 0;
};
} // namespace ModS

#endif
