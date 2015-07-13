
#ifndef LOGFILE_H
#define LOGFILE_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include "../system/system.h"
#include "../except/except.h"

using namespace std;

class logfile {
  private:
    string filename;
    ofstream ofs;
    // Don't allow assignment or copy construction (made private + not implemented)
    logfile& operator=(const logfile &x);
    logfile(const logfile &x);
  public:
    // Constructors/destructors
    logfile();
    ~logfile();
    // Init method
    void init(const string &file, const bool replace);
    // Method to add data
    void add(const bool silent, const string &message);
    // Close method
    void close();
};

#endif

