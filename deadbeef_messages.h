#ifndef DEADBEEF_MESSAGES_H
#define DEADBEEF_MESSAGES_H

// message ids for communicating with player
enum {
    M_SONGFINISHED,
    M_NEXTSONG,
    M_PREVSONG,
    M_PLAYSONG,
    M_PLAYSONGNUM,
    M_STOPSONG,
    M_PAUSESONG,
    M_PLAYRANDOM,
    M_TERMINATE, // must be sent to player thread to terminate
    M_PLAYLISTREFRESH,
    M_REINIT_SOUND,
    M_CONFIGCHANGED, // no arguments
};

#endif /* DEADBEEF_MESSAGES_H */

