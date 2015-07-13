#include<iostream>
#include<string>
#include<valarray>

#include "../../except/except.h"
#include "../../table/table.h"
#include "../../typeconv/typeconv.h"

#include "../functions.h"

using namespace std;

int main() {

  try {
		valarray<string> x_str, y_str;

		// Read table
		table tab;
		tab.read("/home/dkneis/cpp/dklib/functions/test/testfunc.txt", true, "\t", "#");
		tab.get_col("x", x_str);
		tab.get_col("y", y_str);

		// Put into function
		function fun;
		fun.init("myfunc",as_double(x_str), as_double(y_str));

		// Test function
		double dx= (fun.maxarg() - fun.minarg()) / 10.;
    double x;
		x= fun.maxarg();
		while (x >= fun.minarg()) {
		  cout << "x= " << x << " f(x)= " << fun.eval(x) << endl;
		  x= x-dx;
		}
    cout << "x= " << x << " diff= " << fun.maxarg()-x << endl;
	} catch (except) {
		except e ("main", "Failed.", __FILE__, __LINE__);
    e.print();
    return(1);
  }
  return(0);
}

