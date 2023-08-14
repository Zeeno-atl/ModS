#pragma once
#ifndef _MODS_INTERFACE_INFO_HPP
#	define _MODS_INTERFACE_INFO_HPP

#	include <ModS/AbstractInterfaceInfo.hpp>
#	include <ModS/Typename.hpp>

namespace ModS {

template<typename Interface>
class InterfaceInfo : public AbstractInterfaceInfo {
public:
	std::string interfaceName() const override {
		return pretty_name<Interface>();
	}
};
} // namespace ModS

#endif
