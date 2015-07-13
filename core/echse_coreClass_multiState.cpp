
#include "echse_coreClass_multiState.h"

////////////////////////////////////////////////////////////////////////////////
// Ctor & Dtor
////////////////////////////////////////////////////////////////////////////////

multiState::multiState() { }

void multiState::clear() {
  if (variables.size() > 0) {
    for (unsigned int i=0; i<variables.size(); i++) {
      variables[i]->clear();
      delete variables[i];
    }
    variables.clear();
  }
}

multiState::~multiState() {
  clear();
}

////////////////////////////////////////////////////////////////////////////////
// Return number of variables
////////////////////////////////////////////////////////////////////////////////
unsigned int multiState::nvars() const {
  return(variables.size());
}

////////////////////////////////////////////////////////////////////////////////
// Add a new variable (having values at 1...n locations)
////////////////////////////////////////////////////////////////////////////////

void multiState::add(const vector<double> &v) {
  variables.push_back(NULL);
  variables[variables.size()-1]= new(nothrow) vector<double>(v);
  if (!variables[variables.size()-1]) {
    except e(__PRETTY_FUNCTION__,"Memory allocation for new vector state variable failed.",__FILE__,__LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Read-access to value(s) of a variable
////////////////////////////////////////////////////////////////////////////////

const vector<double>& multiState::read_access(unsigned int indexVariable) const {
  if (indexVariable >= variables.size()) {
    stringstream errmsg;
    errmsg << "Cannot access vector state variable with index " << indexVariable <<
      ". Index out of range [0," << (variables.size()-1) << "].";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return(*variables[indexVariable]);
}

////////////////////////////////////////////////////////////////////////////////
// Full access to value(s) of a variable
////////////////////////////////////////////////////////////////////////////////

vector<double>& multiState::full_access(unsigned int indexVariable) {
  if (indexVariable >= variables.size()) {
    stringstream errmsg;
    errmsg << "Cannot access vector state variable with index " << indexVariable <<
      ". Index out of range [0," << (variables.size()-1) << "].";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return(*variables[indexVariable]);
}


////////////////////////////////////////////////////////////////////////////////
// Set values of an existing variable (with possibly changed dimension)
////////////////////////////////////////////////////////////////////////////////
/*
void multiState::set(unsigned int indexVariable, const vector<double> &v) {
  const string method="multiState::set";
  if (indexVariable >= variables.size()) {
    stringstream errmsg;
    errmsg << "Cannot set vector state variable with index " << indexVariable <<
      ". Index out of range [0," << (variables.size()-1) << "].";
    except e(method,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  variables[indexVariable]->assign(v.begin(), v.end());
}
*/


