
#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include "../except/except.h"
#include "../typeconv/typeconv.h"
#include "../stringmanip/stringmanip.h"
#include "../system/system.h"

using namespace std;

class table {

  public:
    // Types
    typedef unsigned int size_type;
    typedef multimap<string, size_type> T_searchmap;  // Map to save values of a column and its row indices
    typedef pair<string, size_type> T_searchmap_pair; // Single pair of the map
  private:
    // Private data
    size_type numcols;
    size_type numrows;
    vector<string> data;
    vector<string> colnames;
    T_searchmap searchmap;
    size_type index_searchcolumn;
    // Private methods
    void export_to_file(const vector<size_type> &colindices, const vector<size_type> &rowindices,
      const string &file, const string &colsep, const bool &replace) const;
  public:
    // Constructors, destructors
    table();
    ~table();
    // Methods to query table dimensions
    size_type nrow() const;
    size_type ncol() const;
    bool empty() const;
    // Methods to retrieve/set column names
    string get_colname(const size_type index) const;
    void get_colnames(vector<string> &strvect) const;
    void set_colname(const size_type index, const string &colname);
    // Method to convert a column name to the column index
    size_type colindex(const string &colname) const;
    // Update search column (stores search column in a map)
    void update_searchcolumn(const size_type index_col);
    // Method to retrieve the index of the first matching row (>=1; 0 if no match)
    table::size_type first_row(const size_type index_col_filter,
      const string &searchstring) const;
    // Method to retrieve an array of row indices based on search in a column
    void which_rows(const size_type index_col_filter, const string &searchstring,
      vector<size_type> &indices) const;
    // Method to retrieve a single element using its col/row index
    string get_element(const size_type index_row,
      const size_type index_col) const;
    // Method to retrieve an entire row
    void get_row(const size_type index, vector<string> &strvect) const;
    // Method to retrieve an entire column
    void get_col(const size_type index, vector<string> &strvect) const;
    // Methods to return a new table containing a subset of the original rows (columns)
    void subset(const vector<size_type> &rowindices, table &tab) const;
    void subset_cols(const vector<size_type> &colindices, table &tab) const;
    // Method to insert a row
    void insert_row(const vector<string> &values, const bool at_end);
    // Method to read table contents from file
    void read(const string &file, const bool &header, const string &chars_colsep,
      const string &chars_comment);
    // Methods to write table contents to file
    void write(const string &file, const string &colsep, const bool &replace) const;
    void write(const string &file, const string &colsep, const bool &replace,
      const vector<size_type> colindices, const vector<size_type> rowindices) const;
    void write(const string &file, const string &colsep, const bool &replace,
      const vector<size_type> indices, const bool indices_are_columns) const;
    // Methods to delete table data
    void clear();
};

#endif

