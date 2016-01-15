
#ifndef ECHSE_COREFUNCT_SOLVEODE_H
#define ECHSE_COREFUNCT_SOLVEODE_H

#include <vector>

#include "echse_coreClass_abstractObject.h"

#include "except/except.h"

// gsl (GNU scientific library) with ode solvers and error handling
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_machine.h>

using namespace std;

// Interface of the ODE solver -- See the .cpp file for documentation
void odesolve_nonstiff(
  const vector<double> &ystart,
  const unsigned int delta_t,
  const double eps,
  const unsigned int nmax,
  abstractObject* objPtr,
  vector<double> &ynew,
	const int choice
);

#endif

