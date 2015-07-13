
#ifndef TYPECONV_H
#define TYPECONV_H

#include <string>
#include <sstream>
#include <valarray>
#include <vector>
#include <typeinfo>
#include <iomanip>

#include <climits>
#include <cfloat>
#include <cerrno>

#include "../except/except.h"

using namespace std;

// TYPE-SPECIFIC FUNCTIONS /////////////////////////////////////////////////////

// Convert double to string after rounding to a specified number of digits
string numprint(double num, unsigned int n);

// Conversion to double
double       as_double           (const string &s);
double       as_double           (const int i);
double       as_double           (const unsigned int i);

// Conversion to int
int          as_integer          (const string &s);
int          as_integer          (const double d);
int          as_integer          (const unsigned int i);

// Conversion to unsigned int
unsigned int as_unsigned_integer (const string &s);
unsigned int as_unsigned_integer (const double d);
unsigned int as_unsigned_integer (const int i);

// Conversion to logical
bool as_logical (const string &s);
bool as_logical (const int i);
bool as_logical (const unsigned int i);

// Case folding for strings
string lowercase(string s);
string uppercase(string s);

// Conversion vector <--> valarray
void valarray2vector_double(const valarray<double> &va, vector<double> &ve);
void vector2valarray_double(const vector<double> &ve, valarray<double> &va);

// GENERIC FUNCTIONS ///////////////////////////////////////////////////////////

template <class T>
string as_string (const T& x) {
  string result;
  stringstream ss;
  ss << x;
  ss >> result;
  if (! ss.eof()) {
    result= "";	  
	}
  return(result);
}

// Conversion of vectors and valarrays (from any to any type)
// Here, we don't use a function that returns an array because
// - this would work only with vectors, not valarrays,
// - passing the result by reference (2nd argument) avoids unneccessary copying
//   of possibly large amounts of data
template <class T1, class T2>
void convert_type (const T1& in, T2& out) {
  unsigned int len= in.size();
  stringstream ss;
  if (len == 0) {
	  stringstream errmsg;
    errmsg << "Unable to convert array of type '" <<
      typeid(in).name() << "' to array of type '" << typeid(out).name() <<
      "'. Input array is empty.";
		except e (__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  out.resize(len);
	for (unsigned int i=0; i<len; i++) {
    ss << in[i];
    ss >> out[i];
	  if (! ss.eof()) {
			stringstream errmsg;
		  errmsg << "Unable to convert array of type '" <<
		    typeid(in).name() << "' to array of type '" << typeid(out).name() <<
        "'. Conversion failed" << " for element at position " << i+1 <<
        " of [1..." << len << "].";
			except e (__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		  throw(e);
    }
    ss.str("");  // Deletes the stream's content
    ss.clear();  // Deletes stream's state
	}
}

#endif

