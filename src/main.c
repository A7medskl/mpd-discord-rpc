#include "mpdclient.h"
#include "discord.h"

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

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
        return 1;
    }

    discord_init();

    char *song_name;
    char *artist_name;
    int send_presence;

    while (!alarm_fired) {
        mpdclient_update(mpd);

        song_name = mpdclient_get_current_song_name(mpd);
        artist_name = mpdclient_get_current_song_artist(mpd);
        send_presence = mpdclient_is_playing(mpd);

        discord_update_song_info(song_name, artist_name, send_presence);
        mpdclient_wait_for_state_change(mpd);
    }

    printf("Inturrupt signal detected. Closing connections...\n");
    Discord_Shutdown();
    printf("Disconnected from Discord.\n");
    mpdclient_free(mpd);
    printf("Disconnected from MPD.\n");

    return 0;
}
