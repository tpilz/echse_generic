
#include "echse_coreFunct_solveODE.h"

// Routines adapted from "Numerical Recipes in Fortran", vol 1 & 2 (F77 & F90)
// 
// History
// - Adapted for use within the ECHSE in 2012.
// - Translated to C++ in 2010.
// - Adapted Fortran version in 2006/07.

////////////////////////////////////////////////////////////////////////////////
/// \fn void RKCK_fixedStepSize(
///   const vector<double> y,
///   const vector<double> dydx,  
///   const double x,             
///   const double h,             
///   abstractObject* objPtr,
///   const unsigned int delta_t,
///   const bool check_args,              
///   vector<double> &yout,
///   vector<double> &yerr
/// )
///
/// \brief Given values for n variables y and their derivatives dydx known at x,
/// use the fifth order Cash-Karp Runge-Kutta method to advance the solution over
/// an interval h and return the incremented variables as yout. Also return an
/// estimate of the local truncation error in yout using the embedded fourth order 
/// method. The user supplies the subroutine, which returns derivatives dydx at x.
/// (Cited from Numerical Recipes in F77/F90).
///
/// \par [in] y
///    Vector of states y at point x, i.e. y(x).
/// \par [in] dydx
///    Vector of the states' derivatives at x, i.e. dy(x)/dx.
/// \par [in] x
///    Value of x where y and dydx are known.
/// \par [in] h
///   Length of step along the x-axis.
/// \par [in] objPtr
///   Pointer to an object of class 'abstractObject'. This object must have
///   a method "derivsScal(const double t, const vector<double> &u,
///   vector<double> &dudt, const unsigned int delta_t)". Note that this is NOT a
///   const pointer, since the "derivsScal" method is not a const method too.
///   This is due to the fact that this method may call lookup functions, which
///   change the object's data (namely store the index of the last accessed
///   argument). The last argument "delta_t" is for cases where the length of
///   the integration time step is explicitly needed in the computation of
///   derivatives (conversion of a precipitation sum into a rate, for example).
/// \par [in] delta_t A time step length used by "derivsScal" for the purpose
///   of converting the unit of sum values into rates (precipitation, for expl).
/// \par [in] check_args
///   Switches basic argument checks on/off.
/// \par [out] yout
///   Estimates of the states y at x+h, i.e. y(x+h).
/// \par [out] yerr
///   Estimated error in yout.
///
/// \return
///   \c void
///
/// \remarks
///   \li This function is not to be called directly but by another function.
///   \li Changes in comparison with the original Fortran code: (1) Routines
///       from module "nrutil" have been replaced by in-line code. (2) Fortran
///       vector arithmetic expressions were replaced by loops. (3) Object pointer
///       is passed instead of function pointer (derivatives are computed in a
///       class method of the object). 
///   \li The function may thow an exception of class "except".
////////////////////////////////////////////////////////////////////////////////

void RKCK_fixedStepSize(
  const vector<double> y,
  const vector<double> dydx,  
  const double x,             
  const double h,             
  abstractObject* objPtr,
  const unsigned int delta_t,
  const bool check_args,
  vector<double> &yout,
  vector<double> &yerr
) {
  // Parameters of the Runge-Kutta method by Cash-Karp 
  const double A2=0.2, A3=0.3, A4=0.6 ,A5=1.0, A6=0.875, B21=0.2, B31=3.0/40.0,
    B32=9.0/40.0, B41=0.3, B42=-0.9, B43=1.2, B51=-11.0/54.0, B52=2.5, 
    B53=-70.0/27.0, B54=35.0/27.0, B61=1631.0/55296.0, B62=175.0/512.0, 
    B63=575.0/13824.0, B64=44275.0/110592.0, B65=253.0/4096.0, C1=37.0/378.0,
    C3=250.0/621.0, C4=125.0/594.0, C6=512.0/1771.0, DC1=C1-2825.0/27648.0,
    DC3=C3-18575.0/48384.0, DC4=C4-13525.0/55296.0, DC5=-277.0/14336.0,
    DC6=C6-0.25;
  // Local data
  const unsigned int ny= y.size();
  vector<double> ak2(ny), ak3(ny), ak4(ny), ak5(ny), ak6(ny), ytemp(ny);
  // Code
  if (check_args) {
    if ((dydx.size() != ny) || (yout.size() != ny) || (yerr.size() != ny)) {
      except e(__PRETTY_FUNCTION__,"Length of input vectors differs.",__FILE__,__LINE__);
      throw(e);
    }
  }
  try {
    // First step
    for (unsigned int i=0; i<ny; i++) ytemp[i]= y[i] + B21 * h * dydx[i];
    // Second step
    objPtr->derivsScal(x+A2*h, ytemp, ak2, delta_t);
    for (unsigned int i=0; i<ny; i++) ytemp[i]= y[i] + h * (B31 * dydx[i] + B32 * ak2[i]);
    // Third step
    objPtr->derivsScal(x+A3*h, ytemp, ak3, delta_t);
    for (unsigned int i=0; i<ny; i++) ytemp[i]= y[i] + h * (B41 * dydx[i] + B42 * ak2[i] + B43 * ak3[i]);
    // Fourth step
    objPtr->derivsScal(x+A4*h, ytemp, ak4, delta_t);
    for (unsigned int i=0; i<ny; i++) ytemp[i]= y[i] + h * (B51 * dydx[i] + B52 * ak2[i] + B53 * ak3[i] + B54 * ak4[i]);
    // Fifth step
    objPtr->derivsScal(x+A5*h, ytemp, ak5, delta_t);
    for (unsigned int i=0; i<ny; i++) ytemp[i]= y[i] + h * (B61 * dydx[i] + B62 * ak2[i] + B63 * ak3[i] + B64 * ak4[i] + B65 * ak5[i]);
    // Sixth step
    objPtr->derivsScal(x+A6*h, ytemp, ak6, delta_t);
    // Accumulate increments with proper weights
    for (unsigned int i=0; i<ny; i++) yout[i]= y[i] + h * (C1 * dydx[i] + C3 * ak3[i] + C4 * ak4[i] + C6 * ak6[i]);
    // Estimate error as difference between fourth and fifth order methods
    for (unsigned int i=0; i<ny; i++) yerr[i]= h * (DC1 * dydx[i] + DC3 * ak3[i] + DC4 * ak4[i] + DC5 * ak5[i] + DC6 * ak6[i]);
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Could not estimate states and errors.",__FILE__,__LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// \fn void RKCK_errDependStepSize(
///   vector<double> &y,
///   const vector<double> &dydx,  
///   double &x,             
///   const double htry,
///   const double eps,
///   const vector<double> &yscal,             
///   abstractObject* objPtr,
///   const unsigned int delta_t,
///   const bool check_args,
///   double &hdid,
///   double &hnext
/// )
///
/// \brief Fifth order Runge-Kutta step with monitoring of local truncation
/// error to ensure accuracy and adjust stepsize (actual stepsize is determined
/// automatically).
/// (Cited from Numerical Recipes in F77/F90).
///
/// \par [inout] y
///    Vector of states y at starting value of x, i.e. y(x). This values in this
///    vector are replaced by the new results.
/// \par [in] dydx
///    Vector of the states' derivatives at starting value of x, i.e. dy(x)/dx.
/// \par [inout] x
///    Starting value of the independend variable. The value is replaced by the
///    new value (x + hdid).
/// \par [in] htry
///   Step size to be attempted.
/// \par [in] eps
///   Required accuracy.
/// \par [in] yscal
///   Vector against which the error is scaled.
/// \par [in] objPtr
///   Pointer to an object of class 'abstractObject'. This object must have
///   a method "derivsScal(const double t, const vector<double> &u,
///   vector<double> &dudt, const unsigned int delta_t)". Note that this is NOT a
///   const pointer, since the "derivsScal" method is not a const method too.
///   This is due to the fact that this method may call lookup functions, which
///   change the object's data (namely store the index of the last accessed
///   argument). The last argument "delta_t" is for cases where the length of
///   the integration time step is explicitly needed in the computation of
///   derivatives (conversion of a precipitation sum into a rate, for example).
/// \par [in] delta_t A time step length used by "derivsScal" for the purpose
///   of converting the unit of sum values into rates (precipitation, for expl).
/// \par [in] check_args
///   Switches basic argument checks on/off.
/// \par [out] hdid
///   Stepsize that was actually accomplished.
/// \par [out] hnext
///   Estimated step size for the next step.
///
/// \return
///   \c void
///
/// \remarks
///   \li This function is not to be called directly but by another function.
///   \li Changes in comparison with the original Fortran code: (1) Routines
///       from module "nrutil" have been replaced by in-line code. (2) Fortran
///       vector arithmetic expressions were replaced by loops. (3) Fortran
///       sign() replaced by C's copysign(). (4) Object pointer
///       is passed instead of function pointer (derivatives are computed in a
///       class method of the object). 
///   \li The function may thow an exception of class "except".
////////////////////////////////////////////////////////////////////////////////

void RKCK_errDependStepSize(
  vector<double> &y,
  const vector<double> &dydx,  
  double &x,             
  const double htry,
  const double eps,
  const vector<double> &yscal,             
  abstractObject* objPtr,
  const unsigned int delta_t,
  const bool check_args,
  double &hdid,
  double &hnext
) {
  // Local data
  const double SAFETY=0.9, PGROW=-0.2, PSHRNK=-0.25, ERRCON=1.89e-4;
  const double ZERO= 0.0, ONE=1.0, TENTH=0.1, FIVE=5.0;
  const unsigned int ny= y.size();
  double errmax, h, htemp, xnew;
  vector<double> yerr(ny), ytemp(ny);
  // Code
  if (check_args) {
    if ((dydx.size() != ny) || (yscal.size() != ny)) {
      except e(__PRETTY_FUNCTION__,"Length of input vectors differs.",__FILE__,__LINE__);
      throw(e);
    }
  }
  try {
    // Set stepsize to the initial trial value
    h= htry;
    while(true) {
      // Take a step using 5th-order runge-kutta-cash-karp
      try {
        RKCK_fixedStepSize(y, dydx, x, h, objPtr, delta_t, check_args, ytemp, yerr);
      } catch (except) {
        except e(__PRETTY_FUNCTION__,"Calculation for fixed step size failed.",__FILE__,__LINE__);
        throw(e);
      }
      // Evaluate accuracy
      // The next lines replace the vector statement 'errmax= (abs(yerr/yscal)).max() / eps'
      errmax= ZERO;
      for (unsigned int i=0; i<ny; i++) errmax= max(errmax, abs(yerr[i]/yscal[i]));
      errmax= errmax / eps;

      // Exit loop if step succeeded
      if (errmax <= ONE) {
        break;
      }
      // Truncation error too large, reduce stepsize
      htemp= SAFETY * h * pow(errmax,PSHRNK);
      // No more than a factor of 10
      h= copysign(max(abs(htemp),TENTH*abs(h)),h);
      xnew= x+h; 
      if (xnew == x) {
        except e(__PRETTY_FUNCTION__,"Automatic step size dropped to zero.",__FILE__,__LINE__);
        throw(e);
      }
      // Go back for another try
    }
    // Compute size for the next step
    if (errmax > ERRCON) {
      hnext= SAFETY * h * pow(errmax,PGROW);
    } else {
      // No more than a factor of 5 increase
      hnext= FIVE * h;
    }
    hdid= h;
    x= x + h;
    y= ytemp;
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Failed.",__FILE__,__LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// \fn void RKCK(
///   const vector<double> &ystart,
///   const double x1,
///   const double x2,
///   const double eps,
///   const double h1,
///   const double hmin,
///   abstractObject* objPtr,
///   const unsigned int delta_t,
///   vector<double> &ynew
/// )
///
/// \brief Runge-Kutta driver with adaptive stepsize control. Integrate the
/// array of starting values ystart from x1 to x2 with accuracy eps.
/// (Cited from Numerical Recipes in F77/F90).
///
/// \par [in] ystart
///    Vector of dependent (state) variables y at starting value of x=x1.
/// \par [in] x1
///    Starting value of x.
/// \par [in] x2
///    Target value of x (upper limit of integration interval).
/// \par [in] eps
///   Required accuracy.
/// \par [in] h1
///   Guessed first step size.
/// \par [in] hmin
///   Minimum acceptable step size (can be zero).
/// \par [in] objPtr
///   Pointer to an object of class 'abstractObject'. This object must have
///   a method "derivsScal(const double t, const vector<double> &u,
///   vector<double> &dudt, const unsigned int delta_t)". Note that this is NOT a
///   const pointer, since the "derivsScal" method is not a const method too.
///   This is due to the fact that this method may call lookup functions, which
///   change the object's data (namely store the index of the last accessed
///   argument). The last argument "delta_t" is for cases where the length of
///   the integration time step is explicitly needed in the computation of
///   derivatives (conversion of a precipitation sum into a rate, for example).
/// \par [in] delta_t A time step length used by "derivsScal" for the purpose
///   of converting the unit of sum values into rates (precipitation, for expl).
/// \par [out] ynew
///    Values obtained at the end of the integration interval (x2).
///
/// \return
///   \c void
///
/// \remarks
///   \li Changes in comparison with the original Fortran code: (1) Routines
///       from module "nrutil" have been replaced by in-line code. (2) Fortran
///       vector arithmetic expressions were replaced by loops. (3) Fortran
///       sign() replaced by C's copysign(). (4) No storage of intermediate
///       results (part of code deleted). (5) Results returned in separate vector
///       (no overwriting of 'ystart').  (6) Object pointer
///       is passed instead of function pointer (derivatives are computed in a
///       class method of the object). 
///   \li The function may thow an exception of class "except".
////////////////////////////////////////////////////////////////////////////////

void RKCK(
  const vector<double> &ystart,
  const double x1,
  const double x2,
  const double eps,
  const double h1,
  const double hmin,
  abstractObject* objPtr,
  const unsigned int delta_t,
  vector<double> &ynew
) {
  // Local data
  const double TINY=1.0e-30;
  const double ZERO=0.0;
  const unsigned int MAXSTP=10000;
  const unsigned int ny= ystart.size();
  double h, hdid, hnext, x;
  vector<double> dydx(ny), y(ny), yscal(ny);
  bool ok;
  // Code
  try {
    // Initializations
    x= x1;
    h= copysign(h1, x2-x1);
    y= ystart;
    // Take at most MAXSTP steps
    ok= false;
    for (unsigned int nstp=1; nstp<=MAXSTP; nstp++) {
      // Compute derivatives at the start of the step
      objPtr->derivsScal(x, y, dydx, delta_t);
      // Scaling used to monitor accuracy. This general purpose choice can be modified.
      for (unsigned int i=0; i<ny; i++) yscal[i]= abs(y[i]) + abs(h * dydx[i]) + TINY;
      // If stepsize can overshoot, decrease
      if (((x+h-x2)*(x+h-x1)) > ZERO) {
        h= x2-x;
      }
      // Make a step with error dependend stepsize
      try {
        RKCK_errDependStepSize(y, dydx, x, h, eps, yscal, objPtr, delta_t, false, hdid, hnext);
      } catch (except) {
        except e(__PRETTY_FUNCTION__,"Exception in subroutine.",__FILE__,__LINE__);
        throw(e);
      }
      // Are we done?
      if (((x-x2)*(x2-x1)) >= ZERO) {
        ynew= y;
        ok= true;
        break;
      }
      if (abs(hnext) < hmin) {
        stringstream errmsg;
        errmsg << "Stepsize in Runge-Kutta ODE solver reached lower limit" <<
          " of " << hmin << ".";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      h= hnext;
    }
    if (!ok) {
      stringstream errmsg;
      errmsg << "Runge-Kutta ODE solver could not find a solution of the" <<
        " requested accuracy within " << MAXSTP << " integration steps.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
  } catch (except) {
    stringstream errmsg;
    errmsg << "Could not integrate ODE system of size " << y.size() <<
      " over time step of length " << x2-x1 << ".";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}


////////////////////////////////////////////////////////////////////////////////
// Simple explicit Euler integration of state variable(s).
//
// ATTENTION: Method is not very accurate, nor stable. You should use this method
// only for very(!) simple ODEs.
////////////////////////////////////////////////////////////////////////////////
void euler_ex(
	const vector<double> &ystart,
	const unsigned int delta_t,
	abstractObject* objPtr,
	vector<double> &ynew
) {
	// local data
	const unsigned int ny= ystart.size();
	vector<double> dydx(ny);
	
	// compute derivatives
	objPtr->derivsScal(0., ystart, dydx, delta_t);
	// update states
	for (unsigned int i=0; i<ny; i++)
		ynew[i] = ystart[i] + dydx[i] * delta_t;
}


////////////////////////////////////////////////////////////////////////////////
// Fourth order explicit Runge-Kutta method.
//
// Apated from Press et al. (2002), v. 2.11, eq. 16.1.3
//
// Classical, often used method. However, this version contains no error control
// nor adaptive step size control. It should only be used for simle problems or
// experimentation.
////////////////////////////////////////////////////////////////////////////////
void rk4(
	const vector<double> &y,
	const double x,
	const double h,
	abstractObject* objPtr,
	vector<double> &ynew
) {
	// local data
	unsigned int i;
	double xh,hh,h6;

	int n=y.size();
	vector<double> dydx(n),dym(n),dyt(n),yt(n);
	hh=h*0.5;
	h6=h/6.0;
	xh=x+hh;
	
	// first step
	objPtr->derivsScal(x, y, dydx, h);
	
	// second step
	for (i=0;i<n;i++) yt[i]=y[i]+hh*dydx[i];
	objPtr->derivsScal(xh, yt, dyt, h);
	
	// third step
	for (i=0;i<n;i++) yt[i]=y[i]+hh*dyt[i];
	objPtr->derivsScal(xh, yt, dym, h);
	
	for (i=0;i<n;i++) {
		yt[i]=y[i]+h*dym[i];
		dym[i] += dyt[i];
	}
	
	// fourth step
	objPtr->derivsScal(x+h, yt, dyt, h);
	
	// accumulate increments with proper weights
	for (i=0;i<n;i++)
		yout[i]=y[i]+h6*(dydx[i]+dyt[i]+2.0*dym[i]);
}


////////////////////////////////////////////////////////////////////////////////
// Second order Runge-Kutta (midpoint) method.
//
// Adapted from Press et al. (2002), v. 2.11, eq. 16.1.2
//
// ATTENTION: Method is not very accurate, no accuracy check included. Use for
// simple problems only!
////////////////////////////////////////////////////////////////////////////////
void rk2(
	const vector<double> &ystart,
	const double x1,
	const unsigned int delta_t,
	abstractObject* objPtr,
	vector<double> &ynew
) {
	// local data
	const unsigned int ny= ystart.size();
	vector<double> dydx(ny), ymid(ny);
	
	// compute derivatives at start point and integrate to midpoint
	objPtr->derivsScal(x1, ystart, dydx, delta_t);
	for (unsigned int i=0; i<ny; i++)
		ymid[i] = ystart[i] + 0.5 * dydx[i] * delta_t;
	
	// compute derivatives at midpoint and integrate to endpoint
	objPtr->derivsScal(x1 + 0.5*delta_t, ymid, dydx, delta_t);
	for (unsigned int i=0; i<ny; i++)
		ynew[i] = ystart[i] + dydx[i] * delta_t;
}


////////////////////////////////////////////////////////////////////////////////
// Simple implicit Euler integration of state variable(s).
//
// ATTENTION: Method can handle simple stiff ODEs but is very inefficient. You should
// use this method only for simple ODEs. For complex problems it might fail.
////////////////////////////////////////////////////////////////////////////////
void euler_im(
	const vector<double> &ystart,
	const unsigned int delta_t,
	abstractObject* objPtr,
	const double eps,
	const unsigned int nmax,
	vector<double> &ynew
) {
	// local data
	const unsigned int ny= ystart.size();
	vector<double> dydx(ny), yini(ny), yimprove(ny);
	double errmax;
	unsigned int n = 0;
	
	// compute derivatives and initial estimate of states at end of time step (explicit Euler)
	objPtr->derivsScal(0., ystart, dydx, delta_t);
	for (unsigned int i=0; i<ny; i++)
		yini[i] = ystart[i] + dydx[i] * delta_t;
	
	// iterate over maximum of nmax steps until solution of eps accuracy is found; throw exception if this fails
	while(true) {
		n += 1;
		// use estimated state at end of time step to compute derivative and improved state estimate
		objPtr->derivsScal(0., yini, dydx, delta_t);
		for (unsigned int i=0; i<ny; i++)
			yimprove[i] = ystart[i] + dydx[i] * delta_t;
		
		// check accuracy (deviation between initial and improved estimate should be small enough)
		errmax = 0.;
		for (unsigned int i=0; i<ny; i++)
			errmax = max(errmax, abs(yini[i] - yimprove[i]));
		errmax = errmax / eps;

		// Exit loop if step succeeded
		if (errmax <= 1.) {
			ynew = yimprove;
			break;
		}
		
		// not more than specified maximum number of sub steps
		if(n == nmax) {
			stringstream errmsg;
			errmsg << "Implicit Euler integration failed! Could not find a solution of" <<
				" requested accuracy " << eps << " within " << nmax << " integration steps.";
			except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
			throw(e);
		}
			
		// set improved state estimate to new initial value
		yini = yimprove;
	}

}


////////////////////////////////////////////////////////////////////////////////
// Wrapper for use within the 'simulate' method of a class
//
// A call within 'simulate' should look like this:
// 
// odesolve_nonstiff(
//   stateScal_all(),    // ystart:  The initial values of the state variables
//   delta_t,            // delta_t: The length of the time step
//   1.0e-06,            // eps:     The desired accuracy
//   100,                // nmax:    The maximum number of sub-time steps
//   this,               // objPtr:  Pointer to the current object
//   set_stateScal_all() // ynew:    The updated values of the state variables
//   1                   // choice:  Choice flag of method for numerical integration
// )

////////////////////////////////////////////////////////////////////////////////

void odesolve_nonstiff(
  const vector<double> &ystart,
  const unsigned int delta_t,
  const double eps,
  const unsigned int nmax,
  abstractObject* objPtr,
  vector<double> &ynew,
	const int choice
) {
	switch(choice) {
		case 1: // Simple explicit Euler integration
			euler_ex(
				ystart,
				delta_t,
				objPtr,
				ynew
			);
			break;
			
		case 2: // Second order explicit Runge-Kutta (midpoint) method
			rk2(
				ystart,
				0.,
				delta_t,
				objPtr,
				ynew
			);
			break;
			
		case 3: // Fourth order explicit Runge-Kutta method
			rk4(
				ystart,
				0.,
				delta_t,
				objPtr,
				ynew
			);
			break;
			
		case 4: // Fifth order Cash-Karp Runge-Kutta; adaptive step size control
			RKCK(
				ystart,
				0.,                                        // x1
				static_cast<double>(delta_t),              // x2
				eps,
				static_cast<double>(delta_t),              // h1
				delta_t/max(1.,static_cast<double>(nmax)), // hmin
				objPtr,
				delta_t,
				ynew
			);
			break;
			
		case 5: // Implicit Euler method
			euler_im(
				ystart,
				delta_t,
				objPtr,
				eps,
				nmax,
				ynew
			);
			break;
			
		default :
			stringstream errmsg;
      errmsg << "Invalid choice of numerical integration method! Currently supported is one of {1,2,3,4}.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
	}
}

