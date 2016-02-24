#include "utils.h"
#include <sstream>
using namespace std;

vector<std::string> &split(const string &s, char delim, vector<string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

vector<std::string> split(const string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

string strToLower(const string& source)
{
    string result = "";
    for(char c: source)
    {
        result.push_back(tolower(c));
    }
    return result;
}