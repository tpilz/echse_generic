#include <iostream>
#include <string>
#include <valarray>

#include "../../except/except.h"
#include "../table.h"

using namespace std;

int main () {

  const string method="main: ";

  const char* ifile="/home/dkneis/cpp/dklib/table/test/data.txt";
  const char* colseps_in="\t";
  const char* comments="#";
  const bool header=true;
 
  table tab, tab_filtered;
  vector<string> x;
  vector<unsigned int> rowinds;

  try {

		// Read table
		tab.read(ifile, header, colseps_in, comments);

		// Show dimensions
		cout << "Read table with " << tab.nrow() << " rows and " << tab.ncol() <<
      " cols." << endl;

    // Spaltennamen
    tab.get_colnames(x);
    for (unsigned int i=0; i<x.size(); i++) {
      cout << "name: " << x[i] << endl;
    }

    // Spalte
    tab.get_col(2,x);
    for (unsigned int i=0; i<x.size(); i++) {
      cout << "spalte: " << x[i] << endl;
    }

    // Zeile
    tab.get_row(2,x);
    for (unsigned int i=0; i<x.size(); i++) {
      cout << "zeile: " << x[i] << endl;
    }

    // subset
    rowinds.resize(2);
    rowinds[0]= 1;
    rowinds[1]= 3;
    tab.subset(rowinds, tab_filtered);
    for (unsigned int i=1; i<=tab_filtered.nrow(); i++) {
      cout << "ROW: " << tab_filtered.get_element(i,1) << " ... " <<
        tab_filtered.get_element(i,tab_filtered.ncol()) << endl;
    }



	} catch (except) {
    except e("main","Failed",__FILE__,__LINE__);
    e.print();
    return(1);
  }
  return(0);
}

