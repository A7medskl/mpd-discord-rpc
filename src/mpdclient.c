#include "mpdclient.h"
#include <stdlib.h>
#include <signal.h>

extern sig_atomic_t alarm_fired;

struct mpdclient *mpdclient_init(char *host, int port, int timeout)
{
    struct mpdclient *mpd = malloc(sizeof(*mpd));
    mpd->connection = mpd_connection_new(host, port, timeout);

    enum mpd_error result = mpd_connection_get_error(mpd->connection);
    if (result == MPD_ERROR_SUCCESS) {
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

void mpdclient_update(struct mpdclient *mpd)
{
    mpd->current_song = mpd_run_current_song(mpd->connection);
    mpd->status = mpd_run_status(mpd->connection);
    mpd->state = mpd_status_get_state(mpd->status);
}

void mpdclient_wait_for_state_change(struct mpdclient *mpd)
{
    while (!alarm_fired) {
        mpd_send_idle_mask(mpd->connection, MPD_IDLE_PLAYER);
        mpd_send_noidle(mpd->connection);
        if (mpd_recv_idle(mpd->connection, 0) == 0)
            continue;

        return;
    }
}

int mpdclient_is_playing(struct mpdclient *mpd)
{
    return mpd->state == MPD_STATE_PLAY || mpd->state == MPD_STATE_PAUSE;
}

const char *mpdclient_get_current_song_name(struct mpdclient *mpd)
{
    if (mpdclient_is_playing(mpd))
        return mpd_song_get_tag(mpd->current_song, MPD_TAG_TITLE, 0);
    else
        return NULL;
}

const char *mpdclient_get_current_song_artist(struct mpdclient *mpd)
{
    if (mpdclient_is_playing(mpd))
        return mpd_song_get_tag(mpd->current_song, MPD_TAG_ARTIST, 0);
    else
        return NULL;
}
