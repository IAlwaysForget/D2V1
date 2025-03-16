#include "API.h"
#include <curl/curl.h>
#include <iostream>
#include <string>

#include "Vorpal/nlohmann/json.hpp"
#include <random>
#include <thread>
#include "../Utils/Zip/zip.h"

#include "context.h"
#include "Cheat/MainThread.h"

c_destiny_api::c_destiny_api()
{
	curl_global_init(CURL_GLOBAL_ALL);
	clientID = "";
	ClientURL = "";
	ClientSecret = "";
	HTTPClientURL = "";
	APIKEY = "";
	AuthCode = "Auth code not found.";
}

c_destiny_api::~c_destiny_api()
{
	curl_global_cleanup();
}

std::string c_destiny_api::Generate16CharString()
{
	const std::string chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"0123456789";
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(0, chars.size() - 1);

	std::string random_string;
	for (std::size_t i = 0; i < 16; ++i)
	{
		random_string += chars[distribution(generator)];
	}

	return random_string;
}

size_t WriteCallback2(void* contents, size_t size, size_t nmemb, std::string* userp)
{
	userp->append((char*)contents, size * nmemb);
	return size * nmemb;
}

struct curl_slist* createHeaders(const std::map<std::string, std::string>& headers, bool type = 1)
{
	struct curl_slist* headerList = NULL;
	// Add custom headers from the map
	if (type == 1)
		headerList = curl_slist_append(headerList, xorstr_("Content-Type: application/x-www-form-urlencoded"));
	else
		headerList = curl_slist_append(headerList, xorstr_("Content-Type: application/json"));
	for (const auto& entry : headers)
	{
		std::string header = entry.first + ": " + entry.second;
		headerList = curl_slist_append(headerList, header.c_str());
	}

	return headerList;
}

bool c_destiny_api::PostRequest(const std::string& url, const std::map<std::string, std::string>& formData, std::string& response)
{
	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		std::string formattedData;
		for (const auto& entry : formData)
		{
			if (!formattedData.empty())
			{
				formattedData += xorstr_("&");
			}
			formattedData += entry.first + xorstr_("=") + entry.second;
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, formattedData.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res != CURLE_OK)
			ctx.m_logger->Error("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		curl_slist_free_all(headers);
		return res == CURLE_OK;
	}
	return false;
}

bool c_destiny_api::PostRequestJ(const std::string& url, json json, std::string& response, const std::map<std::string, std::string>& header)
{
	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		std::string body = json.dump();

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		struct curl_slist* headers = createHeaders(header, 0);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res != CURLE_OK)
			ctx.m_logger->Error("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		return res == CURLE_OK;
	}
	return false;
}

bool c_destiny_api::GetRequest(const std::string& url, std::string& response, const std::map<std::string, std::string>& header)
{
	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		struct curl_slist* headers = createHeaders(header);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res != CURLE_OK)
			ctx.m_logger->Error("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		return res == CURLE_OK;
	}
	return false;
}

std::string c_destiny_api::PollAuthCode(const std::string& GeneratedCode)
{
	std::string response = "";
	if (!GetRequest(HTTPClientURL + "/getAuthCode?uniqueID=" + GeneratedCode, response))
		return "";
	AuthCode = response;
	if (AuthCode != "Auth code not found.")
		return AuthCode;

	while (true)
	{
		response = "";
		if (!GetRequest(HTTPClientURL + "/getAuthCode?uniqueID=" + GeneratedCode, response))
			continue;
		AuthCode = response;
		if (AuthCode != "Auth code not found.")
			return AuthCode;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return AuthCode;
}

std::vector<MemberShipInfo> c_destiny_api::GetMembershipProfiles()
{
	std::vector<MemberShipInfo> result = { };

	std::string response = "";
	if (!GetRequest("https://www.bungie.net/Platform/Destiny2/-1/Profile/" + bungie_membership_id + "/LinkedProfiles/", response,
	                {
		                {xorstr_("X-API-Key"), APIKEY},
		                {xorstr_("Authorization"), "Bearer " + access_token}
	                }))
		return result;

	if (response.empty())
		return result;

	json j = json::parse(response);

	if (!j.contains("Response"))
		return result;
	if (!j["Response"].contains("profiles"))
		return result;
	if (!j["Response"]["profiles"].is_array())
		return result;

	for (const auto& profile : j["Response"]["profiles"])
	{
		MemberShipInfo info = { };
		info.membershipType = profile["membershipType"].get<int>();
		info.membershipId = profile["membershipId"];
		info.displayName = profile["displayName"];
		info.bungieGlobalDisplayName = profile["bungieGlobalDisplayName"];
		info.bungieGlobalDisplayNameCode = std::to_string(profile["bungieGlobalDisplayNameCode"].get<int>());
		result.push_back(info);
	}

	return result;
}

bool c_destiny_api::GetTokens()
{
	std::string response = "";
	if (!PostRequest("https://www.bungie.net/platform/app/oauth/token/", {
		                 {"grant_type", "authorization_code"},
		                 {"code", AuthCode},
		                 {"client_id", clientID},
		                 {"client_secret", ClientSecret}
	                 }, response))
		return false;

	if (response.empty())
		return false;

	json j = json::parse(response);
	if (j.contains("refresh_token"))
		refresh_token = j["refresh_token"];
	if (j.contains("access_token"))
		access_token = j["access_token"];
	if (j.contains("membership_id"))
		bungie_membership_id = j["membership_id"];

	return true;
}

bool c_destiny_api::RefreshToken()
{
	json result;
	std::string response = "";
	if (!PostRequest("https://www.bungie.net/platform/app/oauth/token/", {
		                 {"grant_type", "refresh_token"},
		                 {"refresh_token", refresh_token},
		                 {"client_id", clientID},
		                 {"client_secret", ClientSecret}
	                 }, response))
		return false;

	if (response.empty())
		return false;

	json j = json::parse(response);
	if (j.contains("refresh_token"))
		refresh_token = j["refresh_token"];
	if (j.contains("access_token"))
		access_token = j["access_token"];
	if (j.contains("membership_id"))
		bungie_membership_id = j["membership_id"];

	return true;
}

std::pair<bool, std::string> c_destiny_api::GetGhostInfo(const std::string& character_membership_id, const std::string& characterId)
{
	std::pair<bool, std::string> result = {false, ""};
	if (!has_profiles())
		return result;

	std::string response = "";
	if (!GetRequest("https://www.bungie.net/Platform/Destiny2/" + std::to_string(profiles[current_selected_profile].membershipType) + "/Profile/" + character_membership_id + "/Character/" + characterId + "/?components=205", response,
	                {
		                {xorstr_("X-API-Key"), APIKEY},
		                {xorstr_("Authorization"), "Bearer " + access_token}
	                }))
		return result;

	auto json_res = json::parse(response);
	if (!json_res.contains("Response"))
		return result;
	if (!json_res["Response"].contains("equipment"))
		return result;
	if (!json_res["Response"]["equipment"].contains("data"))
		return result;
	if (!json_res["Response"]["equipment"]["data"].contains("items"))
		return result;

	auto equipmentData = json_res["Response"]["equipment"]["data"]["items"];
	for (const auto& item : equipmentData)
	{
		if (!item.contains("bucketHash"))
			continue;
		if (item["bucketHash"] == 4023194814) // Ghost shell
		{
			if (!item.contains("state") || !item.contains("itemInstanceId"))
				continue;

			int lockState = item["state"];
			std::string itemHash = item["itemInstanceId"];
			result = {lockState == 1 ? false : true, itemHash};
			break;
		}
	}

	return result;
}

std::vector<CharachterInfo> c_destiny_api::FetchAndStorePlayerCharacters(const std::string& character_membership_id)
{
	std::vector<CharachterInfo> result;
	std::string response = "";
	if (!has_profiles())
		return result;

	if (!GetRequest("https://www.bungie.net/Platform/Destiny2/" + std::to_string(profiles[current_selected_profile].membershipType) + "/Profile/" + character_membership_id + "/?components=200", response,
	                {
		                {xorstr_("X-API-Key"), APIKEY},
		                {xorstr_("Authorization"), "Bearer " + access_token}
	                }))
		return result;

	auto json_res = json::parse(response);
	if (!json_res.contains("Response"))
		return result;
	if (!json_res["Response"].contains("characters"))
		return result;
	if (!json_res["Response"]["characters"].contains("data"))
		return result;

	auto characterData = json_res["Response"]["characters"]["data"];
	for (auto& [characterId, characterInfo] : characterData.items())
	{
		CharachterInfo info;

		if (!characterInfo.contains("classType") || !characterInfo.contains("emblemPath") || !characterInfo.contains("emblemBackgroundPath"))
			continue;

		int classType = characterInfo["classType"];
		std::string emblemd = characterInfo["emblemPath"];
		std::string emblembackground = characterInfo["emblemBackgroundPath"];
		std::string className = (classType == 0) ? "Titan" : (classType == 1) ? "Hunter" : (classType == 2) ? "Warlock" : "Unknown";

		info.characterId = characterId;
		info.className = className;
		info.emblemdPath = emblemd;
		info.emblembackgroundPath = emblembackground;

		result.push_back(info);
	}
	return result;
}

bool c_destiny_api::UnlockOrLockGhost(MemberShipInfo MemebershipData, CharachterInfo PlayerData)
{
	auto [state, itemHash] = GetGhostInfo(MemebershipData.membershipId, PlayerData.characterId);

	json body;
	std::string response = "";
	body["state"] = state;
	body["itemId"] = itemHash;
	body["characterId"] = PlayerData.characterId;
	body["membershipType"] = profiles[current_selected_profile].membershipType;
	if (!PostRequestJ("https://www.bungie.net/Platform/Destiny2/Actions/Items/SetLockState/", body, response, {
		                  {xorstr_("X-API-Key"), APIKEY},
		                  {xorstr_("Authorization"), "Bearer " + access_token}
	                  }))
		return false;

	return true;
}

void c_destiny_api::Init()
{
	std::string GeneratedCode = this->Generate16CharString();
	std::string AuthURL = "https://www.bungie.net/en/oauth/authorize?client_id=" + clientID + "&response_type=code&state=" + GeneratedCode + "&redirect_uri=" + ClientURL + "callback";
	std::wstring wurl(AuthURL.begin(), AuthURL.end());
	ShellExecute(0, 0, wurl.c_str(), 0, 0, SW_SHOW);

	AuthCode = PollAuthCode(GeneratedCode); // waits for auth.

	if (!GetTokens())
		return; // Done once for access token

	profiles = GetMembershipProfiles();
	if (profiles.empty())
		return;
	characters = FetchAndStorePlayerCharacters(profiles[current_selected_profile].membershipId);

	/*for (const auto& profile : profiles)
	{
		std::string profile_membership_id = profile.membershipId; // fo this current profile
		auto PlayerData = FetchAndStorePlayerCharacters(profile_membership_id);
		for (size_t i = 0; i < PlayerData.size(); i++)
		{

		}

		bool Success = UnlockOrLockGhost(PlayerData); // Actually lock or unlock the ghost. <<<<<<<<<<<<<<<<<<<<<<<
		if (Success) printf("Success!\n");
		else printf("Failed!\n");
		
	}*/

	/*if (profiles.is_array() && !profiles.empty())
	{
		for (const auto& profile : profiles)
		{
			std::string profile_membership_id = profile["membershipId"]; // fo this current profile
			auto PlayerData = FetchAndStorePlayerCharacters(profile_membership_id, Login.access_token);
			Destiny_Character_Name = profile["bungieGlobalDisplayName"];
			Destiny_Character_ID = std::to_string(profile["bungieGlobalDisplayNameCode"].get<int>());
			break;
		}
	}*/
}

std::chrono::steady_clock::time_point last_refresh = std::chrono::steady_clock::now();
//Refreshes the token every 30 minutes
void c_destiny_api::CanRefreshToken()
{
	if (!access_token.empty())
	{
		if (last_refresh + std::chrono::minutes(30) < std::chrono::steady_clock::now())
		{
			RefreshToken();
			last_refresh = std::chrono::steady_clock::now();
		}
	}
}

bool c_destiny_api::IsConnected()
{
	return !access_token.empty();
}

/*void ApplyChanges()
{
	json profiles = GetMembershipProfiles();

	if (profiles.is_array() && !profiles.empty())
	{
		for (const auto& profile : profiles)
		{
			std::string profile_membership_id = profile["membershipId"]; // fo this current profile
			auto PlayerData = FetchAndStorePlayerCharacters(profile_membership_id, access_token);
			bool Success = UnlockOrLockGhost(access_token, PlayerData); // Actually lock or unlock the ghost. <<<<<<<<<<<<<<<<<<<<<<<
			if (Success) printf("Success!\n");
			else printf("Failed!\n");
		}
	}
	else
	{
		std::cout << "No profiles found" << std::endl;
	}
}*/

std::vector<uint8_t> LoadDataFromZip(std::string dir_path, std::string hash)
{
	std::vector<uint8_t> bffr;
	void* buf = NULL;
	size_t bufsize;
	if (!std::filesystem::exists(dir_path))
	{
		ctx.m_logger->Error(xorstr_("Error Finding assets.piss\n"));
		return bffr; // handle error
	}

	struct zip_t* zip = zip_open(dir_path.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
	if (zip == NULL)
	{
		ctx.m_logger->Error(xorstr_("Error opening zip file\n"));
		return bffr; // handle error
	}

	int err = zip_entry_open(zip, (hash + ".jpg").c_str());
	if (err != 0)
	{
		ctx.m_logger->Error(xorstr_("Error opening zip entry: %i\n"), err);
		zip_close(zip);
		return bffr; // handle error
	}

	err = zip_entry_read(zip, &buf, &bufsize);
	if (err < 0)
	{
		ctx.m_logger->Error(xorstr_("Failed to read zip entry: %i\n"), err);
		return bffr;
	}

	bffr = std::vector<uint8_t>((uint8_t*)buf, (uint8_t*)buf + bufsize);

	zip_entry_close(zip);
	zip_close(zip);

	return bffr;
}

void ParseData(const std::string& url, std::unordered_map<std::string, Items::ui_items>& result)
{
	CURL* curl = curl_easy_init();
	if (!curl)
	{
		ctx.m_logger->Error("Failed to initialize CURL.\n");
		return;
	}

	std::string response = "";
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	auto res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ctx.m_logger->Error("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		return;
	}

	json j = json::parse(response);

	std::string path = std::getenv("USERPROFILE") + std::string("\\Documents\\Sentry\\Destiny2");
	if (!std::filesystem::exists(path))
		std::filesystem::create_directories(path);

	for (auto& [key, value] : j.items())
	{
		Items::ui_items item_data;
		item_data.item_hash = key;

		if (url.find("CleanItems") != std::string::npos)
		{
			if (value.contains("name"))
			{
				item_data.name = value["name"].get<std::string>();
			}

			if (value.contains("icon"))
			{
				std::string dw_path = path + "\\" + item_data.item_hash + ".jpg";
				if (!std::filesystem::exists(dw_path))
				{
					std::string url = value["icon"];
					//cache & load icon
					URLDownloadToFileA(NULL, url.c_str(), dw_path.c_str(), 0, NULL);
				}
				item_data.icon_path = dw_path;
				item_data.icon_texture = 0;
			}
			if (value.contains("itemTypeDisplayName"))
				item_data.description = value["itemTypeDisplayName"].get<std::string>();
			/*if (value.contains("tierTypeName"))
				item_data.rarityName = value["tierTypeName"].get<std::string>();
			if (value.contains("tierType"))
				item_data.rarity = value["tierType"].get<int>();*/
			if (value.contains("realIndex"))
				item_data.item_id = value["realIndex"].get<int>();
		}
		else if (url.find("CleanPerks") != std::string::npos)
		{
			if (value.contains("name"))
			{
				item_data.name = value["name"].get<std::string>();
				if (item_data.name.empty())
				{
					item_data.name = "Unidentified (" + key + ")";
				}
			}

			if (value.contains("icon"))
			{
				std::string dw_path = path + "\\" + item_data.item_hash + ".jpg";
				if (!std::filesystem::exists(dw_path))
				{
					std::string url = value["icon"];
					//cache & load icon
					URLDownloadToFileA(NULL, url.c_str(), dw_path.c_str(), 0, NULL);
				}
				item_data.icon_path = dw_path;
				item_data.icon_texture = 0;
			}
			if (value.contains("description"))
				item_data.description = value["description"].get<std::string>();
			if (value.contains("index"))
				item_data.item_id = value["index"].get<short>();
		}
		else if (url.find("CleanModels") != std::string::npos)
		{
			//Check if key exists in model
			if (cheeto::items_data.contains(key))
			{
				if (value.contains("index"))
					cheeto::items_data[key].model_id = value["index"].get<short>();
			}
		}

		result[item_data.item_hash] = item_data;
	}

	curl_easy_cleanup(curl);
}
