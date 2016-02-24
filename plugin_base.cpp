#include "plugin_base.h"
#include <algorithm>
#include <iostream>
PluginBase* PluginBase::instance = NULL;
DEADBEEF_PLUGIN_IMPLEMENTATION(PluginBase);

PluginBase::PluginBase(DB_functions_t* _deadbeef) {
    deadbeef = _deadbeef;
    pluginInfo.plugin.api_vmajor = DB_API_VERSION_MAJOR;
    pluginInfo.plugin.api_vminor = DB_API_VERSION_MINOR;
    FillHandlersInfo();
    FillPluginInfo();
}

PluginBase::~PluginBase() {
}

void PluginBase::FillHandlersInfo() {
    pluginInfo.plugin.message = &PluginBase::StaticMessage;
    pluginInfo.plugin.start = &PluginBase::StaticStart;
    pluginInfo.plugin.stop = &PluginBase::StaticStop;
    pluginInfo.plugin.connect = &PluginBase::StaticConnect;
    pluginInfo.plugin.disconnect = &PluginBase::StaticDisconnect;
    pluginInfo.plugin.exec_cmdline = &PluginBase::StaticCmdline;
    pluginInfo.plugin.get_actions = &PluginBase::StaticActions;
}

void PluginBase::FillPluginInfo()
{
    pluginInfo.plugin.version_major = 0;
    pluginInfo.plugin.version_minor = 1;
    pluginInfo.plugin.type = DB_PLUGIN_MISC;
    pluginInfo.plugin.id="library";
    pluginInfo.plugin.name ="Test plugin";
    pluginInfo.plugin.descr = "Test plugin";
    pluginInfo.plugin.copyright = "copyright message - author(s), license, etc";
    pluginInfo.plugin.configdialog = NULL;
}

int PluginBase::Start()
{
    return 0;
}

int PluginBase::Stop()
{
    return 0;
}

int PluginBase::Connect()
{
    return 0;
}

int PluginBase::Disconnect()
{
    return 0;
}

int PluginBase::Message(uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2)
{
    return 0;
}

DB_plugin_action_t* PluginBase::Actions(DB_playItem_t* item) 
{
    return NULL;
}

int PluginBase::CmdLine(const char* cmdline, int cmdline_size) 
{
    return 0;
}

void PluginBase::AddHandler(void(*handler)(PluginMessage, void*)) 
{
    for(void(*_handler)(PluginMessage, void*) : messageHandlers)
    {
        if (handler == _handler)
        {
            return;
        }
    }
    messageHandlers.push_back(handler);
}

void PluginBase::RemoveHandler(void(*handler)(PluginMessage, void*))
{
    auto it = std::find(messageHandlers.begin(), messageHandlers.end(), handler);
    if(it != messageHandlers.end())
        messageHandlers.erase(it);
}

void PluginBase::SendMessage(PluginMessage message, void* data) {
    MessageHandler(message);
    for(void(*handler)(PluginMessage, void*) : messageHandlers)
    {
        handler(message, data);
    }
}

DB_functions_t* PluginBase::Deadbeef() 
{
    return deadbeef;
}

void PluginBase::MessageHandler(PluginMessage message) 
{
}
