#include "MemoryManager.h"


#include <stdlib.h>

#include "Platform/Windows/WinError.h"

namespace stay::Memory
{
	

	void Free(void* p)
	{
		ASSERT(p != nullptr);
		free(p);
	}
}
