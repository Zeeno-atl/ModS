#pragma once
#ifndef _MODS_GRAPH_VIZ_HPP
#	define _MODS_GRAPH_VIZ_HPP

#	include <ModS/AbstractInjector.hpp>
#	include <ModS/Typename.hpp>
#	include <ostream>
#	include <ranges>
#	include <regex>
#	include <string_view>

namespace ModS {
struct _Anchor;

inline std::string interfaceLabel(const std::string_view iface) {
	return std::string("<<interface>>\\n") + std::string(iface);
}

inline std::string extractTypeFromSharedPtr(const std::string_view type) {
	const std::string anchorString    = pretty_name<_Anchor>();
	const std::string anchorPtrString = pretty_name<std::shared_ptr<_Anchor>>();

	const std::size_t prefixLen = anchorPtrString.find(anchorString);
	if (prefixLen == std::string::npos) {
		return std::string(type);
	}
	const size_t suffixLen = anchorPtrString.length() - prefixLen - anchorString.length();
	const size_t typeLen   = type.length() - prefixLen - suffixLen;
	return std::string(type.substr(prefixLen, typeLen));
}

inline std::ostream& operator<<(std::ostream& os, AbstractInjector& injector) {
	const auto interfaces = injector.interfaces();

	os << "digraph ModS {" << std::endl;

	for (const auto& iface : interfaces) {
		os << "\t\"" << interfaceLabel(iface) << "\" [shape = pentagon];" << std::endl;
	}

	for (const auto& impl : injector.implementations()) {
		os << "\t\"" << impl << "\";" << std::endl;
	}

	for (const auto& impl : injector.implementations()) {
		std::string deps;
		for (const auto& dep : injector.implementationDependencies(impl)) {
			deps += "\"" + dep + "\", ";

			const std::string depDecay = extractTypeFromSharedPtr(dep);
			if (dep != depDecay) {
				os << "\t\"" << dep << "\" -> \"";
				if (std::ranges::find(interfaces, depDecay) == interfaces.end()) {
					os << depDecay << "\";" << std::endl;
				} else {
					os << interfaceLabel(depDecay) << "\"  [arrowhead = vee];" << std::endl;
				}
			}
		}
		os << "\t\"" << impl << "\"";
		if (!deps.empty()) {
			os << " -> " << deps.substr(0, deps.size() - 2) << " [arrowhead = vee, style = dashed]";
		}
		os << ";" << std::endl;
	}

	for (const auto& [iface, impl, priority] : injector.routes()) {
		os << "\t\"" << interfaceLabel(iface) << "\" -> \"" << impl << "\" [arrowhead = onormal, style = dashed, label = \"p = " << priority << "\"];"
		   << std::endl;
	}

	os << "}";
	return os;
}
} // namespace ModS

#endif
