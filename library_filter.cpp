#include "library_filter.h"
#include "plugin_base.h"
#include "messages.h"
#include <iostream>
WIDGET_IMPLEMENTATION(LibraryFilterWidget, "Library filter", "library_filter", 0)

const int FILTER_MINIMAL_HEIGHT = 30;

LibraryFilterWidget::LibraryFilterWidget(ddb_gtkui_t* gtkui) : WidgetBase(gtkui, DDB_GTKUI_WIDGET_FLAG_NON_EXPANDABLE) 
{
    textbox = gtk_text_view_new();
    buffer = gtk_text_buffer_new(NULL);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(textbox), buffer);
    g_signal_connect(buffer, "changed", (GCallback) StaticTextChanged, (gpointer) this);
}

LibraryFilterWidget::~LibraryFilterWidget() 
{
    std::cout << "LibraryFilterWidget::~LibraryFilterWidget()\n";
}

gboolean LibraryFilterWidget::TextChanged(GtkTextBuffer *textbuffer) 
{
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);
    gchar* text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
    PluginBase::Get()->SendMessage(FILTER_TEXT_CHANGED, text);
    return true;
}

GtkWidget* LibraryFilterWidget::InitGtkWidget() 
{
    WidgetBase::InitGtkWidget();
    gtk_widget_set_size_request(textbox, -1, FILTER_MINIMAL_HEIGHT);
    gtk_container_add (GTK_CONTAINER (container), textbox);
    gtk_widget_show(textbox);
    return container;
}
