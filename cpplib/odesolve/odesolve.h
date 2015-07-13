
#ifndef ODESOLVE_H
#define ODESOLVE_H

#include <iostream>
#include <string>
#include <valarray>
#include <vector>

#include "../except/except.h"
#include "../functions/functions.h"

using namespace std;

void solve_ode_nonStiff(
  valarray<double> &ystart,
  const double x1,
  const double x2,
  const double eps,
  const double h1,
  const double hmin,
  const valarray<double> &par,
  vector<tblFunction> &parfun,
  void(*derivs)(const double t, const valarray<double> &u,
    const valarray<double> &par, vector<tblFunction> &parfun,
    valarray<double> &dudt)
);


#endif

