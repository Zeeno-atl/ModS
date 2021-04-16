#pragma once
#ifndef _MODS_GRAPH_VIZ_HPP
#	define _MODS_GRAPH_VIZ_HPP

#	include <ModS/AbstractInjector.hpp>
#	include <ostream>

namespace ModS {
inline std::ostream& operator<<(std::ostream& os, AbstractInjector& injector) {
	os << "digraph ModS {" << std::endl;

	for (const auto& iface : injector.interfaces()) {
		os << "\t\"<<interface>> " << iface << "\";" << std::endl;
	}

    for (const auto& impl : injector.implementations()) {
		os << "\t\"" << impl << "\";" << std::endl;
    }
	
	for (const auto& impl : injector.implementations()) {
		std::string deps;
		for (const auto& dep : injector.implementationDependencies(impl)) {
			deps += "\"" + dep + "\", ";
		}
		os << "\t\"" << impl << "\"";
		if (!deps.empty()) {
			os << " -> " << deps.substr(0, deps.size() - 2) << " [arrowhead = vee, style = dashed]";
		}
		os << ";" << std::endl;
	}

	for (const auto& [iface, impl, priority] : injector.routes()) {
		os << "\t\"" << impl << "\" -> \"<<interface>> " << iface << "\" [arrowhead = onormal, style = dashed, label = \"p = " << priority << "\"];" << std::endl;
	}

	os << "}";
	return os;
}
} // namespace ModS

#endif