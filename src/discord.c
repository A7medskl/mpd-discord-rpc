#include "discord.h"
#include <string.h>
#include <stdio.h>

#define DISCORD_APPLICATION_ID "549019770280869901"

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

void discord_update_song_info(char *title, char *artist, int send_presence)
{
    if (send_presence) {
        char state_buffer[256];
        sprintf(state_buffer, "by %s", artist);

        DiscordRichPresence discord_presence;
        memset(&discord_presence, 0, sizeof(discord_presence));
        discord_presence.details = title;
        discord_presence.state = state_buffer;
        discord_presence.instance = 0;
        Discord_UpdatePresence(&discord_presence);
    }
    else
        Discord_ClearPresence();
}
