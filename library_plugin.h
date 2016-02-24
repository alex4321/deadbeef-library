#ifndef PLUGIN_EXAMPLE_H
#define PLUGIN_EXAMPLE_H
#include "plugin_base.h"
#include <stdint.h>
#include <gtk/gtk.h>
#include <queue>
#include <deadbeef/deadbeef.h>
#include <deadbeef/gtkui_api.h>
#include <vector>
#include <string>

class LibraryPlugin : public PluginBase {
    DEADBEEF_PLUGIN(LibraryPlugin)
private:
    ddb_gtkui_t* gtkui;
    vector<string> updatingPaths;
    ddb_playlist_t* scanningPlaylist;
    void TrackChangeHandler(ddb_event_track_t *ev);
public:
    LibraryPlugin(DB_functions_t* deadbeef);
    virtual void FillPluginInfo();
    virtual int Connect();
    virtual int Disconnect();
    virtual int Start();
    virtual int Stop();
    virtual DB_plugin_action_t* Actions(DB_playItem_t* item);
    virtual int CmdLine(const char* cmdline, int cmdline_size);
    virtual int Message(uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2);
    virtual void MessageHandler(PluginMessage message);

    void Rescan();
};

#endif /* PLUGIN_EXAMPLE_H */

