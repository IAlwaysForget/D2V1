#pragma once

#include "context.h"
#include "Structs/Inventory.h"
#include "Structs/math.h"

extern std::vector<Vector2> test_players;
extern bool WorldToScreen(Vector world, Vector2& out, VMatrix matrix, float fov);
extern float GetCameraDistanceFactor(Vector world, VMatrix matrix);

namespace cheeto
{
	struct current_weapon
	{
		WeaponInfo weapon;
		short perks[11]; //size 11
		short armor_perks[69]; //size 11
	};

	struct current_armor
	{
		ArmorInfo armor;
	};

	//extern std::vector<std::vector<Items::ui_items>> items_data;
	extern std::unordered_map<std::string, Items::ui_items> items_data;
	extern std::unordered_map<std::string, Items::ui_items> perks_data;
	extern std::unordered_map<std::string, Items::ui_items> models_data;
	extern PlayerInventory* writeInventory;
	extern int main_thread();
	extern void populate_inventory();
	extern void updateInventoryAPI();
	extern bool init;
}
