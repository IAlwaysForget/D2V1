#pragma once
#include <cstdint>
#include <vector>

#include "Cheat/Structs/math.h"

enum EntityType
{
	ACTIVE = 0,
	INACTIVE,
	UNK
};

struct EntityPtr
{
	uintptr_t addr = 0;
	int count = 0;
	int unk = 0;
};

struct Entity
{
	Entity(uintptr_t addr, EntityType type) : addr(addr), type(type), pos(Vector(0, 0, 0)), entity_addr(0)
	{
	}

	Entity() : addr(0), entity_addr(0), pos(Vector(0, 0, 0)), type(EntityType::UNK)
	{
	}

	uintptr_t addr;
	uintptr_t entity_addr;
	int entity_type;
	Vector pos;
	Vector velocity;
	EntityType type;
};

class Cache
{
private:
	std::vector<Entity> m_Entities;

	VMatrix m_matrix;
	float m_fov;

public:
	Cache()
	{
	};

	~Cache()
	{
	};

	void FastCache();

	float GetFov() const { return m_fov; }
	VMatrix GetMatrix() const { return m_matrix; }

	std::vector<Entity> GetEntities() const { return m_Entities; }
};

extern Cache g_Cache;
