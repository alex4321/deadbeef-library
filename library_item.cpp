#include <vector>
#include <iostream>
#include "library_item.h"

LibraryItem::LibraryItem()
{
}

LibraryItem::LibraryItem(string _filename, map<string, string> _tags) {
    filename = _filename;
    tags = _tags;
}

LibraryItem::LibraryItem(DB_functions_t* deadbeef, DB_playItem_t* item) {
    DB_playItem_t *it;
    if (item == NULL)
    {
        it = deadbeef->streamer_get_playing_track ();    
    }
    else
    {
        it = item;
    }
    vector<string> tag_names { "%a", "%t", "%b", "%B", "%n", "%l", "%y", "%g", "%c", "%r", "%T", "%f", "%F" };
    char buffer[256];
    for (string tag: tag_names) {
        deadbeef->pl_format_title(it, -1, buffer, 256, -1, tag.c_str());
        tags.insert(tags.begin(), pair<string, string>(tag, string(buffer)));
    }
    deadbeef->pl_format_title(it, -1, buffer, 256, -1, "%F");
    filename = string(buffer);
}

bool LibraryItem::equal(const LibraryItem& item) 
{
    if (filename != item.filename)
    {
        return false;
    }
    for (pair<string, string> tagItem: tags)
    {
        if (item.tags.count(tagItem.first) == 0)
        {
            return false;
        }
    }
    for (pair<string, string> tagItem: item.tags)
    {
        if (tags.count(tagItem.first) == 0)
        {
            return false;
        }
    }
    for (pair<string, string> tagItem: item.tags)
    {
        string tag = tagItem.first;
        string thisValue = tags.at(tag);
        string otherValue = item.tags.at(tag);
        if (thisValue != otherValue)
        {
            return false;
        }
    }
    return true;
}

bool LibraryItem::operator==(const LibraryItem& item) 
{
    return equal(item);
}

bool LibraryItem::operator!=(const LibraryItem& item) 
{
    return !equal(item);
}
