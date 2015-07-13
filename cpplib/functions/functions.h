
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "../except/except.h"
#include "../table/table.h"
#include "../typeconv/typeconv.h"

using namespace std;

class tblFunction {
  private:
    // Types and constants
    typedef unsigned int size_type;
    static const double FRACTOL_XSPACING;
    // Function name
    string name;
    // The function
    vector<double> x;
    vector<double> fx;
    // Data to facilitate data access
    size_type narg;
    size_type index_lastaccess;
    bool x_equispaced;
  public:
    // Constructors/destructors
    tblFunction();
    ~tblFunction();
    // Method to initialize a function table using two arrays
    void init(const string funcname, const vector<double> &args,
      const vector<double> &vals);
    // Method to initialize a function table from data file
		void read(const string file, const bool header, const string chars_colsep,
      const string chars_comment, const string funcname,
      const string colname_x, const string colname_fx);
    // Methods to get argument range
    double minarg() const;
    double maxarg() const;
    // Method to lookup a value
    double eval(const double arg); // Not const because of access index update
};

#endif

