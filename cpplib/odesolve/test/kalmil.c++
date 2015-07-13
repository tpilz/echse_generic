#include<iostream>
#include<fstream>
#include<string>
#include<valarray>
#include<vector>

#include "../../except/except.h"
#include "../../typeconv/typeconv.h"
#include "../../system/system.h"
#include "../../table/table.h"
#include "../../functions/functions.h"

#include "../odesolve.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

// Derivatives function for cascade of linear reservoirs
// Notes:  (1) Parameters passed in array 'par' with
//             - par[0] = inflow rate for 1st reservoir
//             - par[1] = storage constante for all reservoirs
//         (2) No functions passed in array 'parfun'
//         (3) State variables (u) represent the volumes
//         (4) Arrays u and dudt must be of equal length         
void derivs (const double t, const valarray<double> &u,
  const valarray<double> &par, vector<function> &parfun,
  valarray<double> &dudt
) {
  // Local data
  const unsigned int n= u.size();
  // 1st reservoir
  dudt[0]= par[0] - u[0]/par[1];
  // Further reservoirs
  if (n > 1) {
    for (unsigned int i=1; i<n; i++) {  
      dudt[i] = u[i-1]/par[1] - u[i]/par[1];
    }
  }
}

// Storage volume of a reservoir at constant flow q 
double vol_steadystate (const double k, const double q) {
  return(q * k);
}

////////////////////////////////////////////////////////////////////////////////


int main() {

  // Constants
  const string method="main";

  // Number of linked linear reservoirs
	const unsigned int numreserv= 10;
  // Retention constant of a single reservoir
	const double retconst=  1000.;
  // Time step
  const double dt= 900.;
  // File with inflow hydrograph
  const string file_inflow="/home/dkneis/cpp/dklib/odesolve/test_kalinin-miljukov/q_in.txt";
  valarray<double> q_in;

  // Parameters of the numerical ODE solver
  const double eps= 1.0e-06;
  const double h1= dt;
  const double hmin= dt/100.;

  // Output
  const string file_outflow="/home/dkneis/cpp/dklib/odesolve/test_kalinin-miljukov/q_out.txt";
  ofstream ostr;

  // Volume of the reservoirs
  valarray<double> v;

  // Array of parameters
  valarray<double> par(2);
  vector<function> parfun(0);

  //////////////////////////////////////////////////////////////////////////////

  try {

    // Read inflow
    table q_tab;
    q_tab.read(file_inflow,true,"\t ","#");
    q_in.resize(q_tab.nrow());
    vector<string> strings;
    q_tab.get_col(q_tab.colindex("q_in"),strings);
    convert_type(strings, q_in);
    strings.resize(0);
    q_tab.clear();

    // Initialize the states
    v.resize(numreserv);
    v= vol_steadystate(retconst, q_in[0]);

    // Initialize parameters
    par[0]= q_in[0];
    par[1]= retconst;

    // Open output
    if (file_exists(file_outflow)) {
		  except e(method,"Output file already exists.",__FILE__,__LINE__);
      throw(e);
    }
    ostr.open(file_outflow.c_str());
    if (!ostr) {
		  except e(method,"Could not open output file.",__FILE__,__LINE__);
      throw(e);
    }

    // Output header
    ostr << "time";
    for (unsigned int k=0; k<numreserv; k++) {
      ostr << "\t" << "v" << (k+1);
    }
    ostr << endl;

    // Output initial state
    ostr << 0.;
    for (unsigned int k=0; k<numreserv; k++) {
      ostr << "\t" << v[k];
    }
    ostr << endl;

    // Compute the states for all time steps
    for (unsigned int i=0; i<q_in.size(); i++) {
      // Update boundary conditions
      par[0]= q_in[i];
      cout << "Time step: " << i+1 << " q_in= " << par[0] << endl;
      // Integrate
      solve_ode_nonStiff(v, (i*dt), ((i+1)*dt), eps, h1, hmin,
        par, parfun, derivs);
      // Output
      ostr << ((i+1)*dt);
      for (unsigned int k=0; k<numreserv; k++) {
        ostr << "\t" << v[k];
      }
      ostr << endl;
    }

    
	} catch (except) {
		except e ("main", "Terminated due to error.", __FILE__, __LINE__);
    e.print();
    return(1);
  }
  return(0);
}

