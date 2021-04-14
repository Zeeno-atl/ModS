#pragma once
#ifndef _MODS_POINTER_HPP
#	define _MODS_POINTER_HPP

#	include <memory>

namespace ModS {

struct Pointer {
	void* value{};
	void (*deleter)(void*){};

	template<typename T>
	std::shared_ptr<T> toSharedPtr() const {
		std::shared_ptr<T> ptr;
		ptr.reset(value, deleter);
		return ptr;
	}

	//This believes you know where it points -> deleter is known
	template<typename T>
	std::unique_ptr<T> toUniquePtr() const {
		return {value};
	}
};

} // namespace ModS

#endif // POINTER_HPP
