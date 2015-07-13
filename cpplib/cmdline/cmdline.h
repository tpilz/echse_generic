#ifndef CMDLINE_H
#define CMDLINE_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include "../except/except.h"

using namespace std;

class cmdline {
  public:
    typedef map<string, string> T_map;
  private:
    typedef pair<string, string> T_pair;
    typedef pair<T_map::const_iterator ,bool> T_insertStatus;
    vector<string> cmdargs;
	public:
    cmdline(const int argc, const char* argv[]);
    ~cmdline();
    string get_value(const string key, const string sep) const;
    T_map export_pairs(const string keysep) const;
};

#endif

