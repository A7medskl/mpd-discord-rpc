#include "mpdclient.h"
#include <stdio.h>

#define MPD_DEFAULT_HOST "localhost"
#define MPD_DEFAULT_PORT 6600
#define MPD_DEFAULT_TIMEOUT 30000

int main()
{
    struct mpdclient *mpd = mpdclient_init(MPD_DEFAULT_HOST, MPD_DEFAULT_PORT,
                                           MPD_DEFAULT_TIMEOUT);

    if (mpd)
        printf("Successfully connected to MPD.\n");
    else
        printf("Failed to connect to MPD.\n");

    mpdclient_free(mpd);
    return 0;
}
