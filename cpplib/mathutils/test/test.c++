
double interpol (const double x1, const double x2, const double y1,
  const double y2, const double xout)
{
  if (x1 <= x2) {
    if (xout < x1) return(y1);
    if (xout > x2) return(y2);
    if (x1 == x2) return(0.5*(y1+y2));
    return((xout-x1)/(x2-x1)*y2 + (x2-xout)/(x2-x1)*y1);
  } else {
    if (xout < x2) return(y2);
    if (xout > x1) return(y1);
    return((xout-x2)/(x1-x2)*y1 + (x1-xout)/(x1-x2)*y2);
  }
}


   #include <iostream>
   using namespace std;
   int main() {
     //                x1, x2, y1, y2, xout
     cout << interpol (0., 1., 4., 5., -1.) << endl;
     cout << interpol (0., 1., 4., 5., 0.) << endl;
     cout << interpol (0., 1., 4., 5., .25) << endl;
     cout << interpol (1., 0., 5., 4., .75) << endl;
     cout << interpol (0., 1., 4., 5., 1.) << endl;
     cout << interpol (0., 1., 4., 5., 2.) << endl;
     cout << interpol (0., 0., 4., 5., 1.) << endl;
   }

