#include "discord.h"
#include <Windows.h>
#include <stdio.h>
#include "../context.h"

namespace Discord
{
	const char* username;
	const char* discrim;
	const char* userId;
	const char* avatarHash;
	bool grabbed;

	void handleDiscordReady(const DiscordUser* request)
	{
		userId = request->userId;
		username = request->username;
		discrim = request->discriminator;
		avatarHash = request->avatar;

		grabbed = true;
		printf("\nDiscord: ready\n");
	}

	void Init()
	{
		//DiscordEventHandlers handlers;
		//memset(&handlers, 0, sizeof(handlers));
		//handlers.ready = handleDiscordReady;

		// Discord_Initialize(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId)
		// Discord_Initialize("310270644849737729", &handlers, 1, NULL);
		// Discord_RunCallbacks();
	}
}
