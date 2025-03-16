#include "Draw.h"

#include <regex>

#include "Font/newIcons.h"
#include "Font/textFont.h"
#include "Discord/discord.h"
#include "context.h"
#include "Cheat/MainThread.h"
#include "Cheat/API/API.h"
#include "DMALibrary/Memory/Memory.h"
#include "Utils/Mouse/kmbox.h"
#include "Utils/Mouse/PicoW.h"

#include "Cheat/Features/Character.h"

void c_draw::Init(Fuser& fuse)
{
	Font::initializeFonts();
	Discord::Init();

	this->fuser = fuse;
	//this->fuslie
}

void c_draw::Run()
{
	if (GetAsyncKeyState(ctx.m_settings.menu_menu_key_hotkey) & 0x1)
		ctx.m_settings.Toggle(&ctx.m_settings.menu_menu_key);

	if (!ctx.m_settings.menu_menu_key)
		return;

	this->Begin();
	char fps[64];
	auto& io = ImGui::GetIO();
	sprintf_s(fps, xorstr_("Moe Destiny V0.3 (%.1f)"), (io.Framerate));

	this->DisplayBrand(fps);
	this->End();

	this->BeginMenu();
	this->MoveWindow();
	this->DrawBackground();

	//display tab 
	this->DisplayTab({
		TabButtons("Visuals", ICON_ESP, 9.5f),
		TabButtons("Inventory", ICON_WEAPONS, 9.5f),
		TabButtons("Perks", ICON_FIVEM, 9.5f),
		TabButtons("Character", ICON_USER, 9.5f),
		TabButtons("Movement", ICON_TELEPORT, 9.5f),
		TabButtons("Miscellaneous", ICON_MISC, 9.5f)
	});

	this->DisplayConfigs();
	this->DisplayPadding();

	if (this->IsActive("Visuals"))
	{
		this->Visuals();
	}
	else if (this->IsActive("Inventory"))
	{
		this->Inventory();
	}
	else if (this->IsActive("Perks"))
	{
		this->Perks();
	}
	else if (this->IsActive("Movement"))
	{
		this->Teleport();
	}
	else if (this->IsActive("Character"))
	{
		this->Character();
	}
	else if (this->IsActive("Miscellaneous"))
	{
		this->Miscellaneous();
	}

	this->DisplayModals();
	this->EndMenu();
}

void c_draw::Visuals()
{
	UII::Controls::beginSection(xorstr_("Inventory"), ImVec2(535, 446));
	{
		UII::Controls::Toggle(xorstr_("Enable ESP"), xorstr_("Enable ESP"), &ctx.m_settings.visuals_enable_esp);
	}
	UII::Controls::endSection();
}

void c_draw::Teleport()
{
	UII::Controls::beginSection(xorstr_("Fly"), ImVec2(535, 446));
	{
		UII::Controls::Toggle(xorstr_("Enable"), xorstr_("Fly around"), &ctx.m_settings.movement_noclip);
		UII::Controls::Combo(xorstr_("Style"), {xorstr_("Classic Fly"), xorstr_("Noclip")}, ctx.m_settings.movement_noclip_style);
		UII::Controls::Hotkey(xorstr_("Fly Key (Hold)"), &ctx.m_settings.movement_noclip_key);
		UII::Controls::SliderInt(xorstr_("Fly Speed"), &ctx.m_settings.movement_noclip_normal_speed, 1, 20);
		UII::Controls::SliderInt(xorstr_("Fly Speed (Hold Shift)"), &ctx.m_settings.movement_noclip_fast_speed, 1, 20);
	}
	UII::Controls::endSection();
}

void c_draw::Character()
{
	if (!cheeto::init)
	{
		UII::Controls::beginSection("we waiting", ImVec2(535, 103));
		{
			UII::Controls::Button(xorstr_("Waiting for cheat to be fully initialized"), xorstr_("Waiting"));
		}
		UII::Controls::endSection();
		return;
	}
	UII::Controls::beginSection(xorstr_("Stat Editor"));
	{
		static bool limiter = true;
		UII::Controls::Toggle(xorstr_("Enforce Stat Limits"), xorstr_("Disabling might ban you"), &limiter);

		int cumulative = ctx.m_settings.character_stat_mob() + ctx.m_settings.character_stat_res() + ctx.m_settings.character_stat_rec() + ctx.m_settings.character_stat_dis() + ctx.m_settings.character_stat_int() + ctx.m_settings.character_stat_str();
		int remaining_points = Features::Character::max_stat_limit - cumulative;

		auto get_max_stat = [&](int current_stat) -> int
		{
			return (100 < (remaining_points + current_stat)) ? 100 : (remaining_points + current_stat);
		};

		if (!limiter)
		{
			remaining_points = Features::Character::max_stat_limit;
		}

		int max_mobility = get_max_stat(ctx.m_settings.character_stat_mob());
		int max_resilience = get_max_stat(ctx.m_settings.character_stat_res());
		int max_recovery = get_max_stat(ctx.m_settings.character_stat_rec());
		int max_discipline = get_max_stat(ctx.m_settings.character_stat_dis());
		int max_intellect = get_max_stat(ctx.m_settings.character_stat_int());
		int max_strength = get_max_stat(ctx.m_settings.character_stat_str());

		UII::Controls::SliderInt(xorstr_("Mobility"), &ctx.m_settings.character_stat_mob, 0, max_mobility);
		UII::Controls::SliderInt(xorstr_("Resilience"), &ctx.m_settings.character_stat_res, 0, max_resilience);
		UII::Controls::SliderInt(xorstr_("Recovery"), &ctx.m_settings.character_stat_rec, 0, max_recovery);
		UII::Controls::SliderInt(xorstr_("Discipline"), &ctx.m_settings.character_stat_dis, 0, max_discipline);
		UII::Controls::SliderInt(xorstr_("Intellect"), &ctx.m_settings.character_stat_int, 0, max_intellect);
		UII::Controls::SliderInt(xorstr_("Strength"), &ctx.m_settings.character_stat_str, 0, max_strength);

		if (Features::Character::applied_stats)
		{
			UII::Controls::Button(xorstr_("Setting up stats..."), xorstr_("Please re-equip your gear."));
		}
		else
		{
			if (UII::Controls::Button(xorstr_("Apply Stat"), xorstr_("Stat editin time")))
			{
				Features::Character::Stats stats;
				stats.mobility = ctx.m_settings.character_stat_mob();
				stats.resilience = ctx.m_settings.character_stat_res();
				stats.recovery = ctx.m_settings.character_stat_rec();
				stats.discipline = ctx.m_settings.character_stat_dis();
				stats.intellect = ctx.m_settings.character_stat_int();
				stats.strength = ctx.m_settings.character_stat_str();

				Features::Character::SetStats(stats, limiter);
			}
		}
	}
	UII::Controls::endSection();
	UII::Controls::nextSection();
	UII::Controls::beginSection(xorstr_("Skill Editor"));
	UII::Controls::InputInt(xorstr_("Super"), &ctx.m_settings.character_skill_super);
	UII::Controls::InputInt(xorstr_("Grenade"), &ctx.m_settings.character_skill_grenade);
	UII::Controls::InputInt(xorstr_("Melee"), &ctx.m_settings.character_skill_melee);
	UII::Controls::InputInt(xorstr_("Class-Specific Skill"), &ctx.m_settings.character_skill_class_ability);
	if (Features::Character::applied_skills)
	{
		UII::Controls::Button(xorstr_("Setting up skills..."), xorstr_("Please re-equip your subclass."));
	}
	else
	{
		if (UII::Controls::Button(xorstr_("Apply Skills"), xorstr_("Skill editin' time")))
		{
			Features::Character::Skills skills;
			skills.classability = ctx.m_settings.character_skill_class_ability();
			skills.grenade = ctx.m_settings.character_skill_grenade();
			skills.super = ctx.m_settings.character_skill_super();
			skills.melee = ctx.m_settings.character_skill_melee();

			Features::Character::SetSkills(skills);
		}
	}
	if (UII::Controls::Button(xorstr_("Fetch Skill Info"), xorstr_("Gets your current skills")))
	{
		Features::Character::ReadSkills();
	}
	UII::Controls::endSection();
	//UII::Controls::beginSection()
}

void call_api()
{
	ctx.m_destiny_api->Init();
	if (ctx.m_destiny_api->IsConnected())
	{
		if (ctx.m_destiny_api->has_profiles())
		{
			if (ctx.m_destiny_api->GetProfiles().size() > 1)
				UII::ModalController::ProfileSelector = true;
		}
	}
}

std::unordered_map<std::string, Items::ui_items> searchItems(std::unordered_map<std::string, Items::ui_items> itemsMap, std::string searchTerm)
{
	std::unordered_map<std::string, Items::ui_items> result;

	std::string searchTermLower = searchTerm;
	std::transform(searchTermLower.begin(), searchTermLower.end(), searchTermLower.begin(),
	               [](unsigned char c) { return std::tolower(c); });

	for (std::pair<std::string, Items::ui_items> entry : itemsMap)
	{
		std::string itemNameLower = entry.second.name;
		std::transform(itemNameLower.begin(), itemNameLower.end(), itemNameLower.begin(),
		               [](unsigned char c) { return std::tolower(c); });

		if (itemNameLower.find(searchTermLower) != std::string::npos)
			result[entry.first] = entry.second;
	}

	return result;
}

std::unordered_map<std::string, Items::ui_items> tmp_item_data;
std::unordered_map<std::string, Items::ui_items> tmp_perk_data;
char search[256];
char search_perk[256];

void apply_weapon()
{
	auto weapon_data = cheeto::items_data[ctx.m_settings.inventory_current_selected_item_hash];
	weps inventory_weapons = ctx.m_settings.inventory_weapons;
	if (ctx.m_settings.inventory_weapon_type == 0)
	{
		inventory_weapons.kinetic.ID = weapon_data.item_id;
		mem.Write<short>(reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Kinetic.ID), weapon_data.item_id);
		if (weapon_data.model_id > 0)
			mem.Write<short>(reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Kinetic.Model), weapon_data.model_id);
	}
	if (ctx.m_settings.inventory_weapon_type == 1)
	{
		inventory_weapons.energy.ID = weapon_data.item_id;
		mem.Write<short>(reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Energy.ID), weapon_data.item_id);
		if (weapon_data.model_id > 0)
			mem.Write<short>(reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Energy.Model), weapon_data.model_id);
	}
	if (ctx.m_settings.inventory_weapon_type == 2)
	{
		inventory_weapons.heavy.ID = weapon_data.item_id;
		mem.Write<short>(reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Heavy.ID), weapon_data.item_id);
		if (weapon_data.model_id > 0)
			mem.Write<short>(reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Heavy.Model), weapon_data.model_id);
	}
	if (ctx.m_destiny_api->has_profiles() && ctx.m_destiny_api->has_characters())
	{
		std::thread(cheeto::updateInventoryAPI).detach();
	}
	ctx.m_settings.Set<weps>(&ctx.m_settings.inventory_weapons, inventory_weapons);
}

void c_draw::Inventory()
{
	static bool once = false;
	if (!once && cheeto::items_data.size() > 1)
	{
		tmp_item_data = cheeto::items_data;
		tmp_perk_data = cheeto::perks_data;
		once = true;
	}
	UII::Controls::beginSection(xorstr_("Inventory"), ImVec2(535, 446));
	{
		if (UII::Controls::Button("Connect", "Click to Connect API"))
		{
			std::thread(call_api).detach();
		}
		if (ctx.m_destiny_api->IsConnected())
		{
			if (ctx.m_destiny_api->has_profiles())
			{
				if (ctx.m_destiny_api->GetProfiles().size() > 1)
				{
					if (UII::Controls::Button("Select Profile", "Profile Selector"))
					{
						if (ctx.m_destiny_api->has_profiles())
							UII::ModalController::ProfileSelector = true;
					}
				}
			}

			if (UII::Controls::Input("Search", search, IM_ARRAYSIZE(search))) //comment search fr
			{
				tmp_item_data = searchItems(cheeto::items_data, search);
			}
			//Probably change this to for example: Hand Cannon, Sword, Sniper whatever else there is.
			UII::Controls::Combo("Weapon Type", {"Kinetic", "Energy", "Heavy"}, ctx.m_settings.inventory_weapon_type);
			if (tmp_item_data.size() > 0)
			{
				if (UII::Controls::AddItem(tmp_item_data, ctx.m_settings.inventory_current_selected_item))
				{
					std::thread(apply_weapon).detach();
				}
			}
			else
				UII::Controls::Button("No Items found", "It's probably still parsing...");
		}
	}
	UII::Controls::endSection();

	/*UII::Controls::sameSection();

	UII::Controls::beginSection("Misc", ImVec2(250, 173 - 5));
	{
	}
	UII::Controls::endSection();*/
}

std::vector<std::string> menu_size =
{
	"640 x 480",
	"800 x 600",
	"1024 x 768",
	"1280 x 720",
	"1280 x 800",
	"1366 x 768",
	"1440 x 900",
	"1600 x 900",
	"1680 x 1050",
	"1920 x 1080",
	"1920 x 1200",
	"2560 x 1440",
	"2880 x 1800",
	"3440 x 1440",
	"3840 x 2160"
};

enum menu_types
{
	SIZE_640_480,
	SIZE_800_600,
	SIZE_1024_768,
	SIZE_1280_720,
	SIZE_1280_800,
	SIZE_1366_768,
	SIZE_1440_900,
	SIZE_1600_900,
	SIZE_1680_1050,
	SIZE_1920_1080,
	SIZE_1920_1200,
	SIZE_2560_1440,
	SIZE_2880_1800,
	SIZE_3440_1440,
	SIZE_3840_2160
};

void setConfigPerk(int current_weapon, int current_slot, std::unordered_map<std::string, Items::ui_items> tmp_data)
{
	weps inventory_weapons = ctx.m_settings.inventory_weapons;
	armor_perks inventory_armor = ctx.m_settings.inventory_armor;

	auto weapon_data = tmp_data[ctx.m_settings.inventory_current_selected_perk_hash];
	if (current_weapon == 0)
	{
		inventory_weapons.kinetic.perks[current_slot].id = weapon_data.item_id;
		ctx.m_settings.Set<weps>(&ctx.m_settings.inventory_weapons, inventory_weapons);
	}
	else if (current_weapon == 1)
	{
		inventory_weapons.energy.perks[current_slot].id = weapon_data.item_id;
		ctx.m_settings.Set<weps>(&ctx.m_settings.inventory_weapons, inventory_weapons);
	}
	else if (current_weapon == 2)
	{
		inventory_weapons.heavy.perks[current_slot].id = weapon_data.item_id;
		ctx.m_settings.Set<weps>(&ctx.m_settings.inventory_weapons, inventory_weapons);
	}
	else if (current_weapon == 3)
	{
		inventory_armor.perks[current_slot].id = weapon_data.item_id;
		ctx.m_settings.Set<armor_perks>(&ctx.m_settings.inventory_armor, inventory_armor);
	}
}

void c_draw::Perks()
{
	if (!cheeto::init)
	{
		UII::Controls::beginSection("we waiting", ImVec2(535, 103));
		{
			UII::Controls::Button("Waiting for inventory to be initialized", "Waiting");
		}
		UII::Controls::endSection();
		return;
	}
	static int current_selected_perk_slot = 0;

	// these need to be moved elsewhere..
	static int current_weapon_inventory_type = 0;
	static std::vector<Items::ui_items> current_item_perks;
	std::vector<Items::ui_items> temp_item_perks;

	static bool once = false;
	if (!once && cheeto::perks_data.size() > 1)
	{
		tmp_perk_data = cheeto::perks_data;
		once = true;
	}

	int Perk_UI_Count = 0;
	if (current_weapon_inventory_type == 0) Perk_UI_Count = 11;
	if (current_weapon_inventory_type == 1) Perk_UI_Count = 69;

	auto real_pos = ImGui::GetCursorScreenPos();
	//When config changes update the perks of the current selected weapon
	if (ctx.m_settings.has_config_changed())
	{
		weps inventory_weapons = ctx.m_settings.inventory_weapons;
		current_item_perks.clear();
		current_item_perks.resize(Perk_UI_Count);
		for (size_t ii = 0; ii < Perk_UI_Count; ii++)
		{
			perky* perks = new perky[Perk_UI_Count];
			if (ctx.m_settings.inventory_current_selected_weapon == 0)
			{
				std::memcpy(perks, inventory_weapons.kinetic.perks, sizeof(perky) * Perk_UI_Count);
			}
			else if (ctx.m_settings.inventory_current_selected_weapon == 1)
			{
				std::memcpy(perks, inventory_weapons.energy.perks, sizeof(perky) * Perk_UI_Count);
			}
			else if (ctx.m_settings.inventory_current_selected_weapon == 2)
			{
				std::memcpy(perks, inventory_weapons.heavy.perks, sizeof(perky) * Perk_UI_Count);
			}

			int perk_id = perks[ii].id;
			//find perk id in cheeto::perks_data
			for (auto& perk : cheeto::perks_data)
			{
				if (perk.second.item_id == perk_id)
				{
					current_item_perks[ii] = perk.second;
					break;
				}
			}
			delete[] perks;
		}
		config_two = true;
	}

	UII::Controls::beginSection("Current Weapons", ImVec2(535, 103));
	{
		auto drawList = ImGui::GetForegroundDrawList();
		auto pos = ImGui::GetCursorScreenPos();
		auto sizeAvail = ImGui::GetContentRegionAvail();

		float bigLength = (sizeAvail.x - 65.f) / 3/*3 for size of weapons*/;
		float length = 65.f;
		weps inventory_weapons = ctx.m_settings.inventory_weapons;
		armor_perks inventory_armor = ctx.m_settings.inventory_armor;
		for (size_t i = 0; i < 3/*3 for size of weapons*/; i++)
		{
			if (inventory_weapons.kinetic.ID == -1)
				break;

			bool hovering = false;

			ImVec2 start = ImVec2(pos.x + 5.f + (i * bigLength), pos.y + 5.f);
			ImVec2 end = ImVec2(pos.x + 5.f + (i * bigLength - 5.f) + bigLength, pos.y + 65.f);

			drawList->AddRectFilled(start, end, ImColor(66, 66, 66, 25), 5.f);
			//Draw Picture 50x50 with 5 of padding
			drawList->AddRectFilled(ImVec2(start.x + 5.f, start.y + 5.f), ImVec2(start.x + 55.f, start.y + 55.f), Colors::mainSidebarClockBackground, 5.f);

			if (ImGui::IsMouseHoveringRect(start, end))
			{
				hovering = true;
				if (ImGui::IsMouseClicked(0))
				{
					ctx.m_settings.Set<int>(&ctx.m_settings.inventory_current_selected_weapon, i);
					current_weapon_inventory_type = 0;
					ctx.m_settings.Set<bool>(&ctx.m_settings.inventory_selected_armor, false);

					current_item_perks.clear();
					current_item_perks.resize(Perk_UI_Count);
					for (size_t ii = 0; ii < Perk_UI_Count; ii++)
					{
						perky* perks = new perky[Perk_UI_Count];
						if (ctx.m_settings.inventory_current_selected_weapon == 0)
						{
							std::memcpy(perks, inventory_weapons.kinetic.perks, sizeof(perky) * Perk_UI_Count);
						}
						else if (ctx.m_settings.inventory_current_selected_weapon == 1)
						{
							std::memcpy(perks, inventory_weapons.energy.perks, sizeof(perky) * Perk_UI_Count);
						}
						else if (ctx.m_settings.inventory_current_selected_weapon == 2)
						{
							std::memcpy(perks, inventory_weapons.heavy.perks, sizeof(perky) * Perk_UI_Count);
						}

						int perk_id = perks[ii].id;
						//find perk id in cheeto::perks_data
						for (auto& perk : cheeto::perks_data)
						{
							if (perk.second.item_id == perk_id)
							{
								current_item_perks[ii] = perk.second;
								break;
							}
						}
						delete[] perks;
					}
				}
			}
			float alpha = Animation::GetAnimData("perk" + std::to_string(i), hovering, 20.f, 0.4f);
			if (ctx.m_settings.inventory_current_selected_weapon == i)
				alpha = Animation::GetAnimData("perk" + std::to_string(i), true, 20.f, 0.4f);

			ImColor backgroundCorner = ImColor(94, 255, 176, (int)(alpha * 150));
			drawList->AddRect(start, end, backgroundCorner, 5.f);

			if (cheeto::items_data.size())
			{
				for (auto& item : cheeto::items_data)
				{
					int weapon_id = -1;
					if (i == 0)
						weapon_id = inventory_weapons.kinetic.ID;
					else if (i == 1)
						weapon_id = inventory_weapons.energy.ID;
					else if (i == 2)
						weapon_id = inventory_weapons.heavy.ID;
					if (weapon_id == item.second.item_id)
					{
						if (item.second.icon_texture)
							drawList->AddImageRounded((ImTextureID)item.second.icon_texture, ImVec2(start.x + 5.f, start.y + 5.f), ImVec2(start.x + 55.f, start.y + 55.f), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), 5.f);
						else
							Items::loadSingleItemImage(std::ref(item.second), std::ref(cheeto::items_data));

						bool changed_name = false;
						std::string tmpname = item.second.name;
						ImVec2 tmpnameSize = ImGui::CalcTextSize(tmpname.c_str());
						if (tmpnameSize.x + 70.f > bigLength - 5.f)
						{
							while (tmpnameSize.x + 70.f > bigLength - 5.f)
							{
								tmpname = tmpname.substr(0, tmpname.size() - 6);
								tmpname += "...";
								tmpnameSize = ImGui::CalcTextSize(tmpname.c_str());
							}
							changed_name = true;
						}

						std::string tmpdescname = item.second.name;
						ImVec2 tmpdescnameSize = ImGui::CalcTextSize(tmpdescname.c_str());
						if (tmpdescnameSize.x + 60.f > bigLength - 5.f)
						{
							while (tmpdescnameSize.x + 60.f > bigLength - 5.f)
							{
								tmpdescname = tmpdescname.substr(0, tmpdescname.size() - 6);
								tmpdescname += "...";
								tmpdescnameSize = ImGui::CalcTextSize(tmpdescname.c_str());
							}
						}

						if (changed_name && hovering)
							UII::Tooltip::tip(item.second.name);

						drawList->AddText(ImVec2(start.x + 60.f, start.y + 15.f), Colors::whiteText, tmpname.c_str());
						//Description
						drawList->AddText(ImVec2(start.x + 60.f, start.y + 35.f), Colors::userTag, tmpdescname.c_str());
						break;
					}
				}
			}
		}

		//Armor button
		bool hovering = false;
		ImVec2 start = ImVec2(pos.x + 5.f + (3/*3 for size of weapons*/ * bigLength), pos.y + 5.f);
		ImVec2 end = ImVec2(pos.x + 5.f + (3/*3 for size of weapons*/ * bigLength - 5.f) + length, pos.y + 65.f);

		drawList->AddRectFilled(start, end, ImColor(66, 66, 66, 25), 5.f);
		//Draw Picture 50x50 with 5 of padding
		drawList->AddRectFilled(ImVec2(start.x + 5.f, start.y + 5.f), ImVec2(start.x + 55.f, start.y + 55.f), Colors::mainSidebarClockBackground, 5.f);

		if (ImGui::IsMouseHoveringRect(start, end))
		{
			hovering = true;
			if (ImGui::IsMouseClicked(0))
			{
				ctx.m_settings.Set<bool>(&ctx.m_settings.inventory_selected_armor, true);
				ctx.m_settings.Set<int>(&ctx.m_settings.inventory_current_selected_weapon, 3);
				current_weapon_inventory_type = 1;
				Perk_UI_Count = 69;
				current_item_perks.clear();
				current_item_perks.resize(Perk_UI_Count);
				for (size_t ii = 0; ii < Perk_UI_Count; ii++)
				{
					perky* perks = new perky[Perk_UI_Count];

					std::memcpy(perks, inventory_armor.perks, sizeof(perky) * Perk_UI_Count);

					int perk_id = perks[ii].id;
					//find perk id in cheeto::perks_data
					for (auto& perk : cheeto::perks_data)
					{
						if (perk.second.item_id == perk_id)
						{
							current_item_perks[ii] = perk.second;
							break;
						}
					}
					delete[] perks;
				}
			}
		}
		float alpha = Animation::GetAnimData("armorperk", hovering, 20.f, 0.4f);
		if (ctx.m_settings.inventory_selected_armor)
			alpha = Animation::GetAnimData("armorperk", true, 20.f, 0.4f);

		ImColor backgroundCorner = ImColor(94, 255, 176, (int)(alpha * 150));
		drawList->AddRect(start, end, backgroundCorner, 5.f);

		drawList->AddText(ImVec2(start.x + 70.f, start.y + 15.f), Colors::whiteText, "");
		//Description
		drawList->AddText(ImVec2(start.x + 70.f, start.y + 35.f), Colors::userTag, "");
	}

	UII::Controls::endSection();

	int current_weapon = ctx.m_settings.inventory_current_selected_weapon;

	if (current_weapon != -1)
	{
		/*Animation starts here (Only one time animation)*/
		float alpha = Animation::GetAnimData("CurrentWeaponPerkThing", true, 20.f, 0.4f);
		auto pos = ImGui::GetCursorScreenPos();
		auto avail = ImGui::GetContentRegionAvail();
		ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(pos.x + 5.f, pos.y + 5.f), ImVec2(pos.x + avail.x, pos.y + avail.y), ImColor(26, 26, 38, (int)(255 - (alpha * 255))));
		/*Animation end*/

		ImGui::SetCursorScreenPos(ImVec2(real_pos.x, real_pos.y + 103.f + 8.f));

		UII::Controls::beginSection("Perks", ImVec2(120, 335));
		{
			auto drawList = ImGui::GetWindowDrawList();
			ImGui::Columns(2, NULL, false);

			current_item_perks.resize(Perk_UI_Count);
			for (size_t i = 0; i < Perk_UI_Count; i++)
			{
				bool hovering = false;

				// Offset the left column by 5 pixels
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 11.0f);

				// Set the cursor position for the Y-axis to center the content vertically
				float offsetY = ImGui::GetCursorPosY() + (25 - ImGui::GetTextLineHeight()) * 0.5f;
				ImGui::SetCursorPosY(offsetY);

				ImVec2 pos = ImGui::GetCursorScreenPos();
				ImVec2 size = ImVec2(40, 40);
				ImGui::PushID(std::to_string(i).c_str());
				if (ImGui::InvisibleButton("", size))
					current_selected_perk_slot = i;
				ImGui::PopID();

				if (ImGui::IsItemHovered())
					hovering = true;

				//Draw Picture 50x50 with 5 of padding
				drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), Colors::mainSidebarClockBackground, 5.f);

				if (current_item_perks[i].item_hash != "")
				{
					if (cheeto::perks_data[current_item_perks[i].item_hash].icon_texture)
						drawList->AddImageRounded((ImTextureID)cheeto::perks_data[current_item_perks[i].item_hash].icon_texture, pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), 5.f);
					else if (strstr(cheeto::perks_data[current_item_perks[i].item_hash].name.c_str(), "Unidentified"))
					{
					}
					else
					{
						Items::loadSingleItemImage(std::ref(cheeto::perks_data[current_item_perks[i].item_hash]), std::ref(cheeto::perks_data));
					}
				}
				else
					drawList->AddText(ImVec2(pos.x + 5.f, pos.y + 5.f), Colors::whiteText, std::to_string(i).c_str());

				float alpha = Animation::GetAnimData("current_weapon" + std::to_string(i), hovering, 20.f, 0.4f);
				if (current_selected_perk_slot == i)
					alpha = Animation::GetAnimData("current_weapon" + std::to_string(i), true, 20.f, 0.4f);

				ImColor backgroundCorner = ImColor(94, 255, 176, (int)(alpha * 150));
				drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), backgroundCorner, 5.f);

				if (hovering)
					if (current_item_perks[i].item_hash != "")
						UII::Tooltip::tip(current_item_perks[i].name + "(" + std::to_string(current_item_perks[i].item_id) + ")");

				if ((i + 1) % 2 == 0 && (i + 1) < Perk_UI_Count) ImGui::NextColumn();
			}
			ImGui::Columns(1);
		}
		UII::Controls::endSection();

		ImGui::SetCursorScreenPos(ImVec2(real_pos.x + 128.f, real_pos.y + 103.f + 8.f));

		UII::Controls::beginSection("Perk Selector", ImVec2(407, 335));
		{
			if (ctx.m_destiny_api->IsConnected())
			{
				if (UII::Controls::Input("#Search", search_perk, IM_ARRAYSIZE(search_perk))) //comment search fr
				{
					//std::lock_guard<std::mutex> lock(Items::itemsDataMutex);
					tmp_perk_data = searchItems(cheeto::perks_data, search_perk);
				}
				bool perk_added = false;
				if (tmp_perk_data.size() > 0)
				{
					if (UII::Controls::AddItem(tmp_perk_data, ctx.m_settings.inventory_current_selected_perk, true))
						perk_added = true;
				}
				else
					UII::Controls::Button("No Items found", "It's probably still parsing...");

				if (perk_added)
				{
					current_item_perks[current_selected_perk_slot] = tmp_perk_data[ctx.m_settings.inventory_current_selected_perk_hash];

					setConfigPerk(current_weapon, current_selected_perk_slot, tmp_perk_data);
					if (current_weapon == 0)
						mem.Write<short>(reinterpret_cast<uintptr_t>(&cheeto::writeInventory->KineticPerks) + (current_selected_perk_slot * sizeof(short)), current_item_perks[current_selected_perk_slot].item_id);
					else if (current_weapon == 1)
						mem.Write<short>(reinterpret_cast<uintptr_t>(&cheeto::writeInventory->EnergyPerks) + (current_selected_perk_slot * sizeof(short)), current_item_perks[current_selected_perk_slot].item_id);
					else if (current_weapon == 2)
						mem.Write<short>(reinterpret_cast<uintptr_t>(&cheeto::writeInventory->HeavyPerks) + (current_selected_perk_slot * sizeof(short)), current_item_perks[current_selected_perk_slot].item_id);
					else if (current_weapon == 3)
						mem.Write<short>(reinterpret_cast<uintptr_t>(&cheeto::writeInventory->ArmorPerks) + (current_selected_perk_slot * sizeof(short)), current_item_perks[current_selected_perk_slot].item_id);

					if (ctx.m_destiny_api->has_profiles() && ctx.m_destiny_api->has_characters())
					{
						std::thread(cheeto::updateInventoryAPI).detach();
					}
				}
			}
			else
			{
				UII::Controls::Button("Waiting...", "Please connect to the Destiny API!");
			}
		}
		UII::Controls::endSection();
	}
}

void c_draw::Miscellaneous()
{
	UII::Controls::beginSection("Main");
	{
		UII::Controls::Toggle("VSync", "Enabled VSync for Overlay", &ctx.m_settings.vsync);
		UII::Controls::Hotkey("Menu Key", &ctx.m_settings.menu_menu_key_hotkey);
		UII::Controls::Combo("Fuser Aspect Ratio", menu_size, ctx.m_settings.menu_size_type);
		switch (ctx.m_settings.menu_size_type)
		{
		case menu_types::SIZE_640_480:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(640, 480));
			break;
		case menu_types::SIZE_800_600:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(800, 600));
			break;
		case menu_types::SIZE_1024_768:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(1024, 768));
			break;
		case menu_types::SIZE_1280_720:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(1280, 720));
			break;
		case menu_types::SIZE_1280_800:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(1280, 800));
			break;
		case menu_types::SIZE_1366_768:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(1366, 768));
			break;
		case menu_types::SIZE_1440_900:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(1440, 900));
			break;
		case menu_types::SIZE_1600_900:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(1600, 900));
			break;
		case menu_types::SIZE_1680_1050:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(1680, 1050));
			break;
		case menu_types::SIZE_1920_1080:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(1920, 1080));
			break;
		case menu_types::SIZE_1920_1200:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(1920, 1200));
			break;
		case menu_types::SIZE_2560_1440:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(2560, 1440));
			break;
		case menu_types::SIZE_2880_1800:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(2880, 1800));
			break;
		case menu_types::SIZE_3440_1440:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(3440, 1440));
			break;
		case menu_types::SIZE_3840_2160:
			ctx.m_settings.Set(&ctx.m_settings.menu_size, ImVec2(3840, 2160));
			break;
		default:
			break;
		}
	}
	UII::Controls::endSection();

	UII::Controls::nextSection();

	UII::Controls::beginSection("Movement", ImVec2(250, 223 - 5));
	{
	}
	UII::Controls::endSection();

	UII::Controls::sameSection();

	UII::Controls::beginSection("World", ImVec2(250, 223 - 5));
	{
	}
	UII::Controls::endSection();
}

void c_draw::DrawString(std::string text, ImVec2 pos, ImColor color)
{
	auto draw_list = ImGui::GetBackgroundDrawList();

	draw_list->AddText(pos, color, text.c_str());
}

void c_draw::DrawRect(ImVec2 min, ImVec2 max, float thickness, ImColor color)
{
	auto draw_list = ImGui::GetBackgroundDrawList();
	draw_list->AddRect(min, max, color, 0.0f, 0, thickness);
}

void c_draw::DrawFilledRect(ImVec2 min, ImVec2 max, ImColor color)
{
	auto draw_list = ImGui::GetBackgroundDrawList();
	draw_list->AddRectFilled(min, max, color, 0.0f, 0);
}

void c_draw::DrawLine(ImVec2 p1, ImVec2 p2, ImColor color, float thickness)
{
	auto draw_list = ImGui::GetBackgroundDrawList();
	draw_list->AddLine(p1, p2, color, thickness);
}

void c_draw::DrawCircle(ImVec2 center, float raduis, ImColor color, int segments, float thickness)
{
	auto draw_list = ImGui::GetBackgroundDrawList();
	draw_list->AddCircle(center, raduis, color, segments, thickness);
}

void c_draw::DrawFilledCircle(ImVec2 center, float raduis, ImColor color, int segments)
{
	auto draw_list = ImGui::GetBackgroundDrawList();
	draw_list->AddCircleFilled(center, raduis, color, segments);
}
