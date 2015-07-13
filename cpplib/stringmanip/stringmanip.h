
#ifndef STRINGMANIP_H
#define STRINGMANIP_H

#include <string>
#include <vector>

using namespace std;

unsigned int split(const string& str, vector<string>& words, const string &sepchars);
string casefold (const string str, const bool to_lower);

#endif

