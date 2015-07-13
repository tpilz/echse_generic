
#ifndef ECHSE_COREFUNCT_SOLVEODE_H
#define ECHSE_COREFUNCT_SOLVEODE_H

#include <vector>

#include "echse_coreClass_abstractObject.h"

#include "except/except.h"

using namespace std;

// Interface of the ODE solver -- See the .cpp file for documentation
void odesolve_nonstiff(
  const vector<double> &ystart,
  const unsigned int delta_t,
  const double eps,
  const unsigned int nmax,
  abstractObject* objPtr,
  vector<double> &ynew
);

#endif

