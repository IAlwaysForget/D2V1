#pragma once
#include "hash.h"
#include "fileIO.h"
#include <vector>
#include <string>

#include "Draw.h"
#include "imgui.h"
#include "xorstr.hpp"
#include "Cheat/Structs/Inventory.h"

struct string_setting_t
{
	char text[128];
};

class node_t
{
public:
	virtual ~node_t() = default;

	virtual void add(node_t* node_ptr) = 0;

	virtual void load_setting(std::string& config_str) = 0;
	virtual void save_setting(std::string& config_str) = 0;
	//virtual bool set_value(std::string& feature) = 0;
};

class holder_t : public node_t
{
public:
	holder_t(const char* name) : m_name(name)
	{
	}

	holder_t(holder_t* holder_ptr, const char* name) : m_name(name) { holder_ptr->add(this); }

	void load_setting(std::string& node) override
	{
	}

	void save_setting(std::string& node) override
	{
	}

	//bool set_value(std::string& feature) override { return false; }

	void add(node_t* node_ptr) override;

	void load(const char* path, const char* file);

	void load(std::string& config);

	void save(std::string& config);

	void save(const char* path, const char* file);

	const char* m_name;
	std::vector<node_t*> m_nodes;
	std::string tempConfig;
};

class ISetting : public node_t
{
	void add(node_t* node) override
	{
	}
};

template <typename T>
class setting_t : ISetting
{
public:
	__forceinline setting_t(holder_t* holder_ptr, uint64_t name) : m_name(name) { holder_ptr->add(this); }

	__forceinline setting_t(holder_t* holder_ptr, uint64_t name, const T& rhs) : m_old_value(rhs), m_value(rhs), m_default(rhs), m_name(name) { holder_ptr->add(this); }

	bool changed()
	{
		uint8_t* data1 = reinterpret_cast<uint8_t*>(&m_value);
		uint8_t* data2 = reinterpret_cast<uint8_t*>(&m_old_value);

		for (size_t i = 0; i < sizeof(m_value); i++)
		{
			if (data1[i] != data2[i])
			{
				m_old_value = m_value;
				return true; // Objects are not equal
			}
		}
		return false;
	}

	void load_setting(std::string& config_str) override
	{
		m_value = m_default;

		uint8_t* data = (uint8_t*)&m_value;

		auto pos = config_str.find(std::to_string(m_name));
		if (pos == std::string::npos)
			return;

		pos += std::to_string(m_name).length() + 1;
		const char* buffer = config_str.data() + pos;
		for (size_t i = 0; i < sizeof(m_value); i++)
		{
			unsigned temp;
			sscanf(&buffer[2 * i], "%02X", &temp);
			data[i] = temp; //Set Value
		}
	}

	void save_setting(std::string& config_str) override
	{
		config_str.append(std::to_string(m_name) + xorstr_(":"));

		uint8_t* data = (uint8_t*)&m_value;

		char* buffer = new char[sizeof(m_value) * 2 + 1];
		for (size_t i = 0; i < sizeof(m_value); i++)
			sprintf(&buffer[2 * i], "%02X", data[i]);

		config_str.append(std::string(buffer) + xorstr_(";"));
		delete[] buffer;
	}

	setting_t& operator=(const T& rhs)
	{
		return *this;
	}

	__forceinline operator T&()
	{
		return m_value;
	}

	__forceinline operator const T&() const
	{
		return m_value;
	}

	__forceinline T* operator &()
	{
		return &m_value;
	}

	__forceinline const T* operator &() const
	{
		return &m_value;
	}

	__forceinline T& operator ()()
	{
		return m_value;
	}

	__forceinline const T& operator ()() const
	{
		return m_value;
	}

	bool operator!=(const T& rhs) const
	{
		return m_value != rhs;
	}

private:
	T m_value;
	T m_old_value;
	T m_default;
	uint64_t m_name;
};

template <typename t>
class settingtype_t
{
public:
	settingtype_t(const t& value = { })
	{
		for (size_t i = 0; i < 1; ++i)
			m_data[i] = value;
	}

	t m_data[1];

	t& operator( )()
	{
		return m_data[0];
	}

	t& operator[](const size_t& iter)
	{
		return m_data[iter];
	}
};

// fuck it!
// fr? lmfao
struct settingstring_t
{
	char buffer[128];

	settingstring_t()
	{
		clear();
	}

	settingstring_t(const char* text)
	{
		set(text);
	}

	__forceinline void set(std::string& str)
	{
		set(str.c_str());
	}

	__forceinline void set(const char* text)
	{
		clear();
		strncpy_s(buffer, 128, text, 128);
	}

	__forceinline std::string get() const
	{
		return buffer;
	}

	__forceinline bool empty() const
	{
		return buffer[0] == 0;
	}

	__forceinline void clear()
	{
		memset(buffer, 0, 128);
	}

	__forceinline const char* c_str()
	{
		return buffer;
	}
};

struct perky
{
	short id = -1;
};

struct armor_perks
{
	perky perks[69] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //33
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //44
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //55
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //66
		-1, -1, -1
	}; //69};
};

struct weps_perks
{
	int ID = -1;
	int Model = -1;
	perky perks[11] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
};

struct weps
{
	weps_perks kinetic = weps_perks();
	weps_perks energy = weps_perks();
	weps_perks heavy = weps_perks();
};

extern bool new_config;
extern bool config_two;

class c_settings
{
public:
	bool tooltips = false;
	holder_t m_holder {""};

	setting_t<bool> menu_menu_key {&m_holder, fnvc("menu_menu_key"), true};
	setting_t<int> menu_menu_key_hotkey {&m_holder, fnvc("menu_menu_key_hotkey"), VK_INSERT};
	setting_t<ImVec2> menu_size {&m_holder, fnvc("menu_size"), {1920.f, 1080.f}};
	setting_t<int> menu_size_type {&m_holder, fnvc("menu_size_type"), 9};

	/* Hardware mouse type */
	setting_t<int> hardware_mouse_type {&m_holder, fnvc("hardware_mouse_type"), 1};
	setting_t<int> hardware_kmbox_baudrate {&m_holder, fnvc("hardware_kmbox_baudrate"), 115200};
	setting_t<int> hardware_kmbox_delay {&m_holder, fnvc("hardware_kmbox_delay"), 10};
	setting_t<std::string> hardware_mouse_network_ip {&m_holder, fnvc("hardware_mouse_network_ip"), "192.168.178.102"};
	setting_t<std::string> hardware_mouse_com_port {&m_holder, fnvc("hardware_mouse_com_port"), "COM0"};

	setting_t<bool> visuals_enable_esp {&m_holder, fnvc("visuals_enable_esp"), true};

	/*Movement*/
	setting_t<bool> movement_noclip{ &m_holder, fnvc("movement_noclip"), false };
	setting_t<int> movement_noclip_key{ &m_holder, fnvc("movement_noclip_key"), VK_MENU };
	setting_t<int> movement_noclip_normal_speed{ &m_holder, fnvc("movement_noclip_fast_speed"), 1 };
	setting_t<int> movement_noclip_fast_speed{ &m_holder, fnvc("movement_noclip_fast_speed"), 5 };
	setting_t<int> movement_noclip_style{ &m_holder, fnvc("movement_noclip_style"), 1 };
	/*Inventory shit*/
	setting_t<int> inventory_weapon_type {&m_holder, fnvc("inventory_weapon_type"), 0};
	setting_t<int> inventory_current_selected_item {&m_holder, fnvc("inventory_current_selected_item"), 0};
	setting_t<std::string> inventory_current_selected_item_hash {&m_holder, fnvc("inventory_current_selected_item_hash"), ""};
	setting_t<int> inventory_current_selected_weapon {&m_holder, fnvc("inventory_current_selected_weapon"), -1};
	setting_t<bool> inventory_selected_armor {&m_holder, fnvc("inventory_selected_armor"), false};

	setting_t<weps> inventory_weapons {&m_holder, fnvc("inventory_weapons"), weps()};
	setting_t<armor_perks> inventory_armor {&m_holder, fnvc("inventory_weapons"), armor_perks()};

	setting_t<int> inventory_current_selected_perk {&m_holder, fnvc("inventory_current_selected_perk"), -1};
	setting_t<std::string> inventory_current_selected_perk_hash {&m_holder, fnvc("inventory_current_selected_perk_hash"), ""};

	/*Character Shit*/
	setting_t<int> character_stat_mob{ &m_holder, fnvc("character_stat_mob"), 10 };
	setting_t<int> character_stat_res{ &m_holder, fnvc("character_stat_res"), 10 };
	setting_t<int> character_stat_rec{ &m_holder, fnvc("character_stat_rec"), 10 };
	setting_t<int> character_stat_dis{ &m_holder, fnvc("character_stat_dis"), 10 };
	setting_t<int> character_stat_int{ &m_holder, fnvc("character_stat_int"), 10 };
	setting_t<int> character_stat_str{ &m_holder, fnvc("character_stat_str"), 10 };

	setting_t<int> character_skill_super{ &m_holder, fnvc("character_stat_super"), 0 };
	setting_t<int> character_skill_class_ability{ &m_holder, fnvc("character_stat_class_ability"), 0 };
	setting_t<int> character_skill_grenade{ &m_holder, fnvc("character_stat_grenade"), 0 };
	setting_t<int> character_skill_melee{ &m_holder, fnvc("character_stat_melee"), 0 };

	/*Miscellaneous settings*/
	setting_t<bool> allow_memory_writes {&m_holder, fnvc("allow_memory_writes"), false};
	setting_t<bool> vsync {&m_holder, fnvc("vsync"), false};

	//Misc
	setting_t<bool> aimbot_crosshair {&m_holder, fnvc("aimbot_crosshair"), false};

	void save(const char* name)
	{
		std::string path = std::getenv("USERPROFILE");
		path += "\\Documents";
		if (!DoesPathExist(path))
			CreateFolder(path);
		path += "\\Sentry";
		if (!DoesPathExist(path))
			CreateFolder(path);
		path += "\\Config";
		if (!DoesPathExist(path))
			CreateFolder(path);
		path += "\\Destiny";
		if (!DoesPathExist(path))
			CreateFolder(path);
		m_holder.save(path.c_str(), name);
	}

	bool exists(const char* name)
	{
		std::string path = std::getenv("USERPROFILE");
		path += "\\Documents";
		if (!DoesPathExist(path))
			return false;
		path += "\\Sentry";
		if (!DoesPathExist(path))
			return false;
		path += "\\Config";
		if (!DoesPathExist(path))
			return false;
		path += "\\Destiny";
		if (!DoesPathExist(path))
			return false;
		path += "\\" + std::string(name);
		if (std::filesystem::exists(path))
			return true;
		return false;
	}

	void load(const char* name)
	{
		std::string path = std::getenv("USERPROFILE");
		path += "\\Documents";
		if (!DoesPathExist(path))
			CreateFolder(path);
		path += "\\Sentry";
		if (!DoesPathExist(path))
			CreateFolder(path);
		path += "\\Config";
		if (!DoesPathExist(path))
			CreateFolder(path);
		path += "\\Destiny";
		if (!DoesPathExist(path))
			CreateFolder(path);

		m_holder.load(path.c_str(), name);
	}

	void _delete(const char* name)
	{
		std::string path = std::getenv("USERPROFILE");
		path += "\\Documents";
		if (!DoesPathExist(path))
			return;
		path += "\\Sentry";
		if (!DoesPathExist(path))
			return;
		path += "\\Config";
		if (!DoesPathExist(path))
			return;
		path += "\\Destiny";
		if (!DoesPathExist(path))
			CreateFolder(path);
		path += "\\" + std::string(name);
		if (std::filesystem::exists(path))
			std::filesystem::remove(path);
	}

	std::vector<std::string> get_all_configs()
	{
		std::vector<std::string> configs;
		std::string path = std::getenv("USERPROFILE");
		path += "\\Documents";
		if (!DoesPathExist(path))
			return configs;
		path += "\\Sentry";
		if (!DoesPathExist(path))
			return configs;
		path += "\\Config";
		if (!DoesPathExist(path))
			return configs;
		path += "\\Destiny";
		if (!DoesPathExist(path))
			return configs;
		//get all files in folder
		try
		{
			for (const auto& entry : std::filesystem::directory_iterator(path))
				if (std::filesystem::is_regular_file(entry.path()))
					configs.push_back(entry.path().filename().string());
		}
		catch (const std::exception& e)
		{
		}
		return configs;
	}

	bool has_config_changed()
	{
		if (new_config)
		{
			new_config = false;
			return true;
		}
		return false;
	}

	void save_conf(std::string& config) { m_holder.save(config); }
	void load_conf(std::string& config) { m_holder.load(config); }

	template <typename T>
	void Set(T* setting, T value)
	{
		*setting = value;
	}

	template <typename T>
	void Toggle(T* setting)
	{
		*setting = !*setting;
	}
};

extern c_settings settings;
