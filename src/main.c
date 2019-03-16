#include "mpdclient.h"
#include "discord.h"

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>

#define MPD_DEFAULT_HOST "localhost"
#define MPD_DEFAULT_PORT 6600
#define MPD_DEFAULT_TIMEOUT 30000


volatile sig_atomic_t alarm_fired = 0;

void ding(int signum)
{
    alarm_fired = 1;
}

void setup_sighandlers()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = ding;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGINT, &action, NULL);
}

void print_usage()
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  mpd_discord_rpc [OPTION...]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "MPD Rich Presence for Discord - Display your music in Discord.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --host       MPD host to connect to\n");
    fprintf(stderr, "  -p, --port       MPD port to connect to\n");
    fprintf(stderr, "  -t, --timeout    timeout for MPD\n");
    fprintf(stderr, "  --help           show help options\n");
}

int get_args(int argc, char **argv, char **mpd_host, int *mpd_port, int *mpd_timeout)
{
    int ch;
    static int help_flag;

    while (1) {
        static struct option long_options[] =
            {
                {"host",    required_argument, 0, 'h'},
                {"port",    required_argument, 0, 'p'},
                {"timeout", required_argument, 0, 't'},
                {"help",    no_argument, &help_flag, 1},
                {0, 0, 0, 0}
            };

        int option_index = 0;
        ch = getopt_long(argc, argv, "h:p:t:", long_options, &option_index);

        if (help_flag) {
            print_usage();
            exit(0);
        }

        /* Detect the end of the options. */
        if (ch == -1)
            break;

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
            /* getopt_long will have already printed an error message. */
            return 1;
            break;
        default:
            print_usage();
            abort();
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    setup_sighandlers();

    char *mpd_host = MPD_DEFAULT_HOST;
    int mpd_port = MPD_DEFAULT_PORT;
    int mpd_timeout = MPD_DEFAULT_TIMEOUT;

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
