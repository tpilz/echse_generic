
#include <iostream>

#include "../except.h"

using namespace std;

/*******************************************************************************
Test functions where errors may occur
*******************************************************************************/

// On error, this function throws an exception.
void func1(int k) {
  string funcname= "func1";
  stringstream errmsg;
  if (k<0) {
    errmsg << "Value of k is " << k << " but must not be < 0.";
    except e(funcname,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}

// On error, this function does not throw an exception but it sets a return code
int func2(int k) {
  string funcname= "func2";
  if (k>0) {
		return(1);
  }
  return(0);
}

/*******************************************************************************
Main program to test exception handling
*******************************************************************************/

int main () {
  string funcname="main";
  int i, k;

  cout << "Wert: ";
  cin >> k;

  // Try block around main
  try {

		// Call the function that may throw an exception
		try {
			func1(k);
		}
		catch (except) {
			except e(funcname,"Func returned error.",__FILE__,__LINE__);
			throw(e);
		}

		// Call the function that uses a return code instead of exceptions
		i= func2(k);
		if (i) {
			except e(funcname,"Func returned error.",__FILE__,__LINE__);
			throw(e);
		}

	}
  // Error handling at the level of main
  catch (except) {
    except e(funcname,"Program terminated due to error.",__FILE__,__LINE__);
    e.send_to_stderr();
    e.clear();
    return(1);
  }
  return(0);
}

