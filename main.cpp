#include <deadbeef/deadbeef.h>
#include "library_plugin.h"

extern "C" {
    DB_plugin_t* library_load (DB_functions_t *ddb) {
        LibraryPlugin* plugin = LibraryPlugin::Get(ddb);
        return &plugin->pluginInfo.plugin;
    }
}
