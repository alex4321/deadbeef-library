#include "widget_base.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>

std::vector<WidgetBase*> WidgetBase::widgets;
ddb_gtkui_t* WidgetBase::ui = NULL;

WidgetBase::WidgetBase(ddb_gtkui_t* gtkui, uint32_t flags) {
    ui = gtkui;
    container = NULL;
    base = (ddb_gtkui_widget_t*)malloc (sizeof (ddb_gtkui_widget_t));
    memset (base, 0, sizeof (ddb_gtkui_widget_t));  
    base->init = WidgetBase::StaticInit;
    //base->load = WidgetBase::StaticLoad;
    //base->save = WidgetBase::StaticSave;
    base->initmenu = WidgetBase::StaticMenu;
    base->initchildmenu = WidgetBase::StaticChildMenu;
    base->destroy = WidgetBase::StaticDestroy;
    base->get_container = WidgetBase::StaticGetContainer;
    base->flags = flags;
    PluginBase::Get()->AddHandler(WidgetBase::StaticPluginMessageHandler);
    mainThread = 0;
}

bool WidgetBase::Registered(WidgetBase* _widget) 
{
    for(WidgetBase* widget: widgets)
    {
        if(widget == _widget)
        {
            return true;
        }
    }
    return false;
}

void WidgetBase::Run()
{
    base->widget = InitGtkWidget();
    gtk_widget_show(Base());
    ui->w_override_signals(Base(), base);
    gtk_widget_set_size_request(base->widget, -1, -1);
    widgets.push_back(this);
}

WidgetBase::~WidgetBase()
{
    for (size_t i=0; i<widgets.size(); i++)
    {
        if (widgets[i] == this)
        {
            widgets.erase(widgets.begin() + i);
            break;
        }
    }
}

WidgetBase* WidgetBase::Find(ddb_gtkui_widget_t* base) {
    for (size_t i=0; i<widgets.size(); i++)
    {
        if (widgets[i]->base == base)
        {
            return widgets[i];
        }
    }
    return NULL;
}

ddb_gtkui_widget_t* WidgetBase::DDBBase() 
{
    return base;
}

GtkWidget* WidgetBase::Base() {
    return (GtkWidget*)base->widget;
}

void WidgetBase::StaticInit(ddb_gtkui_widget_t* widget) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        _widget->Init();
    }
}

void WidgetBase::StaticSave(ddb_gtkui_widget_t* widget, char* s, int sz) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        _widget->Save(s, sz);
    }
}

char* WidgetBase::StaticLoad(ddb_gtkui_widget_t* widget, const char* type, const char* s) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        return _widget->Load(type, s);
    }
    else {
        return NULL;
    }
}

void WidgetBase::StaticDestroy(ddb_gtkui_widget_t* widget) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        delete _widget;
    }
}

void WidgetBase::StaticAppend(ddb_gtkui_widget_t* widget, ddb_gtkui_widget_t* child) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        _widget->Append(child);
    }
}

void WidgetBase::StaticRemove(ddb_gtkui_widget_t* widget, ddb_gtkui_widget_t* child) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        _widget->Remove(child);
    }
}

void WidgetBase::StaticReplace(ddb_gtkui_widget_t* widget, ddb_gtkui_widget_t* child, ddb_gtkui_widget_s* newchild) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        _widget->Replace(child, newchild);
    }
}

GtkWidget* WidgetBase::StaticGetContainer(ddb_gtkui_widget_t* widget) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        return _widget->GetContainer();
    }
    else
    {
        return NULL;
    }
}

void WidgetBase::StaticMessage(ddb_gtkui_widget_s* widget, uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        _widget->Message(id, ctx, p1, p2);
    }
}

void WidgetBase::StaticMenu(ddb_gtkui_widget_s* widget, GtkWidget* menu) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        _widget->Menu(menu);
    }
}

void WidgetBase::StaticChildMenu(ddb_gtkui_widget_s* widget, GtkWidget* menu) {
    WidgetBase* _widget = Find(widget);
    if (_widget) {
        _widget->ChildMenu(menu);
    }
}

void WidgetBase::StaticPluginMessageHandler(PluginMessage message, void* data) 
{
    for (WidgetBase* widget: widgets)
    {
        widget->PluginMessageHandler(message, data);
    }
}

void WidgetBase::Init()
{
}

void WidgetBase::Save(char* s, int sz) 
{
}


char* WidgetBase::Load(const char* type, const char* s) 
{
    return NULL;
}

void WidgetBase::Append(ddb_gtkui_widget_s* child) 
{
}

void WidgetBase::Remove(ddb_gtkui_widget_s* child) 
{
}

void WidgetBase::Replace(ddb_gtkui_widget_s* child, ddb_gtkui_widget_s* newchild) 
{
}

GtkWidget* WidgetBase::GetContainer() 
{
    return NULL;
}

void WidgetBase::Message(uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2)
{
}

void WidgetBase::Menu(GtkWidget* menu) 
{
}

void WidgetBase::ChildMenu(GtkWidget* menu) 
{
}

void WidgetBase::PluginMessageHandler(PluginMessage message, void* data) 
{
}

GtkWidget* WidgetBase::InitGtkWidget() 
{
    container = gtk_event_box_new ();
    gtk_widget_set_size_request (container, -1, -1);
    return container;
}

void WidgetBase::Update() 
{
    if ( (mainThread == 0) || (pthread_self() == mainThread)) {
        mainThread = pthread_self();
    } else {
        gdk_threads_lock();
    }
}

void WidgetBase::UpdateFinished() 
{
    gdk_threads_unlock();
}

