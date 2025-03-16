#include <thread>

#include <DMALibrary/Memory/Memory.h>

#include "MainThread.h"

#include "API/API.h"

#include "Structs/Inventory.h"
#include "Structs/math.h"

#include "Cheat/Features/Character.h"

namespace cheeto
{
	std::unordered_map<std::string, Items::ui_items> items_data;
	std::unordered_map<std::string, Items::ui_items> perks_data;
	std::unordered_map<std::string, Items::ui_items> models_data;
	PlayerInventory* writeInventory = nullptr;
	bool init = false;
}

void cheeto::populate_inventory()
{
	weps tmp_weapons;
	WeaponInfo info = mem.Read<WeaponInfo>(reinterpret_cast<uintptr_t>(&writeInventory->Kinetic));
	tmp_weapons.kinetic.ID = info.ID;
	tmp_weapons.kinetic.Model = info.Model;
	mem.Read(reinterpret_cast<uintptr_t>(&writeInventory->KineticPerks), &tmp_weapons.kinetic.perks, sizeof(tmp_weapons.kinetic.perks));

	info = mem.Read<WeaponInfo>(reinterpret_cast<uintptr_t>(&writeInventory->Energy));
	tmp_weapons.energy.ID = info.ID;
	tmp_weapons.energy.Model = info.Model;
	mem.Read(reinterpret_cast<uintptr_t>(&writeInventory->EnergyPerks), &tmp_weapons.energy.perks, sizeof(tmp_weapons.energy.perks));

	info = mem.Read<WeaponInfo>(reinterpret_cast<uintptr_t>(&writeInventory->Heavy));
	tmp_weapons.heavy.ID = info.ID;
	tmp_weapons.heavy.Model = info.Model;
	mem.Read(reinterpret_cast<uintptr_t>(&writeInventory->HeavyPerks), &tmp_weapons.heavy.perks, sizeof(tmp_weapons.heavy.perks));
	ctx.m_settings.Set<weps>(&ctx.m_settings.inventory_weapons, tmp_weapons);

	armor_perks tmp_armor;
	mem.Read(reinterpret_cast<uintptr_t>(&writeInventory->ArmorPerks), tmp_armor.perks, sizeof(tmp_armor.perks));
	ctx.m_settings.Set<armor_perks>(&ctx.m_settings.inventory_armor, tmp_armor);
}

void cheeto::updateInventoryAPI()
{
	if (ctx.m_destiny_api->has_characters() && ctx.m_destiny_api->has_profiles())
	{
		ctx.m_destiny_api->UnlockOrLockGhost(ctx.m_destiny_api->GetProfile(), ctx.m_destiny_api->GetCharacter());
	}
}

bool WorldToScreen(Vector world, Vector2& out, VMatrix matrix, float fov)
{
	Vector vPosition;
	vPosition = world - *reinterpret_cast<Vector*>(&matrix.m[3]);

	const float x = vPosition.Dot(*reinterpret_cast<Vector*>(&matrix.m[0]));
	const float y = vPosition.Dot(*reinterpret_cast<Vector*>(&matrix.m[1]));
	const float z = vPosition.Dot(*reinterpret_cast<Vector*>(&matrix.m[2]) * -1.f);

	if (z < 0.0)
		return false;

	const ImVec2 res = ctx.m_settings.menu_size;
	const Vector2 resolution = {res.x, res.y};
	const Vector2 center = Vector2(resolution.x / 2, resolution.y / 2);

	out.x = static_cast<float>(center.x) * (1 + x / fov / z);
	out.y = static_cast<float>(center.y) * (1 - y / fov / z * 1.777777777777778);

	return !(out.x <= 0 || out.x >= (float)resolution.x || out.y <= 0 || out.y >= (float)resolution.y);
}

float GetCameraDistanceFactor(Vector world, VMatrix matrix)
{
	Vector vPosition;
	vPosition = world - *reinterpret_cast<Vector*>(&matrix.m[3]);

	const float z = vPosition.Dot(*reinterpret_cast<Vector*>(&matrix.m[2]) * -1.f);

	return z;
}

std::vector<Vector2> test_players = { };

int cheeto::main_thread()
{
	init = false;

	//mem.DumpMemory(mem.GetBaseDaddy("Destiny2.exe"), "fucky_destiny2.exe");

	if (!Keyboard::InitKeyboard())
		ctx.m_logger->Error(xorstr_("Failed to find keyboard\n"));
	else
		ctx.m_logger->Succes(xorstr_("Initialized keyboard\n"));

	if (!mem.vHandle)
	{
		ctx.m_logger->Error(xorstr_("Invalid vHandle on main thread\n"));
		return 0;
	}

	size_t base = mem.GetBaseDaddy(xorstr_("Destiny2.exe"));
	size_t size = mem.GetBaseSize(xorstr_("Destiny2.exe"));
	ctx.m_logger->Info(xorstr_("Base -> 0x%llx, size -> 0x%lx\n"), base, size);
	//mem.DumpMemory(mem.GetBaseDaddy("Destiny2.exe"), "C:\\Users\\Gerald\\Documents\\DMA\\Destiny2-5-8-2024");
	/*uintptr_t IdleArray = mem.Read<uintptr_t>(ptr + 0x40);
	int count = mem.Read<int>(ptr + 0x48);

	if (count > 0)
	{
		uintptr_t v10 = mem.Read<uintptr_t>(IdleArray);
	}*/

	uintptr_t InventoryBase = 0;
	//auto addr = mem.FindSignature(xorstr_("01 74 69 67 65 72 5F 66 69 6E 61 6C 00 00 00 00 00 00 00 00 00 00 00 00 00"), base, base + size);
	//uintptr_t InventoryBase = mem.Read<uintptr_t>(mem.Read<uintptr_t>(addr + 0x5A0) + 0xE250) + 0x512DBC;
	BYTE pattern[] = {

		0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};
	if (!InventoryBase)
		InventoryBase = mem.AOBScanVad(pattern);
	while (!InventoryBase)
	{
		InventoryBase = mem.AOBScanVad(pattern);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	uintptr_t InventoryPtr = InventoryBase + 0xC5C;
	auto test = mem.Read<PlayerInventory>(InventoryPtr);
	writeInventory = reinterpret_cast<PlayerInventory*>(InventoryPtr);
	ctx.m_logger->Succes("Initialized Cheat\n");

	//Wait for inventory to be populated before we continue.
	ctx.m_logger->Info(xorstr_("Waiting for inventory to be populated (Get in game)\n"));
	while (true)
	{
		auto kinetic = mem.Read<WeaponInfo>(reinterpret_cast<uintptr_t>(&writeInventory->Kinetic));
		if (kinetic.ID != 0)
			break;
	}
	ctx.m_logger->Succes(xorstr_("Inventory populated\n"));

	weps inventory_weapons = ctx.m_settings.inventory_weapons;
	if (inventory_weapons.kinetic.ID != -1)
		mem.Write<short>(reinterpret_cast<uintptr_t>(&writeInventory->Kinetic.ID), inventory_weapons.kinetic.ID);
	if (inventory_weapons.energy.ID != -1)
		mem.Write<short>(reinterpret_cast<uintptr_t>(&writeInventory->Energy.ID), inventory_weapons.energy.ID);
	if (inventory_weapons.heavy.ID != -1)
		mem.Write<short>(reinterpret_cast<uintptr_t>(&writeInventory->Heavy.ID), inventory_weapons.heavy.ID);

	populate_inventory();

	armor_perks inventory_armor = ctx.m_settings.inventory_armor;

	auto startTime = std::chrono::high_resolution_clock::now();
	init = true;
	while (true)
	{
		if (UII::Nav::restart)
		{
			ctx.m_logger->Info(xorstr_("Restarting cheat\n"));
			mem.CloseMemory();
			UII::Nav::restart = false;
			break;
		}
		try
		{
			//Update once in a while
			// Get the current time point again
			auto now = std::chrono::high_resolution_clock::now();

			// Calculate the elapsed time duration
			auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
			/*Check if they load a new config, if they do modify the guns that aren't matching & update old inventory variable to match new ones*/
			inventory_weapons = ctx.m_settings.inventory_weapons;
			if (config_two)
			{
				inventory_weapons = ctx.m_settings.inventory_weapons;
				auto handle = mem.CreateScatterHandle();
				if (inventory_weapons.kinetic.ID != -1)
				{
					mem.AddScatterWriteRequest(handle, reinterpret_cast<uintptr_t>(&writeInventory->Kinetic.ID), &inventory_weapons.kinetic.ID, sizeof(short));
					mem.AddScatterWriteRequest(handle, reinterpret_cast<uintptr_t>(&writeInventory->Kinetic.Model), &inventory_weapons.kinetic.Model, sizeof(short));
				}
				if (inventory_weapons.energy.ID != -1)
				{
					mem.AddScatterWriteRequest(handle, reinterpret_cast<uintptr_t>(&writeInventory->Energy.ID), &inventory_weapons.energy.ID, sizeof(short));
					mem.AddScatterWriteRequest(handle, reinterpret_cast<uintptr_t>(&writeInventory->Energy.Model), &inventory_weapons.energy.Model, sizeof(short));
				}
				if (inventory_weapons.heavy.ID != -1)
				{
					mem.AddScatterWriteRequest(handle, reinterpret_cast<uintptr_t>(&writeInventory->Heavy.ID), &inventory_weapons.heavy.ID, sizeof(short));
					mem.AddScatterWriteRequest(handle, reinterpret_cast<uintptr_t>(&writeInventory->Heavy.Model), &inventory_weapons.heavy.Model, sizeof(short));
				}
				weps perk_weapons = ctx.m_settings.inventory_weapons;

				for (size_t i = 0; i < 11; i++)
				{
					perk_weapons.kinetic.perks[i];
					mem.AddScatterWriteRequest(handle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->KineticPerks) + (i * sizeof(short)), &perk_weapons.kinetic.perks[i].id, sizeof(short));
					perk_weapons.energy.perks[i];
					mem.AddScatterWriteRequest(handle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->EnergyPerks) + (i * sizeof(short)), &perk_weapons.energy.perks[i].id, sizeof(short));
					perk_weapons.heavy.perks[i];
					mem.AddScatterWriteRequest(handle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->HeavyPerks) + (i * sizeof(short)), &perk_weapons.heavy.perks[i].id, sizeof(short));
				}

				armor_perks perk_armor = ctx.m_settings.inventory_armor;
				for (size_t i = 0; i < 69; i++)
				{
					mem.AddScatterWriteRequest(handle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->ArmorPerks) + (i * sizeof(short)), &perk_armor.perks[i].id, sizeof(short));
				}
				mem.ExecuteWriteScatter(handle);
				mem.CloseScatterHandle(handle);
				if (ctx.m_destiny_api->has_profiles() && ctx.m_destiny_api->has_characters())
					std::thread(updateInventoryAPI).detach();
				config_two = false;
				Sleep(1000);
				populate_inventory(); //Update inv info again
			}

			// Check if the desired duration has elapsed
			if (elapsedTime >= 10)
			{
				//3 sec?
				populate_inventory(); //Update inv info again
				startTime = now;
			}

			Features::Character::Run();
		}
		catch (...)
		{
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	//recursion
	main_thread();
	return 0;
}
