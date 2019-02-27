#include "mpdclient.h"
#include <stdlib.h>

struct mpdclient *mpdclient_init(char *host, int port, int timeout)
{
    struct mpdclient *mpd = malloc(sizeof(*mpd));
    mpd->connection = mpd_connection_new(host, port, timeout);

    enum mpd_error result = mpd_connection_get_error(mpd->connection);
    if (result == MPD_ERROR_SUCCESS) { /* Successfully connected to MPD */
        mpd->current_song = mpd_run_current_song(mpd->connection);
        mpd->status = mpd_run_status(mpd->connection);
        mpd->state = mpd_status_get_state(mpd->status);
    }
    else {
        mpd_connection_free(mpd->connection);
        free(mpd);
        mpd = NULL;
    }

    return mpd;
}

void mpdclient_free(struct mpdclient *mpd)
{
    mpd_connection_free(mpd->connection);
    mpd_song_free(mpd->current_song);
    mpd_status_free(mpd->status);
    free(mpd);
}
