#pragma once

namespace Features::Character {
	struct Stats {
		int mobility = 10;
		int resilience = 10;
		int recovery = 10;
		int discipline = 10;
		int intellect= 10;
		int strength=10;
	};

	struct Skills {
		int grenade;
		int melee;
		int classability;
		int super;
	};


	constexpr int max_stat_limit = 385;
	extern bool applied_stats; //Indicates whether stats are being applied
	bool SetStats(Stats stats, bool limiter);

	extern bool applied_skills;
	void SetSkills(Skills skills);

	void ReadSkills();

	void Run();
}