
#ifndef MATHUTILS_H
#define MATHUTILS_H

////////////////////////////////////////////////////////////////////////////////
/// \fn ouble interpol (const double x1, const double x2, const double y1,
///   const double y2, const double xout)
/// \brief Linear interpolation with handling of special cases.
///
/// \param [in] x1, x2
///   Two x values. It does not matter which of the two values is greater.
/// \param [in] y1
///   The y-value corresponding to \c x1.
/// \param [in] y2
///   The y-value corresponding to \c x2.
/// \param [in] xout
///   The x-value for which y is to be found by linear interpolation.
/// 
/// \return
///   The value of y at \c xout.
///
/// \remarks
///   \li If the argument xout is outside the expected range x1...x2, the result
///       is the value at the nearest margin (y1 or y2, respectively).
///   \li If x1 == x2, the mean of y1 and y2 is returned.
///   \li It is ok to supply x1 > x2.
///
/// \par Example:
/// \code
///   #include <iostream>
///   using namespace std;
///   int main() {
///     //                x1, x2, y1, y2, xout
///     cout << interpol (0., 1., 4., 5., -1.) << endl;
///     cout << interpol (0., 1., 4., 5., 0.) << endl;
///     cout << interpol (0., 1., 4., 5., .25) << endl;
///     cout << interpol (1., 0., 5., 4., .75) << endl;
///     cout << interpol (0., 1., 4., 5., 1.) << endl;
///     cout << interpol (0., 1., 4., 5., 2.) << endl;
///     cout << interpol (0., 0., 4., 5., 0.) << endl;
///   }
/// \endcode
///
/// \author dkneis@uni-potsdam.de
////////////////////////////////////////////////////////////////////////////////

double interpol (const double x1, const double x2, const double y1,
  const double y2, const double xout);

#endif

