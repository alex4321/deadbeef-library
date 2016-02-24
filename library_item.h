#ifndef LIBRARY_ITEM
#define LIBRARY_ITEM

#include <map>
#include <string>
#include <deadbeef/deadbeef.h>
using namespace std;

class LibraryItem
{
private:
    bool equal(const LibraryItem& item);
public:
    string filename;
    map<string, string> tags;
    
    LibraryItem();
    LibraryItem(string _filename, map<string, string> _tags);
    LibraryItem(DB_functions_t* deadbeef, DB_playItem_t* item = NULL);
    bool operator == (const LibraryItem& item);
    bool operator != (const LibraryItem& item);
};

#endif