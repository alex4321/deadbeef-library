#ifndef LIBRARY_H
#define LIBRARY_H

#include <vector>
#include <string>
#include <functional>
#include "library_item.h"
using namespace std;

class Library {
public:
    static Library* Get(string basePath = "");
    vector<LibraryItem>& Items();
    bool Store(const LibraryItem& item);
    string Name();
    void Load();
    void Save();
    string BasePath();
    void SetBasePath(const string& path);
    void Rescan(function<void(string)> callback);
    void Sort(const vector<string>& order);
private:
    string basePath;
    static Library* instance;
    vector<LibraryItem> items;
    Library(string basePath = "");
    string libraryPath;
    bool IsMusicFile(const string& path);
    void RescanSubdir(const string& path, function<void(string)> callback);
};

#endif /* LIBRARY_H */

