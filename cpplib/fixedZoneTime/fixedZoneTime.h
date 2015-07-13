
#ifndef FIXEDZONETIME_H
#define FIXEDZONETIME_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>  // for function abs() and floor()

#include "../except/except.h"
#include "../typeconv/typeconv.h"

class fixedZoneTime {
  private:
    double unixtime;     // 8-byte number to avoid overflow in year 2038
  public:
    // Constructors
    fixedZoneTime();
    fixedZoneTime(const double &unixtime);
    fixedZoneTime(const int &year, const int &month, const int &day,
      const int &hour, const int &minute, const int &second);
    fixedZoneTime(const string &iso8601_string);
    // Destructor
    ~fixedZoneTime();
    // Overloaded comparison operators
    friend bool operator < (const fixedZoneTime &t1, const fixedZoneTime &t2);
    friend bool operator <= (const fixedZoneTime &t1, const fixedZoneTime &t2);
    friend bool operator >= (const fixedZoneTime &t1, const fixedZoneTime &t2);
    friend bool operator > (const fixedZoneTime &t1, const fixedZoneTime &t2);
    friend bool operator == (const fixedZoneTime &t1, const fixedZoneTime &t2);
    friend bool operator != (const fixedZoneTime &t1, const fixedZoneTime &t2);
    // Overloaded numerical operators
    fixedZoneTime operator + (const int &seconds);
    fixedZoneTime operator - (const int &seconds);
    fixedZoneTime operator + (const unsigned int &seconds);
    fixedZoneTime operator - (const unsigned int &seconds);
    // Set methods
    void set(const double &unixtime);
    void set(const int &year, const int &month, const int &day,
      const int &hour, const int &minute, const int &second);
    void set(const string &iso8601_string);
    // Get methods
    double get() const;
    string get(const string &sepDate, const string &sepTime, const string &sepDateTime) const;
    void get(int &year, int &month, int &day, int &hour, int &minute, int &second);
};

#endif

