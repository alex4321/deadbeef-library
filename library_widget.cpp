#include <gtk/gtk.h>
#include "messages.h"
#include "library.h"
#include "library_widget.h"
#include <iostream>
#include <gtk-2.0/gtk/gtktable.h>
#include "plugin_base.h"
#include "utils.h"
#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include "deadbeef_messages.h"

using namespace std;
WIDGET_IMPLEMENTATION(LibraryWidget, "Library", "library", 0)

LibraryWidget::LibraryWidget(ddb_gtkui_t* gtkui) : WidgetBase(gtkui, DDB_GTKUI_WIDGET_FLAG_NON_EXPANDABLE) 
{
    updateEnabled = true;
    tree = gtk_tree_view_new();
    scrollArea = gtk_scrolled_window_new(NULL, NULL);
    filter = "";
}

LibraryWidget::~LibraryWidget() 
{
}

void LibraryWidget::TreeInitialize()
{
    GtkTreeStore* registry = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeViewColumn* column = gtk_tree_view_column_new();
    GtkCellRenderer *text_cell = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, text_cell, TRUE);
    gtk_tree_view_column_add_attribute(column, text_cell, "text", 0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(registry));
    gtk_signal_connect(GTK_OBJECT(tree), "row-activated", (GCallback)StaticTreeViewDblClicked, this);
}

void LibraryWidget::TreeFill() 
{
    Update();
    GtkTreeStore* registry = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree), NULL);
    gtk_tree_store_clear(registry);
    Library* lib = Library::Get();
    string orderStr(PluginBase::Get()->Deadbeef()->conf_get_str_fast("library.treeTagsOrder", "%B %b %t"));
    vector<string> order = split(orderStr, ' ');
    map<string, GtkTreeIter*> iters;
    for(LibraryItem item : lib->Items())
    {
        GtkTreeIter* current = NULL;
        string iterFilter = "";
        bool add;
        if (filter != "")
        {
            add = false;
            for(string tagName: order)
            {
                string value = item.tags[tagName];
                if (strToLower(value).find(strToLower(filter)) != string::npos)
                {
                    add = true;
                    break;
                }
            }
        }
        else 
        {
            add = true;
        }
        if (! add)
        {
            continue;
        }
        for(size_t i=0; i < order.size() - 1; i++)
        {
            string tag = order[i];
            string tagValue = item.tags[tag];
            iterFilter += ":" + tagValue;
            if (iters.count(iterFilter) == 0) {
                GtkTreeIter* iter = new GtkTreeIter;
                gtk_tree_store_append(registry, iter, current);
                gtk_tree_store_set(registry, iter, 0, tagValue.c_str(), 1, NULL, -1);
                current = iter;
                iters.insert(pair<string, GtkTreeIter*>(iterFilter, gtk_tree_iter_copy(iter)));
            } else {
                current = iters[iterFilter];
            }
        }
        string lastTag = order[order.size() - 1];
        string lastTagValue = item.tags[lastTag];
        GtkTreeIter lastIter;
        gtk_tree_store_append(registry, &lastIter, current);
        gtk_tree_store_set(registry, &lastIter, 0, lastTagValue.c_str(), 1, item.filename.c_str(), -1);
    }
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(registry));
    gtk_tree_view_expand_all(GTK_TREE_VIEW(tree));
    UpdateFinished();
}

void LibraryWidget::SelectionIterate(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iterselection, vector<string>* selection) 
{
    char* title;
    char* file;
    gtk_tree_model_get_iter (model, iterselection, path);
    gtk_tree_model_get (model, iterselection, 0, &title, 1, &file, -1);
    if (file == NULL) {
        int childrenCount = gtk_tree_model_iter_n_children(model, iterselection);
        GtkTreeIter childIter;
        for(int i=0; i<childrenCount; i++) {
            gtk_tree_model_iter_nth_child(model, &childIter, iterselection, i);
            SelectionIterate(model, gtk_tree_model_get_path(model, &childIter), &childIter, selection);
        }
    } else {
        selection->push_back(file);
    }
}

vector<string> LibraryWidget::Selection() 
{
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    vector<string> result;
    gtk_tree_selection_selected_foreach(selection, (GtkTreeSelectionForeachFunc)&SelectionIterate, (gpointer)&result);
    return result;
}

vector<DB_playItem_t*> LibraryWidget::AddToDeadbeefPlayList(vector<string> files) 
{
    DB_functions_t* deadbeef = PluginBase::Get()->Deadbeef();
    ddb_playlist_t* playlist = deadbeef->plt_get_curr ();
    vector<DB_playItem_t*> items(files.size());
    deadbeef->pl_lock();
    for (size_t i=0; i<files.size(); i++)
    {
        int index = deadbeef->plt_add_file(playlist, files[i].c_str(), NULL, NULL);
        items[i] = deadbeef->plt_get_item_for_idx(playlist, index, PL_MAIN);
    }
    deadbeef->pl_unlock();
    deadbeef->sendmessage(M_PLAYLISTREFRESH, 0, 0, 0);
    return items;
}

void LibraryWidget::PlaySelected()
{
    DB_functions_t* deadbeef = PluginBase::Get()->Deadbeef();
    ddb_playlist_t* playlist = deadbeef->plt_get_curr ();
    vector<string> files = Selection();
    vector<DB_playItem_t*> items(files.size());
    deadbeef->plt_clear(playlist);
    AddToDeadbeefPlayList(files);
    if (files.size() >= 1) 
    {
        deadbeef->plt_set_cursor(deadbeef->plt_get_curr (), 0, 0);
        deadbeef->sendmessage (M_PLAYSONG, 0, 0, 0);
    }
}

bool LibraryWidget::TreeViewDblClicked(GtkTreeView* tree, GtkTreePath* path, GtkTreeViewColumn* collumn)
{
    PlaySelected();
    return true;
}


gboolean LibraryWidget::PlayClicked(GtkWidget* sender) 
{
    PlaySelected();
    return true;
}

gboolean LibraryWidget::AddToPlaylistClicked(GtkWidget* sender) 
{
    DB_functions_t* deadbeef = PluginBase::Get()->Deadbeef();
    vector<string> files = Selection();
    vector<DB_playItem_t*> items(files.size());
    AddToDeadbeefPlayList(files);
    deadbeef->plt_set_cursor(deadbeef->plt_get_curr (), 0, 0);
    return true;
}

gboolean LibraryWidget::RescanClicked(GtkWidget* sender) 
{
    PluginBase::Get()->SendMessage(MESSAGE_LIBRARY_RESCAN_REQUEST, NULL);
    return true;
}

void LibraryWidget::ShowPopupMenu(GdkEventButton* e)
{
    GtkWidget*menu = gtk_menu_new();
    GtkWidget* play = gtk_menu_item_new_with_label("Play immediatelly");
    GtkWidget* addToPlayList = gtk_menu_item_new_with_label("Add to playlist");
    GtkWidget* rescan = gtk_menu_item_new_with_label("Rescan");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), play);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), addToPlayList);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), rescan);
    gtk_widget_show_all(menu);
    g_signal_connect(play, "activate",
                     (GCallback) StaticPlayClicked, (gpointer)this);
    g_signal_connect(addToPlayList, "activate",
                     (GCallback) StaticAddToPlaylistClicked, (gpointer)this);
    g_signal_connect(rescan, "activate",
                     (GCallback) StaticRescanClicked, (gpointer)this);
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 
            (e != NULL) ? e->button : 0,
            gdk_event_get_time((GdkEvent*)e));
}

gboolean LibraryWidget::PopupMenu(GtkWidget* treeview, GdkEvent* event) 
{
    ShowPopupMenu(NULL);
    return true;
}

gboolean LibraryWidget::ButtonPressed(GtkWidget* treeview, GdkEvent* event)
{
    if ( ((GdkEventButton*)event)->type == GDK_BUTTON_PRESS  &&  ((GdkEventButton*)event)->button == 3)
    {
        ShowPopupMenu((GdkEventButton*)event);
        return true;
    }
    return false;
}

void LibraryWidget::InitMenu()
{
    g_signal_connect(tree, "popup-menu", (GCallback)StaticPopupMenu, (gpointer)this);
    g_signal_connect(tree, "button-press-event", (GCallback)StaticButtonPressed, (gpointer)this);
}

GtkWidget* LibraryWidget::InitGtkWidget() 
{
    WidgetBase::InitGtkWidget();
    gtk_container_add (GTK_CONTAINER (container), scrollArea);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollArea),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);    
    gtk_widget_set_size_request (scrollArea, -1, -1);
    gtk_widget_set_can_focus (scrollArea, FALSE);
    gtk_widget_set_can_focus (tree, FALSE);
    gtk_container_add (GTK_CONTAINER(scrollArea), tree);
    gtk_widget_show(tree);
    gtk_widget_show(scrollArea);
    InitMenu();
    TreeInitialize();
    TreeFill();
    return container;
}

void LibraryWidget::TreeFilter(string newFilter) 
{
    filter = newFilter;
    TreeFill();
}

void LibraryWidget::PluginMessageHandler(PluginMessage message, void* data) 
{
    switch(message)
    {
        case MESSAGE_LIBRARY_UPDATE:
            if (updateEnabled)
            {
                TreeFill();
            }
            break;
        case MESSAGE_LIBRARY_RESCAN_STARTED:
            updateEnabled = false;
            break;
        case MESSAGE_LIBRARY_RESCAN_STOPPED:
            updateEnabled = true;
            TreeFill();
            break;
        case FILTER_TEXT_CHANGED: 
            TreeFilter(string((char*)data));
            break;
        default:
            break;
    }
}
