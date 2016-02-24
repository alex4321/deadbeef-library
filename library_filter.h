#ifndef LIBRARY_FILTER_H
#define LIBRARY_FILTER_H

#include "widget_base.h"
#include <gtk/gtk.h>

class LibraryFilterWidget : public WidgetBase {
    WIDGET(LibraryFilterWidget)
private:
    WIDGET_SIGNAL(LibraryFilterWidget, StaticTextChanged, TextChanged, 
        (GtkTextBuffer *textbuffer, gpointer object), (textbuffer));
    GtkWidget* textbox;
    GtkTextBuffer* buffer;
    gboolean TextChanged(GtkTextBuffer *textbuffer);
public:
    LibraryFilterWidget(ddb_gtkui_t* gtkui);
    virtual ~LibraryFilterWidget();
    virtual GtkWidget* InitGtkWidget();
};

#endif /* LIBRARY_FILTER_H */

