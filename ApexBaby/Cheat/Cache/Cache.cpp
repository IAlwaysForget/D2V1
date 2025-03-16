#include "Cache.h"

#include <cstdint>

#include "xorstr.hpp"
#include "DMALibrary/Memory/Memory.h"

void Cache::FastCache()
{
	static uintptr_t ptr = 0;
	static uintptr_t viewmatrix_offset = 0;
	std::vector<Entity> tmp_entities = { };
	//Sorry not sorry. removed.
}

Cache g_Cache;
