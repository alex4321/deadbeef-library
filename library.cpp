#include "library.h"
#include "utils.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


Library* Library::instance = NULL;

Library::Library(string basePath) 
{
    items = vector<LibraryItem>();
    SetBasePath(basePath);
    Load();
}

Library* Library::Get(string basePath)
{
    if (Library::instance == NULL) {
        Library::instance = new Library(basePath);
    }
    return Library::instance;
}

bool Library::Store(const LibraryItem& newItem) 
{
    bool alreadyStored = false;
    for(size_t i=0; i<items.size(); i++)
    {
        if (items[i] == newItem)
        {
            alreadyStored = true;
            break;
        }
    }
    if (! alreadyStored)
    {
        items.push_back(newItem);
    }
    return !alreadyStored;
}

vector<LibraryItem>& Library::Items() 
{
    return items;
}

string Library::Name()
{
    return basePath + "/list.db";
}

void Library::Load()
{
    ifstream f;
    f.open(Name());
    string content((std::istreambuf_iterator<char>(f)),
                 std::istreambuf_iterator<char>());
    vector<string> records = split(content, ';');
    for  (string record : records) 
    {
        vector<string> parts = split(record, '\n');
        if (parts.size() > 0) 
        {
            string fileName = parts[0];
            map<string, string> tags;
            for (size_t i=1; i<parts.size(); i++)
            {
                if (parts.size() < 4)
                {
                    continue;
                }
                string key = parts[i].substr(0, 2);
                string value = parts[i].substr(3, parts[i].size() - 3);
                tags.insert(pair<string, string>(key, value));
            }
            Store(LibraryItem(fileName, tags));
        }
    }
}

void Library::Save()
{
    ofstream f;
    f.open(Name());
    for (LibraryItem item : items) {
        f << ";";
        f << item.filename << "\n";
        for (pair<string, string>p : item.tags)
        {
            string key = p.first;
            string value = p.second;
            f << key << ":" << value << "\n";
        }
    }
    f.close();
}

string Library::BasePath() 
{
    return basePath;
}

void Library::SetBasePath(const string& path) 
{
    basePath = path;
}

void Library::Rescan(function<void(string) > callback) 
{
    if (basePath == "")
    {
        return;
    }
    RescanSubdir(basePath, callback);
}

bool Library::IsMusicFile(const string& path) 
{
    if (path == Name())
    {
        return false;
    }
    vector<string> whitelist
    {
        ".3gp", ".aa", ".aac", ".aax", ".act", ".aiff", ".amr", ".ape", ".au", 
        ".awb", ".dct", ".dss", ".dvf", ".flac", ".gsm", ".iklax", ".ivs", 
        ".m4a", ".m4b", ".m4p", ".mmf", ".mp3", ".ogg", ".oga", ".opus", ".ra", 
        ".rm", ".raw", ".tta", ".vox", ".wav", ".wma", ".wv", ".webm"
    };
    string path_lowercase = strToLower(path);
    string extension = path_lowercase.substr(path_lowercase.find_last_of(".") + 1);
    for (string whitelist_extension: whitelist)
    {
        if (extension == whitelist_extension)
        {
            return true;
        }
    }
    return false;
}

void Library::RescanSubdir(const string& path, function<void(string) > callback) 
{
    DIR* dir = opendir(path.c_str());
    dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        string name(entry->d_name);
        if (name == "." || name == "..") 
        {
            continue;
        }
        string fullName = path + "/" + name;
        struct stat statbuf;
        stat(fullName.c_str(), &statbuf);
        bool isDir = S_ISDIR(statbuf.st_mode);
        if (isDir)
        {
            RescanSubdir(fullName, callback);
        }
        else if (IsMusicFile(fullName))
        {
            callback(fullName);
        }
    }
    closedir(dir);
}

void Library::Sort(const vector<string>& order) 
{
    for(string tagName: order)
    {
        sort(items.begin(), items.end(), [tagName](LibraryItem i1, LibraryItem i2)
        {
            return i1.tags[tagName] < i2.tags[tagName];
        });
    }
}
