#include <iostream>
#include <string>

#include "../../except/except.h"
#include "../table.h"

using namespace std;

int main () {

  string ifile="/home/dkneis/progress/echse/echse_generic/cpplib/current/table/test/data2.txt";
  string ofile="/home/dkneis/progress/echse/echse_generic/cpplib/current/table/test/data_colFiltered.txt";
 
  table tab;
  vector<string> x;
  vector<table::size_type> colinds;

  try {
     // Read table
    tab.read(ifile, true, "\t", "#");
    cout << "Table has " << tab.nrow() << " rows and " << tab.ncol() << " cols." << endl;

    // Column names
    tab.get_colnames(x);
    for (unsigned int i=0; i<x.size(); i++) {
      cout << "name: " << x[i] << endl;
    }

    // subset
    colinds.resize(3);
    colinds[0]= 1;
    colinds[1]= 2;
    colinds[2]= 4;

    tab.insert_row({"a","b","c","d"}, false);
    tab.insert_row({"w","x","y","z"}, true);
    cout << "Table has " << tab.nrow() << " rows and " << tab.ncol() << " cols." << endl;

    tab.write(ofile, "\t", false, colinds, true);

  } catch (except) {
    except e("main","Failed",__FILE__,__LINE__);
    e.print();
    return(1);
  }
  return(0);
}

