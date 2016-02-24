#include "library_plugin.h"
#include "messages.h"
#include "widget_base.h"
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include "library_widget.h"
#include "library_filter.h"
#include "library_item.h"
#include "utils.h"
#include "library.h"
#include "deadbeef_messages.h"
DEADBEEF_PLUGIN_IMPLEMENTATION(LibraryPlugin)

LibraryPlugin::LibraryPlugin(DB_functions_t* deadbeef) : PluginBase(deadbeef) {
    FillPluginInfo();
    scanningPlaylist = NULL;
}

void LibraryPlugin::FillPluginInfo() {
    pluginInfo.plugin.version_major = 0;
    pluginInfo.plugin.version_minor = 1;
    pluginInfo.plugin.id = "library";
    pluginInfo.plugin.type = DB_PLUGIN_MISC;
    pluginInfo.plugin.name = "Library browser";
    pluginInfo.plugin.descr = "Library browser plugin";
    pluginInfo.plugin.copyright = "copyright message - author(s), license, etc";
    pluginInfo.plugin.configdialog = "property \"tree tags order :\" entry library.treeTagsOrder \"\";\n"
    "property \"music directory:\" entry library.musicDirectory \"\";\n";
}

DB_plugin_action_t* LibraryPlugin::Actions(DB_playItem_t* item) {
    return NULL;
}

int LibraryPlugin::CmdLine(const char* cmdline, int cmdline_size) {
    return 0;
}

int LibraryPlugin::Connect() {
    gtkui = (ddb_gtkui_t*)deadbeef->plug_get_for_id(DDB_GTKUI_PLUGIN_ID);
    if (gtkui && gtkui->gui.plugin.version_major == 2) {
        LibraryWidget::Register(gtkui);
        LibraryFilterWidget::Register(gtkui);
        return 0;
    }
    else {
        return -1;
    }
}

int LibraryPlugin::Disconnect() {
    gtkui = (ddb_gtkui_t*)deadbeef->plug_get_for_id(DDB_GTKUI_PLUGIN_ID);
    if (gtkui && gtkui->gui.plugin.version_major == 2) {
        LibraryWidget::Unregister();
        LibraryFilterWidget::Unregister();
        return 0;
    }
    return 0;
}

void LibraryPlugin::TrackChangeHandler(ddb_event_track_t *ev) 
{
    LibraryItem item(deadbeef);
    if (Library::Get()->Store(item) && item.filename != "") {
        SendMessage(MESSAGE_LIBRARY_UPDATE, NULL);
        string orderStr(deadbeef->conf_get_str_fast("library.treeTagsOrder", "%B %b %t"));
        vector<string> order = split(orderStr, ' ');
        Library::Get()->Sort(order);
    }
    if (updatingPaths.size() >= 1)
    {
        if (item.filename != updatingPaths[0])
        {
            return;
        }
        updatingPaths.erase(updatingPaths.begin());
        deadbeef->plt_set_curr(scanningPlaylist);
        if (updatingPaths.size() != 0)
        {
            deadbeef->plt_set_curr(scanningPlaylist);
            deadbeef->plt_add_file(scanningPlaylist, updatingPaths[0].c_str(), NULL, NULL);
            Deadbeef()->sendmessage (M_NEXTSONG, 0, 0, 0);
        }
        else
        {
            deadbeef->plt_remove(deadbeef->plt_get_curr_idx());
            Deadbeef()->sendmessage (M_STOPSONG, 0, 0, 0);
            string orderStr(deadbeef->conf_get_str_fast("library.treeTagsOrder", "%B %b %t"));
            vector<string> order = split(orderStr, ' ');
            Library::Get()->Sort(order);
        }
    }
}

int LibraryPlugin::Message(uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2) {
    if (id == DB_EV_TRACKINFOCHANGED) {
        ddb_event_track_t *ev = (ddb_event_track_t *)ctx;
        TrackChangeHandler(ev);
    }
    return 0;
}

int LibraryPlugin::Start() {
    DB_functions_t* deadbeef = Deadbeef();
    deadbeef->conf_lock();
    string libraryPath(deadbeef->conf_get_str_fast("library.musicDirectory", ""));
    if (libraryPath == "")
    {
        libraryPath = string(getenv("HOME"));
    }
    deadbeef->conf_unlock();
    Library::Get(libraryPath);
    return 0;
}

int LibraryPlugin::Stop() {
    Library::Get()->Save();
    return 0;
}

void LibraryPlugin::MessageHandler(PluginMessage message) 
{
    switch(message)
    {
        case MESSAGE_LIBRARY_RESCAN_REQUEST:
            Rescan();
            break;
        default:
            break;
    }
}

void LibraryPlugin::Rescan() 
{
    SendMessage(MESSAGE_LIBRARY_RESCAN_STARTED, NULL);
    Library* library = Library::Get();
    updatingPaths.clear();
    library->Rescan([this](string path) 
    {
        updatingPaths.push_back(path);
    });
    if (updatingPaths.size() == 0)
    {
        return;
    }
    DB_functions_t* deadbeef = Deadbeef();
    int playlist_index = deadbeef->plt_add(0, "scanning");
    deadbeef->plt_set_curr_idx(playlist_index);
    scanningPlaylist = deadbeef->plt_get_curr();
    deadbeef->pl_lock();
    deadbeef->plt_add_file(scanningPlaylist, updatingPaths[0].c_str(), NULL, NULL);
    deadbeef->plt_set_cursor (scanningPlaylist, 0, 0);
    deadbeef->sendmessage (M_PLAYSONG, 0, 0, 0);
    deadbeef->pl_unlock();
}
