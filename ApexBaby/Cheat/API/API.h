#pragma once
#include <string>
#include "Vorpal/nlohmann/json.hpp"
#include <random>

#include "GUI.h"

using json = nlohmann::json;

typedef struct MemberShipInfo
{
	std::string bungieGlobalDisplayName = "";
	std::string bungieGlobalDisplayNameCode = "";
	std::string displayName = "";
	std::string membershipId = "";
	int membershipType = 0;
};

typedef struct CharachterInfo
{
	std::string emblemdPath = "";
	std::string emblembackgroundPath = "";
	std::string characterId = "";
	std::string className = "";
	std::string item_hash = "";
	int item_state = 0;
};

class c_destiny_api
{
private:
	std::string Generate16CharString();
	bool PostRequest(const std::string& url, const std::map<std::string, std::string>& formData, std::string& response);
	bool PostRequestJ(const std::string& url, json json, std::string& response, const std::map<std::string, std::string>& header);
	bool GetRequest(const std::string& url, std::string& response, const std::map<std::string, std::string>& header = { });

	std::string PollAuthCode(const std::string& GeneratedCode);

	std::string refresh_token = "";
	std::string access_token = "";
	std::string bungie_membership_id = "";

	//assigned in the constructor
	std::string clientID = "";
	std::string ClientURL = "";
	std::string ClientSecret = "";
	std::string HTTPClientURL = "";
	std::string APIKEY = "";
	std::string AuthCode = "";

	int membership_type = 0;

	std::vector<MemberShipInfo> profiles;
	//This is the currents profile characters.
	std::vector<CharachterInfo> characters;

public:
	int current_selected_profile = 0;
	int current_selected_character = 0;

	c_destiny_api();
	~c_destiny_api();

	std::pair<bool, std::string> GetGhostInfo(const std::string& membership_id, const std::string& characterId);
	std::vector<CharachterInfo> FetchAndStorePlayerCharacters(const std::string& membership_id);
	std::vector<MemberShipInfo> GetMembershipProfiles();
	bool UnlockOrLockGhost(MemberShipInfo MemebershipData, CharachterInfo PlayerData);

	bool RefreshToken();
	bool GetTokens();

	void CanRefreshToken();
	bool IsConnected();

	void Init();

	std::vector<MemberShipInfo> GetProfiles() { return profiles; };
	//this returns the currents profile character ( The profile that's currently selected)
	std::vector<CharachterInfo> GetCharacters() { return characters; };

	bool has_profiles() { return profiles.size() > 0; };
	bool has_characters() { return characters.size() > 0; };

	MemberShipInfo GetProfile()
	{
		MemberShipInfo ret;
		if (profiles.size() == 0 || profiles.empty())
			return ret;
		return profiles[current_selected_profile];
	};

	CharachterInfo GetCharacter()
	{
		CharachterInfo ret;
		if (characters.size() == 0 || characters.empty())
			return ret;
		return characters[current_selected_character];
	};
};

extern void ParseData(const std::string& url, std::unordered_map<std::string, Items::ui_items>& result);
