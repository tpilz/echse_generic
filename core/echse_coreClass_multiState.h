
#ifndef ECHSE_CORECLASS_MULTISTATE_H
#define ECHSE_CORECLASS_MULTISTATE_H

#include <vector>
#include <sstream>
#include <string>

#include "except/except.h"
#include "table/table.h"

using namespace std;

class multiState {
  private:
    vector< vector<double>* > variables;
  public:
    // Ctor, Dtor
    multiState ();
    ~multiState ();
    // Clear method
    void clear();
    // Return number of variables
    unsigned int nvars() const;
    // Get & set methods
    void add(const vector<double> &v);
    const vector<double>& read_access(unsigned int indexVariable) const;
    vector<double>& full_access(unsigned int indexVariable);
};

#endif

