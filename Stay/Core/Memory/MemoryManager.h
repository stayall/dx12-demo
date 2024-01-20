#pragma once

#include <memory>

namespace stay::Memory
{
	template<typename T>
	T* Malloc(size_t size)
	{
		void* rawMemory = malloc(size);
		if (!rawMemory) {
			throw std::bad_alloc();
		}

		return reinterpret_cast<T*>(rawMemory);
	}

	void Free(void* p);
	
}