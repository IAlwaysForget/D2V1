#include "include/discord_rpc.h"
#include "include/discord_register.h"


namespace Discord {
    extern const char* username;
    extern const char* discrim;
    extern const char* userId;
    extern const char* avatarHash;
    extern bool grabbed;
    extern void handleDiscordReady(const DiscordUser* request);


    extern void Init();
}