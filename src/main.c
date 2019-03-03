#include "mpdclient.h"
#include "discord.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MPD_DEFAULT_HOST "localhost"
#define MPD_DEFAULT_PORT 6600
#define MPD_DEFAULT_TIMEOUT 30000

volatile sig_atomic_t alarm_fired = 0;

void ding(int signum)
{
    alarm_fired = 1;
}

int main()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = ding;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    struct mpdclient *mpd = mpdclient_init(MPD_DEFAULT_HOST, MPD_DEFAULT_PORT,
                                           MPD_DEFAULT_TIMEOUT);

    if (!mpd) {
        printf("Unable to connect to MPD. Exiting...\n");
        mpdclient_free(mpd);
        return 1;
    }

    discord_init();
    update_presence();

    char *song_name;
    while (!alarm_fired) {
        mpdclient_update(mpd);
        song_name = mpdclient_get_current_song_name(mpd);
        if (song_name)
            printf("%s\n", song_name);
        else
            printf("Nothing is playing\n");
        sleep(5);
    }

    printf("Inturrupt signal detected. Closing connections...\n");
    Discord_Shutdown();
    printf("Disconnected from Discord.\n");
    mpdclient_free(mpd);
    printf("Disconnected from MPD.\n");

    return 0;
}
