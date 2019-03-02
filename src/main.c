#include "mpdclient.h"
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

    for (int i = 0; i < 5; ++i) {
        sleep(1);
        mpdclient_update(mpd);
        printf("%s\n", mpdclient_get_current_song_name(mpd));
    }

    mpdclient_free(mpd);
    return 0;
}
