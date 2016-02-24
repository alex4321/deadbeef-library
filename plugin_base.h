#ifndef PLUGIN_H
#define PLUGIN_H
#include "plugin_signletone.h"
#include "messages.h"
#include <deadbeef/deadbeef.h>
#include <vector>
using namespace std;

#define PLUGIN_STATICAL_METHOD(Class, Type, StaticName, DynamicName, Types, Vars) \
    static Type StaticName Types { \
        return Class::Get()->DynamicName Vars; \
    };

class PluginBase {
    DEADBEEF_PLUGIN(PluginBase)
private:
    vector<void(*)(PluginMessage, void*)> messageHandlers;
    void FillHandlersInfo();
    PLUGIN_STATICAL_METHOD(PluginBase, int, StaticStart, Start, (), ())
    PLUGIN_STATICAL_METHOD(PluginBase, int, StaticStop, Stop, (), ())
    PLUGIN_STATICAL_METHOD(PluginBase, int, StaticConnect, Connect, (), ())
    PLUGIN_STATICAL_METHOD(PluginBase, int, StaticDisconnect, Disconnect, (), ())
    PLUGIN_STATICAL_METHOD(PluginBase, int, StaticMessage, Message, 
        (uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2), (id, ctx, p1, p2));
    PLUGIN_STATICAL_METHOD(PluginBase,DB_plugin_action_t*, StaticActions, Actions,
        (DB_playItem_t *item), (item))
    PLUGIN_STATICAL_METHOD(PluginBase, int, StaticCmdline, CmdLine,
        (const char* cmdline, int cmdline_size), (cmdline, cmdline_size))
    
protected:
    static PluginBase* instance;
    PluginBase(DB_functions_t* _deadbeef);
    DB_functions_t* deadbeef;
    ~PluginBase();
public:
    void AddHandler(void(*handler)(PluginMessage, void*));
    void RemoveHandler(void(*handler)(PluginMessage, void*));
    void SendMessage(PluginMessage message, void* data);
    DB_misc_t pluginInfo;
    virtual void FillPluginInfo();
    virtual int Start(void);
    virtual int Stop();
    virtual int Connect();
    virtual int Disconnect();
    virtual int Message(uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2);
    virtual DB_plugin_action_t* Actions(DB_playItem_t* item);
    virtual int CmdLine(const char* cmdline, int cmdline_size);
    virtual void MessageHandler(PluginMessage message);
    DB_functions_t* Deadbeef();
};

#endif /* PLUGIN_H */
