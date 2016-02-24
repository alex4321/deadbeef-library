#ifndef WIDGET_H
#define WIDGET_H

#include "plugin_base.h"
#include <vector>
#include <iostream>
#include <stdint.h>
#include <gtk/gtk.h>
#include <deadbeef/deadbeef.h>
#include <deadbeef/gtkui_api.h>
#include <pthread.h>

#define WIDGET(Class) \
    public: \
        static const char* title; \
        static const char* id; \
        static uint32_t flags; \
        static void Register(ddb_gtkui_t* gtkui); \
        static void Unregister(); \
        static ddb_gtkui_widget_t* Create();

#define WIDGET_IMPLEMENTATION(Class, Title, WidgetId, Flags) \
    const char* Class::title = Title; \
    const char* Class::id = WidgetId; \
    uint32_t Class::flags = Flags; \
    void Class::Register(ddb_gtkui_t* gtkui) \
    { \
        ui = gtkui; \
        ui->w_reg_widget(title, flags, &Class::Create, WidgetId, NULL); \
    } \
    void Class::Unregister() \
    { \
        ui->w_unreg_widget(id); \
    } \
    ddb_gtkui_widget_t* Class::Create() \
    { \
        Class* instance = new Class(ui);  \
        instance->Run(); \
        return instance->DDBBase(); \
    }


#define WIDGET_SIGNAL(Class, StaticName, Name, Types, CallTypes) \
    static gboolean StaticName Types { \
        if (WidgetBase::Registered((WidgetBase*)object)) \
            return ((Class*)object)->Name CallTypes; \
        else \
            std::cerr << "UNREGISTERED OBJECT " << object << "\n"; \
            return false; \
    }


class WidgetBase {
protected:
    static ddb_gtkui_t* ui;
    ddb_gtkui_widget_t* base;
    static WidgetBase* Find(ddb_gtkui_widget_t* base);
    static std::vector<WidgetBase*> widgets;
    GtkWidget* container;
    static bool Registered(WidgetBase* _widget);
    pthread_t mainThread;
    void Update();
    void UpdateFinished();
private:
    static void StaticInit(ddb_gtkui_widget_t* widget);
    static void StaticSave(ddb_gtkui_widget_t* widget, char* s, int sz);
    static char* StaticLoad(ddb_gtkui_widget_t* widget, const char *type, const char *s);
    static void StaticDestroy(ddb_gtkui_widget_t* widget);
    static void StaticAppend(ddb_gtkui_widget_t* widget, ddb_gtkui_widget_t* child);
    static void StaticRemove(ddb_gtkui_widget_t* widget, ddb_gtkui_widget_t* child);
    static void StaticReplace(ddb_gtkui_widget_t* widget, ddb_gtkui_widget_t* child, ddb_gtkui_widget_s *newchild);
    static GtkWidget* StaticGetContainer(ddb_gtkui_widget_t* widget);
    static void StaticMessage(ddb_gtkui_widget_s *widget, uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2);
    static void StaticMenu (ddb_gtkui_widget_s *widget, GtkWidget *menu);
    static void StaticChildMenu (ddb_gtkui_widget_s *widget, GtkWidget *menu);
    static void StaticPluginMessageHandler(PluginMessage message, void* data);
public:
    WidgetBase(ddb_gtkui_t* gtkui, uint32_t flags);
    virtual ~WidgetBase();
    void Run();
    ddb_gtkui_widget_t* DDBBase();
    GtkWidget* Base();
    virtual GtkWidget* InitGtkWidget();
    virtual void Init();
    virtual void Save(char* s, int sz);
    virtual char* Load(const char *type, const char *s);
    virtual void Append(ddb_gtkui_widget_s *child);
    virtual void Remove(ddb_gtkui_widget_s *child);
    virtual void Replace(ddb_gtkui_widget_s *child, ddb_gtkui_widget_s *newchild);
    virtual GtkWidget* GetContainer();
    virtual void Message(uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2);
    virtual void Menu(GtkWidget *menu);
    virtual void ChildMenu (GtkWidget *menu);
    virtual void PluginMessageHandler(PluginMessage message, void* data);
};

#endif /* WIDGET_H */

