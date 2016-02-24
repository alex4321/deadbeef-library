#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include "widget_base.h"
#include <gtk/gtk.h>
#include <string>
#include <vector>
using namespace std;

class LibraryWidget : public WidgetBase {
    WIDGET(LibraryWidget)
private:
    bool updateEnabled;
    GtkWidget* tree;
    GtkWidget* scrollArea;
    string filter;
    
    void TreeInitialize();
    void TreeFill();
    void InitMenu();
    WIDGET_SIGNAL(LibraryWidget, StaticPopupMenu, PopupMenu, (GtkWidget* tree, GdkEvent* e, gpointer object), (tree, e));
    WIDGET_SIGNAL(LibraryWidget, StaticButtonPressed, ButtonPressed, (GtkWidget* tree, GdkEvent* e, gpointer object), (tree, e));
    WIDGET_SIGNAL(LibraryWidget, StaticPlayClicked, PlayClicked, (GtkWidget* item, gpointer object), (item));
    WIDGET_SIGNAL(LibraryWidget, StaticAddToPlaylistClicked, AddToPlaylistClicked, (GtkWidget* item, gpointer object), (item));
    WIDGET_SIGNAL(LibraryWidget, StaticTreeViewDblClicked, TreeViewDblClicked, 
        (GtkTreeView* tree, GtkTreePath* path, GtkTreeViewColumn* column, gpointer object), (tree, path, column));
    WIDGET_SIGNAL(LibraryWidget, StaticRescanClicked, RescanClicked, (GtkWidget* item, gpointer object), (item))
    
    static void SelectionIterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iterselection, vector<string>* selection);
    vector<string> Selection();
    
    void ShowPopupMenu(GdkEventButton* e);
    gboolean PopupMenu(GtkWidget* treeview, GdkEvent *event);
    gboolean ButtonPressed(GtkWidget* treeview, GdkEvent *event);
    
    void PlaySelected();
    
    bool TreeViewDblClicked(GtkTreeView* tree, GtkTreePath* path, GtkTreeViewColumn* collumn);
    vector<DB_playItem_t*> AddToDeadbeefPlayList(vector<string> files);
    gboolean PlayClicked(GtkWidget* sender);
    gboolean AddToPlaylistClicked(GtkWidget* sender);
    gboolean RescanClicked(GtkWidget* sender);
    void TreeFilter(string newFilter);
public:
    LibraryWidget(ddb_gtkui_t* gtkui);
    ~LibraryWidget();
    virtual GtkWidget* InitGtkWidget();
    virtual void PluginMessageHandler(PluginMessage message, void* data);
};

#endif /* LIBRARYWIDGET_H */

