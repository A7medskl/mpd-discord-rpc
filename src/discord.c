#include "discord.h"
#include <string.h>

#define DISCORD_APPLICATION_ID "549019770280869901"

static int SendPresence = 1;

void discord_init()
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = NULL;
    handlers.errored = NULL;
    handlers.disconnected = NULL;
    handlers.joinGame = NULL;
    handlers.spectateGame = NULL;
    handlers.joinRequest = NULL;

    Discord_Initialize(DISCORD_APPLICATION_ID, &handlers, 1, 0);
}

void update_presence()
{
    if (SendPresence) {
        DiscordRichPresence discordPresence;
        memset(&discordPresence, 0, sizeof(discordPresence));
        discordPresence.details = "Track Name";
        discordPresence.state = "By Artist";
        discordPresence.instance = 0;
        Discord_UpdatePresence(&discordPresence);
    }
    else {
        Discord_ClearPresence();
    }
}
