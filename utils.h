#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <list>
#include <gtk/gtk.h>
using namespace std;

vector<std::string> &split(const string &s, char delim, vector<string> &elems);
vector<std::string> split(const string &s, char delim);
string strToLower(const string& source);

#endif /* UTILS_H */

