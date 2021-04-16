#pragma once
#ifndef _MODS_GRAPH_VIZ_HPP
#	define _MODS_GRAPH_VIZ_HPP

#	include <ModS/AbstractInjector.hpp>
#	include <ostream>

namespace ModS {
inline std::ostream& operator<<(std::ostream& os, AbstractInjector& injector) {
	os << "digraph ModS {" << std::endl;

	for (const auto& iface : injector.interfaces()) {
		os << "\t\"" << iface << "\";" << std::endl;
	}

	for (const auto& impl : injector.implementations()) {
		std::string deps;
		for (const auto& dep : injector.implementationDependencies(impl)) {
			deps += "\"" + dep + "\", ";
		}
		if (!deps.empty()) {
			os << "\t\"" << impl << "\" -> ";
			os << deps.substr(0, deps.size() - 2);
			os << ";" << std::endl;
		}
	}

	for (const auto& [iface, impl] : injector.routes()) {
		os << "\t\"" << impl << "\" -> \"" << iface << "\" [arrowhead = open, style = dashed];" << std::endl;
	}

	os << "}";
	return os;
}
} // namespace ModS

#endif