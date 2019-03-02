#include "mpdclient.h"
#include "discord.h"
#include <stdio.h>
#include <unistd.h>

#define MPD_DEFAULT_HOST "localhost"
#define MPD_DEFAULT_PORT 6600
#define MPD_DEFAULT_TIMEOUT 30000

int main()
{
    struct mpdclient *mpd = mpdclient_init(MPD_DEFAULT_HOST, MPD_DEFAULT_PORT,
                                           MPD_DEFAULT_TIMEOUT);

    if (!mpd) {
        printf("Unable to connect to MPD. Exiting...\n");
        mpdclient_free(mpd);
        return 1;
    }

    discord_init();
    update_presence();
    sleep(6);
    Discord_Shutdown();

    mpdclient_free(mpd);
    return 0;
}
