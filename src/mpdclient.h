#ifndef MPDCLIENT_H
#define MPDCLIENT_H

#include <mpd/client.h>

struct mpdclient {
    struct mpd_connection *connection;
    struct mpd_song *current_song;
    struct mpd_status *status;
    enum mpd_state state;
};

struct mpdclient *mpdclient_init(char *host, int port, int timeout);
void mpdclient_free(struct mpdclient *mpd);

void mpdclient_update(struct mpdclient *mpd);
const char *mpdclient_get_current_song_name(struct mpdclient *mpd);

#endif /* MPDCLIENT_H */
