
#include "odesolve.h"

// Routines adapted from "Numerical Recipes in Fortran", vol 1 & 2 (F77 & F90)
// 
// History
// - Adapted Fortran version in 2006/07.
// - Translated to C++ in 2010.

////////////////////////////////////////////////////////////////////////////////
/// \fn void RKCK_fixedStepSize(
///   const valarray<double> y,
///   const valarray<double> dydx,  
///   const double x,             
///   const double h,             
///   const valarray<double> &par,
///   vector<tblFunction> &parfun,   
///   void(*derivs)(const double t, const valarray<double> &u,
///     const valarray<double> &par, vector<tblFunction> &parfun,
///     valarray<double> &dudt),
///   const bool check_args,              
///   valarray<double> &yout,
///   valarray<double> &yerr
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
/// \par [in] par
///   Vector of parameters passed to the function which computes the
///   derivatives of y with respect to x.
/// \par [inout] parfun
///   Vector of functions passed to the function which computes the
///   derivatives of y with respect to x. This is not a const, because data
///   members of a function object (namely the index of the argument which has
///   been accessed last) may be updated when the function is called.
/// \par [in] derivs
///   Pointer to the function which computes the derivatives of y with
///   respect to x.
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
///   \li Functions calls "derivs" to computes the righthand side of the ODEs.
///   \li Changes in comparison with the original Fortran code: (1) The function
///       'derivs' that computes the righthand sides of the derivatives uses an
///       additional input array which can be used to pass values for parameters
///       which appear in the derivatives. (2) Routines from module "nrutil"
///       have been replaced by in-line code. (3) Use of valarray<double> as
///       array type.
///   \li The function may thow an exception of class "except".
////////////////////////////////////////////////////////////////////////////////

void RKCK_fixedStepSize(
  const valarray<double> y,
  const valarray<double> dydx,  
  const double x,             
  const double h,             
  const valarray<double> &par,
  vector<tblFunction> &parfun,
  void(*derivs)(const double t, const valarray<double> &u,
    const valarray<double> &par, vector<tblFunction> &parfun,
    valarray<double> &dudt),
  const bool check_args,
  valarray<double> &yout,
  valarray<double> &yerr
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
  valarray<double> ak2(ny), ak3(ny), ak4(ny), ak5(ny), ak6(ny), ytemp(ny);
  // Code
  if (check_args) {
    if ((dydx.size() != ny) || (yout.size() != ny) || (yerr.size() != ny)) {
      except e(__PRETTY_FUNCTION__,"Length of input vectors differs.",__FILE__,__LINE__);
      throw(e);
    }
  }
  try {
    // First step
    ytemp= y + B21 * h * dydx;
    // Second step
    derivs(x+A2*h, ytemp, par, parfun, ak2);
    ytemp= y + h * (B31 * dydx + B32 * ak2);
    // Third step
    derivs(x+A3*h, ytemp, par, parfun, ak3);
    ytemp= y + h * (B41 * dydx + B42 * ak2 + B43 * ak3);
    // Fourth step
    derivs(x+A4*h, ytemp, par, parfun, ak4);
    ytemp= y + h * (B51 * dydx + B52 * ak2 + B53 * ak3 + B54 * ak4);
    // Fifth step
    derivs(x+A5*h, ytemp, par, parfun, ak5);
    ytemp= y + h * (B61 * dydx + B62 * ak2 + B63 * ak3 + B64 * ak4 + B65 * ak5);
    // Sixth step
    derivs(x+A6*h, ytemp, par, parfun, ak6);
    // Accumulate increments with proper weights
    yout= y + h * (C1 * dydx + C3 * ak3 + C4 * ak4 + C6 * ak6);
    // Estimate error as difference between fourth and fifth order methods
    yerr= h * (DC1 * dydx + DC3 * ak3 + DC4 * ak4 + DC5 * ak5 + DC6 * ak6);
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Could not estimate states and errors.",__FILE__,__LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// \fn void RKCK_errDependStepSize(
///    valarray<double> &y,
///    const valarray<double> &dydx,  
///    double &x,             
///    const double htry,
///    const double eps,
///    const valarray<double> &yscal,             
///    const valarray<double> &par,
///    vector<tblFunction> &parfun,
///    void(*derivs)(const double t, const valarray<double> &u,
///      const valarray<double> &par, vector<tblFunction> &parfun,
///      valarray<double> &dudt),
///    const bool check_args,
///    double &hdid,
///    double &hnext
///  )
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
/// \par [in] par
///   Vector of parameters passed to the function which computes the
///   derivatives of y with respect to x.
/// \par [inout] parfun
///   Vector of functions passed to the function which computes the
///   derivatives of y with respect to x. This is not a const, because data
///   members of a function object (namely the index of the argument which has
///   been accessed last) may be updated when the function is called.
/// \par [in] derivs
///   Pointer to the function which computes the derivatives of y with
///   respect to x.
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
///   \li Changes in comparison with the original Fortran code: (1) The function
///       'derivs' that computes the righthand sides of the derivatives uses an
///       additional input array which can be used to pass values for parameters
///       which appear in the derivatives. (2) Routines from module "nrutil"
///       have been replaced by in-line code. (3) Fortran sign() replaced by
///       C's copysign(). (4) Use of valarray<double> as array type.
///   \li The function may thow an exception of class "except".
////////////////////////////////////////////////////////////////////////////////

void RKCK_errDependStepSize(
  valarray<double> &y,
  const valarray<double> &dydx,  
  double &x,             
  const double htry,
  const double eps,
  const valarray<double> &yscal,             
  const valarray<double> &par,
  vector<tblFunction> &parfun,
  void(*derivs)(const double t, const valarray<double> &u,
    const valarray<double> &par, vector<tblFunction> &parfun,
    valarray<double> &dudt),
  const bool check_args,
  double &hdid,
  double &hnext
) {
  // Local data
  const double SAFETY=0.9, PGROW=-0.2, PSHRNK=-0.25, ERRCON=1.89e-4;
  const double ONE=1.0, TENTH=0.1, FIVE=5.0;
  const unsigned int ny= y.size();
  double errmax, h, htemp, xnew;
  valarray<double> yerr(ny), ytemp(ny);
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
        RKCK_fixedStepSize(y, dydx, x, h, par,  parfun, derivs, check_args,
          ytemp, yerr);
      } catch (except) {
        except e(__PRETTY_FUNCTION__,"Calculation for fixed step size failed.",__FILE__,__LINE__);
        throw(e);
      }
      // Evaluate accuracy
      errmax= (abs(yerr/yscal)).max() / eps;
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
/// \fn void solve_ode_nonStiff(
///   valarray<double> &ystart,
///   const double x1,
///   const double x2,
///   const double eps,
///   const double h1,
///   const double hmin,
///   const valarray<double> &par,
///   vector<tblFunction> &parfun,
///   void(*derivs)(const double t, const valarray<double> &u,
///     const valarray<double> &par, vector<tblFunction> &parfun,
///     valarray<double> &dudt)
/// )
///
/// \brief Runge-Kutta driver with adaptive stepsize control. Integrate the
/// array of starting values ystart from x1 to x2 with accuracy eps.
/// (Cited from Numerical Recipes in F77/F90).
///
/// \par [inout] ystart
///    Vector of dependent (state) variables y at starting value of x=x1. The
///    values of this vector are replaced by the result values obtained at
///    the end of the integration interval (x2).
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
/// \par [in] par
///   Vector of parameters passed to the function which computes the
///   derivatives of y with respect to x.
/// \par [inout] parfun
///   Vector of functions passed to the function which computes the
///   derivatives of y with respect to x. This is not a const, because data
///   members of a function object (namely the index of the argument which has
///   been accessed last) may be updated when the function is called.
/// \par [in] derivs
///   Pointer to the function which computes the derivatives of y with
///   respect to x.
///
/// \return
///   \c void
///
/// \remarks
///   \li Changes in comparison with the original Fortran code: (1) The function
///       'derivs' that computes the righthand sides of the derivatives uses an
///       additional input array which can be used to pass values for parameters
///       which appear in the derivatives. (2) Routines from module "nrutil"
///       have been replaced by in-line code. (3) Fortran sign() replaced by
///       C's copysign(). (4) Use of valarray<double> as array type. (5) No
///       storage of intermediate results (part of code deleted).
///   \li The function may thow an exception of class "except".
////////////////////////////////////////////////////////////////////////////////

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
) {
  // Local data
  const double TINY=1.0e-30;
  const double ZERO=0.0;
  const unsigned int MAXSTP=10000;
  const unsigned int ny= ystart.size();
  double h, hdid, hnext, x;
  valarray<double> dydx(ny), y(ny), yscal(ny);
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
      derivs(x, y, par, parfun, dydx);
      // Scaling used to monitor accuracy. This general purpose choice can be modified.
      yscal= abs(y) + abs(h * dydx) + TINY;
      // If stepsize can overshoot, decrease
      if (((x+h-x2)*(x+h-x1)) > ZERO) {
        h= x2-x;
      }
      // Make a step with error dependend stepsize
      try {
        RKCK_errDependStepSize(y, dydx, x, h, eps, yscal, par, parfun, derivs,
          false, hdid, hnext);
      } catch (except) {
        except e(__PRETTY_FUNCTION__,"Exception in subroutine.",__FILE__,__LINE__);
        throw(e);
      }
      // Are we done?
      if (((x-x2)*(x2-x1)) >= ZERO) {
        ystart= y;
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


