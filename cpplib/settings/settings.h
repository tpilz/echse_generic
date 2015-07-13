#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <map>
#include <sstream>
#include "../except/except.h"
#include "../table/table.h"
#include "../cmdline/cmdline.h"

using namespace std;

class settings {
  private:
    typedef map<string, string> T_map;
    typedef pair<string, string> T_pair;
    typedef pair<T_map::const_iterator ,bool> T_insertStatus;
    T_map data;
    string source;
	public:
    settings();
    ~settings();
    void read_fileOnly(const string &file, const string &chars_colsep, const string &chars_comment);
    void read_fileAndArgs(const string &file, const string &chars_colsep, const string &chars_comment,
      const cmdline &cmdl, const string &chars_keysep);
    void clear();
    // Overloading of index operator for access
    const string& operator[](const string &key) const;
};

#endif

