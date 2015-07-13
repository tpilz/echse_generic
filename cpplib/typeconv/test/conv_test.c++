
#include <string>
#include <iostream>

#include "/home/dkneis/cpp/dklib/except/except.h"
#include "/home/dkneis/cpp/dklib/typeconv/typeconv.h"


using namespace std;

void from_string (const string &s) {
  try {
    cout << "String '" << s << "' --> double: ";
    cout << as_double(s) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
  try {
    cout << "String '" << s << "' --> int: ";
    cout << as_integer(s) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
  try {
    cout << "String '" << s << "' --> uint: ";
    cout << as_unsigned_integer(s) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
}

void from_double (const double d) {
  try {
    cout << "Double '" << d << "' --> string: ";
    cout << as_string(d) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
  try {
    cout << "Double '" << d << "' --> int: ";
    cout << as_integer(d) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
  try {
    cout << "Double '" << d << "' --> uint: ";
    cout << as_unsigned_integer(d) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
}

void from_int (const int i) {
  try {
    cout << "Int '" << i << "' --> string: ";
    cout << as_string(i) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
  try {
    cout << "Int '" << i << "' --> uint: ";
    cout << as_unsigned_integer(i) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
  try {
    cout << "Int '" << i << "' --> double: ";
    cout << as_double(i) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
}

void from_uint (const unsigned int i) {
  try {
    cout << "uInt '" << i << "' --> string: ";
    cout << as_string(i) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
  try {
    cout << "uInt '" << i << "' --> int: ";
    cout << as_integer(i) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
  try {
    cout << "uInt '" << i << "' --> double: ";
    cout << as_double(i) << endl;
  } catch (except) {
    cout << "Failed." << endl;
  }
}

int main (void) {

  cout << "---------------------------------" << endl;
  string s;
  s= "Hallo";
  from_string(s);
  s= "-1.0";
  from_string(s);
  s= "1.0";
  from_string(s);
  s= "5.0e999";
  from_string(s);
  s= "2";
  from_string(s);
  s= "-2";
  from_string(s);

  cout << "---------------------------------" << endl;
  double d;
  d= -5.;
  from_double(d);
  d= 5.;
  from_double(d);
  d= -5.e99;
  from_double(d);
  d= +5.e99;
  from_double(d);
  d= 5.e-99;
  from_double(d);
  d= 3.3;
  from_double(d);
  d= 3.5;
  from_double(d);
  d= 3.6;
  from_double(d);

  cout << "---------------------------------" << endl;
  int i;
  i= -500000;
  from_int(i);
  i= -5;
  from_int(i);
  i= 0;
  from_int(i);
  i= 5;
  from_int(i);
  i= 500000;
  from_int(i);

  cout << "---------------------------------" << endl;
  int ui;
  ui= 0;
  from_uint(ui);
  ui= 5;
  from_uint(ui);
  ui= 500000;
  from_uint(ui);
  ui= 4200000000;
  from_uint(ui);

}
