#include <iostream>
#include <thread>

#include "context.h"
#include "Config/Config.h"
#include "Fuser/Fuser.h"
#include <DMALibrary/Memory/Memory.h>

#include "imgui_internal.h"
#include "Cheat/MainThread.h"
#include "Utils/Mouse/kmbox.h"
#include "Vorpal/API/Vorpal.h"
#include "Vorpal/HWID/HWID.h"
#include "Cheat/API/API.h"
#include "Cheat/Cache/Cache.h"
#include "Cheat/Structs/math.h"
#include "Cheat/Features/Movement.h"
c_context ctx;

void DrawPlayers()
{
	/*static bool open = true;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ctx.m_settings.menu_size);
	ImGui::Begin("Players", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNav);

	if (ctx.m_settings.visuals_enable_esp)
	{
		g_Cache.FastCache();
		auto entitties = g_Cache.GetEntities();
		if (entitties.size() > 0)
		{
			auto drawList = ImGui::GetWindowDrawList();

			for (auto& entity : entitties)
			{
				Vector2 screen;
				if (WorldToScreen(entity.pos, screen, g_Cache.GetMatrix(), g_Cache.GetFov()) || true)
				{
					/*i
					if (entity.entity_type == 0)
					{
						drawList->AddText(ImVec2(screen.x, screen.y), ImColor(36, 209, 64, 255), xorstr_("Entity")); //Humanoid entities? Pretty much enemies and friendlies.
						//printf(xorstr_("[0x%p] Entity: %f %f %f\n"), entity.addr, entity.pos.x, entity.pos.y, entity.pos.z);
						if (ctx.m_settings.movement_noclip() && (GetAsyncKeyState(ctx.m_settings.movement_noclip_key()) || Keyboard::IsKeyDown(ctx.m_settings.movement_noclip_key())))
						{
							auto zthing = GetCameraDistanceFactor(entity.pos, g_Cache.GetMatrix());
							if (zthing < 1.f && zthing > -1.f) //0 means the coord and the camera are at the same spot, ofc that's not true but if it's under 1.f and above +1.f it sure as hell is our player (in first person only)
							{
								Features::Movement::HandleNoclip(entity);
							}
						}
					}
					/*else
					{
						std::string name = xorstr_("UNK");
						/*name += xorstr_(" (");
						name += std::to_string(entity.entity_type);
						name += xorstr_(")");#2#
						drawList->AddText(ImVec2(screen.x, screen.y), ImColor(255, 255, 255, 255), name.c_str());
					}#1#
					// 1058642330
					//else
					//	drawList->AddText(ImVec2(screen.x, screen.y), ImColor(255, 255, 255, 255), entity.type == EntityType::Player ? xorstr_("Player") : entity.type == EntityType::NPC ? xorstr_("NPC") : xorstr_("UNK"));
				}
			}
		}
	}
	ImGui::End();*/

	ctx.m_destiny_api->CanRefreshToken();
	ctx.m_draw->Run();
}

#include "Cheat/API/API.h"

#define DO_AUTH
//Just in case check in context.h #define I_HAVE_NO_DMA

#ifdef DO_AUTH
#endif
#define INIT_TIMER auto start = std::chrono::high_resolution_clock::now();
#define START_TIMER  start = std::chrono::high_resolution_clock::now();
#define STOP_TIMER(name)  std::cout << "RUNTIME of " << name << ": " << \
std::fixed << std::setprecision(2) << \
std::chrono::duration<double, std::chrono::milliseconds::period>( \
std::chrono::high_resolution_clock::now()-start \
).count() << " ms " << std::endl;

void get_data()
{
	ParseData(xorstr_("ITEM_URL_STUFF_HERE.json"), cheeto::items_data);
	ParseData(xorstr_("ITEM_URL_STUFF_HERE.json"), cheeto::perks_data);
	ParseData(xorstr_("ITEM_URL_STUFF_HERE.json"), cheeto::models_data);
}

int main()
{
#ifdef DO_AUTH
	char username[50];
	char password[50];
	auto currentPath = std::filesystem::temp_directory_path();
	if (std::filesystem::exists(currentPath.string() + xorstr_("\\login.txt")))
	{
		std::ifstream infile(currentPath.string() + xorstr_("\\login.txt"));

		std::string line;
		std::getline(infile, line);
		if (line.find(xorstr_(":")) != std::string::npos)
		{
			std::string User = line.substr(0, line.find(xorstr_(":")));
			std::string Pass = line.substr(line.find(xorstr_(":")) + 1);
			User.copy(username, User.size() + 1);
			Pass.copy(password, Pass.size() + 1);

			// null-terminate the char arrays
			username[User.size()] = '\0';
			password[Pass.size()] = '\0';
		}

		infile.close();
	}
	ctx.m_logger->Info(xorstr_("Username: %s\n"), username);
	ctx.m_logger->Info(xorstr_("Logging in...\n"));
	
#endif

	while (!mem.Init(xorstr_("Destiny2.exe"), true))
	{
		ctx.m_logger->Error(xorstr_("Failed to find \"\Destiny2.exe\"\ retrying in a second\n"));
	
		Sleep(1000);
	}
	
	ctx.m_logger->Succes(xorstr_("We in baby\n"));
	ctx.m_logger->Info(xorstr_("Initializing Cheat...\n"));
	std::thread mt(cheeto::main_thread);
	ctx.m_logger->Info(xorstr_("Loading Default Config\n"));
	if (!ctx.m_settings.exists(xorstr_("Default.conf")))
		ctx.m_settings.save(xorstr_("Default.conf"));
	ctx.m_settings.load(xorstr_("Default.conf"));
	ctx.m_logger->Succes(xorstr_("Loaded Default Config\n"));
	auto comport = kmbox::find_port(xorstr_("USB-SERIAL CH340"));
	ctx.m_logger->Info(xorstr_("Maybe This is your Comport: %s\n"), comport.c_str());
	
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		ctx.m_logger->Error("WSAStartup failed: %d\n", iResult);
	}
	
	if (!comport.empty())
	{
		ctx.m_settings.Set<std::string>(&ctx.m_settings.hardware_mouse_com_port, std::string(comport));
		kmbox::initialize(comport.c_str());
	}
	
	//ctx.m_settings.menu_
	fuser.Initialize(ctx.m_settings.menu_size);
	ctx.m_draw->Init(fuser); //Initialize font ect after we have initialized fuser.
	ctx.m_logger->Info(xorstr_("Loading Data...\n"));
	get_data();
	ctx.m_logger->Succes(xorstr_("Loaded Data...\n"));
	fuser.Run(&DrawPlayers);
	
	mt.join();
#ifdef DO_AUTH
	MessageBoxA(NULL, xorstr_("Invalid Authentication"), xorstr_("ERROR"), MB_OK);
#endif
	return 0;
}
