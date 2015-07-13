
#include "typeconv.h"

// Convert double to string after rounding to a specified number of digits
string numprint(double num, unsigned int n) {
  stringstream ss;
//  ss << fixed << round(num*pow(10,n)) / pow(10,n);   // "fixed" is an IO modifier
  ss << setprecision(n);
  ss << fixed << num;
  return(ss.str());
}

////////////////////////////////////////////////////////////////////////////////
// Conversion to double
////////////////////////////////////////////////////////////////////////////////

// from string
double as_double (const string &s) {
	char *stopChar;
  double val;
  const double ZERO= 0.0;
  if (s.length() == 0) {
    stringstream errmsg;
    errmsg << "Cannot convert zero-length string to double.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }  
  errno = 0;
  val = strtod(s.c_str(), &stopChar);
  if (errno == ERANGE) {
    // Case 1: Underflow (i.e. precision loss) --> Assume zero
    if (fabs(val) < DBL_MIN) {
        val= ZERO;
    // Case 2: Positive or negative overflow --> Exception
    } else if (fabs(val) == HUGE_VAL) {
      stringstream errmsg;
      errmsg << "Cannot convert string '" << s << "' to double." <<
        " Value is out of the range of representable values.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    // Case 3: Undefined behavior --> Exception
    } else {
      stringstream errmsg;
      errmsg << "Cannot convert string '" << s << "' to double. Unknown conversion error.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
	}
  if (*stopChar) {
    stringstream errmsg;
    errmsg << "Cannot convert string '" << s << "' to double. Illegal sequence of characters.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
	}
  return(val);
}
// from int
double as_double (const int i) {
  return(static_cast<double>(i));
}
// from uint
double as_double (const unsigned int i) {
  return(static_cast<double>(i));
}
// from longint
double as_double (const long int i) {
  return(static_cast<double>(i));
}

////////////////////////////////////////////////////////////////////////////////
// Conversion to signed integer
////////////////////////////////////////////////////////////////////////////////

// from string
int as_integer (const string &s) {
	char *stopChar;
  long int val;
  if (s.length() == 0) {
    stringstream errmsg;
    errmsg << "Cannot convert zero-length string to integer.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }  
  errno = 0;
  val = strtol(s.c_str(), &stopChar, 10);
  if (errno == ERANGE) {
    stringstream errmsg;
    errmsg << "Cannot convert string '" << s << "' to integer. Under-/Overflow occurred.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
	}
  if (*stopChar) {
    stringstream errmsg;
    errmsg << "Cannot convert string '" << s << "' to integer. Illegal sequence of characters.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
	}
  if ((val > static_cast<long int>(INT_MAX)) | (val < static_cast<long int>(INT_MIN))) {  // Does longint fit into int?
    stringstream errmsg;
    errmsg << "Cannot convert string '" << s << "' to integer. Under-/Overflow occurred.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return(static_cast<int>(val));
}
// from uint
int as_integer (const unsigned int i) {
  if (i > static_cast<unsigned int>(INT_MAX)) {  // Does uint fit into int?
    stringstream errmsg;
    errmsg << "Cannot convert unsigned integer '" << i << "' to integer. Under-/Overflow occurred.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return(static_cast<int>(i));
}
// from double
int as_integer (const double d) {
  if ((d > INT_MAX) | (d < INT_MIN)) {  // Does double fit into int?
    stringstream errmsg;
    errmsg << "Cannot convert double '" << d << "' to integer. Under-/Overflow occurred.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return(static_cast<int>(d));
}

////////////////////////////////////////////////////////////////////////////////
// Conversion to unsigned integer
////////////////////////////////////////////////////////////////////////////////

// from string
unsigned int as_unsigned_integer (const string &s) {
	int val;
  try {
    val= as_integer(s);  // Convert to signed int first
  } catch (except) {
    stringstream errmsg;
    errmsg << "Cannot convert string '" << s << "' to unsigned integer. Conversion to integer failed.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  if (val < 0) {   // Does int fit into uint?
    stringstream errmsg;
    errmsg << "Cannot convert string '" << s << "' to unsigned integer. Under-/Overflow occurred.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return(static_cast<unsigned int>(val));
}
// from uint
unsigned int as_unsigned_integer (const int i) {
  if (i < 0) {  // Does int fit into uint?
    stringstream errmsg;
    errmsg << "Cannot convert integer '" << i << "' to unsigned integer. Under-/Overflow occurred.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return(static_cast<unsigned int>(i));
}
// from double
unsigned int as_unsigned_integer (const double d) {
  if ((d > UINT_MAX) | (d < 0)) {  // Does double fit into uint?
    stringstream errmsg;
    errmsg << "Cannot convert double '" << d << "' to unsigned integer. Under-/Overflow occurred.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return(static_cast<unsigned int>(d));
}

////////////////////////////////////////////////////////////////////////////////
// Conversion to logical values
////////////////////////////////////////////////////////////////////////////////

bool as_logical (const string &s) {
  string tmp= lowercase(s);
  if ((tmp == "t") || (tmp == "true")) {
    return(true);
  } else if ((tmp == "f") || (tmp == "false")) {
    return(false);
  } else {
	  stringstream errmsg;
    errmsg << "Unable to convert string '" << s << "' to logical value.";
		except e (__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}
bool as_logical (const int i) {
  if (i != 0) return(true);
  return(false);
}
bool as_logical (const unsigned int i) {
  if (i != 0) return(true);
  return(false);
}

////////////////////////////////////////////////////////////////////////////////
// Case folding for strings
////////////////////////////////////////////////////////////////////////////////

string lowercase(string s) {
  for (unsigned int i=0; i<s.length(); i++) s[i]= tolower(s[i]);
  return(s);
}
string uppercase(string s) {
  for (unsigned int i=0; i<s.length(); i++) s[i]= toupper(s[i]);
  return(s);
}

////////////////////////////////////////////////////////////////////////////////
// Conversion vector <--> valarray
////////////////////////////////////////////////////////////////////////////////

void valarray2vector_double(const valarray<double> &va, vector<double> &ve) {
  unsigned int n=va.size();
  ve.resize(n);
  for (unsigned int i=0; i<n; i++) ve[i]= va[i];
}
void vector2valarray_double(const vector<double> &ve, valarray<double> &va) {
  unsigned int n=ve.size();
  va.resize(n);
  for (unsigned int i=0; i<n; i++) va[i]= ve[i];
}


