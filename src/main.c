#include "mpdclient.h"
#include "discord.h"

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define MPD_DEFAULT_HOST "localhost"
#define MPD_DEFAULT_PORT 6600
#define MPD_DEFAULT_TIMEOUT 30000


volatile sig_atomic_t alarm_fired = 0;

int get_args(int argc, char **argv, char **mpd_host, int *mpd_port, int *mpd_timeout)
{
    int ch;
    opterr = 0;

    while ((ch = getopt(argc, argv, "h:p:t:")) != -1) {
        switch (ch) {
        case 'h':
            *mpd_host = optarg;
            break;
        case 'p':
            *mpd_port = atoi(optarg);
            if (*mpd_port == 0) {  /* Argument was non-numeric */
                fprintf(stderr, "Invalid port number: %s\n", optarg);
                return 1;
            }
            break;
        case 't':
            *mpd_timeout = atoi(optarg);
            if (*mpd_timeout == 0) {  /* Argument was non-numeric */
                fprintf(stderr, "Invalid timeout: %s\n", optarg);
                return 1;
            }
            break;
        case '?':
            if (optopt == 'h' || optopt == 'p' || optopt == 't')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option '-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort();
        }
    }
    return 0;
}

void ding(int signum)
{
    alarm_fired = 1;
}

int main(int argc, char **argv)
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = ding;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGINT, &action, NULL);


    int mpd_port = MPD_DEFAULT_PORT;
    int mpd_timeout = MPD_DEFAULT_TIMEOUT;
    char *mpd_host = MPD_DEFAULT_HOST;

    if (get_args(argc, argv, &mpd_host, &mpd_port, &mpd_timeout))
        return 1;

    struct mpdclient *mpd = mpdclient_init(mpd_host, mpd_port, mpd_timeout);
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
