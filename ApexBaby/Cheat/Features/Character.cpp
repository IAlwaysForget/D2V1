#include "Character.h"
#include "context.h"
#include <DMALibrary/Memory/Memory.h>
#include "Cheat/API/API.h"
#include "Cheat/Cache/Cache.h"
#include "Cheat/Structs/math.h"
#include "Cheat/MainThread.h"

namespace Features::Character {
	Stats set_stats;

	bool applied_stats = false;
	auto whentostop = std::chrono::steady_clock::now();

	Skills set_skills;

	bool applied_skills = false;
	auto whentostop_skills = std::chrono::steady_clock::now();

	bool SetStats(Stats stats, bool limiter) {

		stats.discipline = std::clamp(stats.discipline, 0, 100);
		stats.intellect = std::clamp(stats.intellect, 0, 100);
		stats.mobility = std::clamp(stats.mobility, 0, 100);
		stats.recovery = std::clamp(stats.recovery, 0, 100);
		stats.resilience = std::clamp(stats.resilience, 0, 100);
		stats.strength = std::clamp(stats.strength, 0, 100);

		
		auto total = stats.discipline + stats.intellect + stats.mobility + stats.recovery + stats.resilience + stats.strength;

		if (total > max_stat_limit && limiter) {
			return false;
		}

		set_stats = stats;
		applied_stats = true;
		whentostop = std::chrono::steady_clock::now() + std::chrono::seconds(15);
		return true;
	}

	bool SetStats_OnEachFrame(VMMDLL_SCATTER_HANDLE &writehandle) {
		bool hasanywrites = true; //There's no conditions where we won't ever write?
		if (applied_stats) {
			mem.AddScatterWriteRequest(writehandle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Mobility), &set_stats.mobility, sizeof(set_stats.mobility));
			mem.AddScatterWriteRequest(writehandle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Resilience), &set_stats.resilience, sizeof(set_stats.resilience));
			mem.AddScatterWriteRequest(writehandle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Recovery), &set_stats.recovery, sizeof(set_stats.recovery));
			mem.AddScatterWriteRequest(writehandle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Discipline), &set_stats.discipline, sizeof(set_stats.discipline));
			mem.AddScatterWriteRequest(writehandle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Intellect), &set_stats.intellect, sizeof(set_stats.intellect));
			mem.AddScatterWriteRequest(writehandle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Strength), &set_stats.strength, sizeof(set_stats.strength));
			

			if (std::chrono::steady_clock::now() > whentostop) {
				applied_stats = false;
			}
		}
		return hasanywrites;
	}

	void SetSkills(Skills skills) {
		set_skills = skills;
		applied_skills = true;
		whentostop_skills = std::chrono::steady_clock::now() + std::chrono::seconds(15);
	}

	bool SetSkills_OnEachFrame(VMMDLL_SCATTER_HANDLE& writehandle) {
		bool hasanywrites = false;
		if (applied_skills) {
			
			if (set_skills.classability != -1 && (set_skills.classability > 0)) {
				hasanywrites = true;
				mem.AddScatterWriteRequest(writehandle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->ClassAbility), &set_skills.classability, sizeof(set_skills.classability));
			}
			if (set_skills.grenade != -1 && (set_skills.grenade > 0)) {
				hasanywrites = true;
				mem.AddScatterWriteRequest(writehandle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Grenade), &set_skills.grenade, sizeof(set_skills.grenade));
			}
			if (set_skills.melee != -1 && (set_skills.melee > 0)) {
				hasanywrites = true;
				mem.AddScatterWriteRequest(writehandle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Melee), &set_skills.melee, sizeof(set_skills.melee));
			}
			if (set_skills.super != -1 && (set_skills.super > 0)) {
				hasanywrites = true;
				mem.AddScatterWriteRequest(writehandle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Super), &set_skills.super, sizeof(set_skills.super));
			}

			if (std::chrono::steady_clock::now() > whentostop_skills) {
				applied_skills = false;
			}
			
		}
		return hasanywrites;
	}

	void ReadSkills() {
		auto handle = mem.CreateScatterHandle();
		short ca;
		mem.AddScatterReadRequest(handle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->ClassAbility), &ca, sizeof(ca));
		short grenade;
		mem.AddScatterReadRequest(handle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Grenade), &grenade, sizeof(grenade));
		short super;
		mem.AddScatterReadRequest(handle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Super), &super, sizeof(super));
		short melee;
		mem.AddScatterReadRequest(handle, reinterpret_cast<uintptr_t>(&cheeto::writeInventory->Melee), &melee, sizeof(melee));
		mem.ExecuteReadScatter(handle);
		ctx.m_settings.character_skill_class_ability() = ca;
		ctx.m_settings.character_skill_grenade() = grenade;
		ctx.m_settings.character_skill_melee() = melee;
		ctx.m_settings.character_skill_super() = super;

		mem.CloseScatterHandle(handle);
	}

	void Run() {
		auto writehandle = mem.CreateScatterHandle();
		auto st = SetStats_OnEachFrame(writehandle);
		auto sk = SetSkills_OnEachFrame(writehandle);
		if (sk || st) {
			mem.ExecuteWriteScatter(writehandle);
		}
		mem.CloseScatterHandle(writehandle);
	}
}