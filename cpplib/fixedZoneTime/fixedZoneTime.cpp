
#include "fixedZoneTime.h"

////////////////////////////////////////////////////////////////////////////////
// SECTION 1: Prototypes of non-class methods (defined in Section 3) 
////////////////////////////////////////////////////////////////////////////////

bool datetime_to_unixtime(const int &year, const int &month, const int &day,
  const int &hour, const int &minute, const int &second, double &unixtime);

bool unixtime_to_datetime(const double &unixtime,
  int &year, int &month, int &day, int &hour, int &minute, int &second);

////////////////////////////////////////////////////////////////////////////////
// SECTION 2: Class methods and static data
////////////////////////////////////////////////////////////////////////////////

// Constructors
fixedZoneTime::fixedZoneTime() {
  unixtime= 0.;
}
fixedZoneTime::fixedZoneTime(const double &unixtime) {
  fixedZoneTime::unixtime= unixtime;
}
fixedZoneTime::fixedZoneTime(const int &year, const int &month,
  const int &day, const int &hour, const int &minute, const int &second) {
  try {
    set(year,month,day,hour,minute,second);
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Failed to convert date/time components to datetime.", __FILE__, __LINE__);
    throw(e);
  }
}
fixedZoneTime::fixedZoneTime(const string &iso8601_string) {
  try {
    set(iso8601_string);
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Failed to convert iso8601 string to datetime.", __FILE__, __LINE__);
    throw(e);
  }
}

// Destructor
fixedZoneTime::~fixedZoneTime() {}

// Overloaded comparison operators
// Note: Implementation as global friend functions. If the operators were
//       implemented as class methods, the left operand would always have to
//       be an actual fixedZoneTime-object (a reference would'nt be accepted).
bool operator < (const fixedZoneTime &t1, const fixedZoneTime &t2) {return(t1.get() < t2.get());}
bool operator <= (const fixedZoneTime &t1, const fixedZoneTime &t2) {return(t1.get() <= t2.get());}
bool operator >= (const fixedZoneTime &t1, const fixedZoneTime &t2) {return(t1.get() >= t2.get());}
bool operator > (const fixedZoneTime &t1, const fixedZoneTime &t2) {return(t1.get() > t2.get());}
bool operator == (const fixedZoneTime &t1, const fixedZoneTime &t2) {return(t1.get() == t2.get());}
bool operator != (const fixedZoneTime &t1, const fixedZoneTime &t2) {return(t1.get() != t2.get());}
// Overloaded numerical operators
// Note: Implemented as class methods
fixedZoneTime fixedZoneTime::operator + (const int &seconds) {
  fixedZoneTime t_new(this->get() + seconds);
  return(t_new);
}
fixedZoneTime fixedZoneTime::operator - (const int &seconds) {
  fixedZoneTime t_new(this->get() - seconds);
  return(t_new);
}
fixedZoneTime fixedZoneTime::operator + (const unsigned int &seconds) {
  fixedZoneTime t_new(this->get() + seconds);
  return(t_new);
}
fixedZoneTime fixedZoneTime::operator - (const unsigned int &seconds) {
  fixedZoneTime t_new(this->get() - seconds);
  return(t_new);
}

////////////////////////////////////////////////////////////////////////////////
// Set methods
////////////////////////////////////////////////////////////////////////////////

// Input: Unix time
void fixedZoneTime::set(const double &unixtime) {
  fixedZoneTime::unixtime= unixtime;
}

// Input: Date & time components
void fixedZoneTime::set(const int &year, const int &month,
  const int &day, const int &hour,
  const int &minute, const int &second) {
  if (!datetime_to_unixtime(year, month, day, hour, minute, second, unixtime)) {
    stringstream errmsg;
    errmsg << "Invalid date and/or time components detected (Year=" << year <<
      " Month=" << month << " Day=" << day << " Hour=" << hour <<
      " Minutes=" << minute << " Seconds=" << second << "').";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

// Input: String in ISO format (YYYY-MM-DD hh:mm:ss)
void fixedZoneTime::set(const string &iso8601_string) {
  int year, month, day, hour, minute, second;
  if (iso8601_string.length() != 19) {
    stringstream errmsg;
    errmsg << "Datetime string '" << iso8601_string << "' is not in ISO 8601 format" <<
      " ('YYYY-MM-DD hh:mm:ss'). String has " << iso8601_string.length() <<
      " instead of 19 character(s).";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  if (iso8601_string.substr(10,1).find_first_of("0123456789") != string::npos) {
    stringstream errmsg;
    errmsg << "Datetime string '" << iso8601_string << "' is not in ISO 8601 format"
      " ('YYYY-MM-DD hh:mm:ss'). Expecting date and time to be separated" <<
      " by a single alpha-numeric character at position 10 (counting from zero).";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  // YYYY-MM-DD hh:mm:ss
  // 0123456789012345678
  try {
    year=   as_integer(iso8601_string.substr(0,4));
    month=  as_integer(iso8601_string.substr(5,2));
    day=    as_integer(iso8601_string.substr(8,2));
    hour=   as_integer(iso8601_string.substr(11,2));
    minute= as_integer(iso8601_string.substr(14,2));
    second= as_integer(iso8601_string.substr(17,2));
  } catch (except) {
    stringstream errmsg;
    errmsg << "Datetime string '" << iso8601_string << "' is not in ISO 8601 format" <<
      " ('YYYY-MM-DD hh:mm:ss'). Extraction of numeric component(s) failed.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  if (!datetime_to_unixtime(year, month, day, hour, minute, second, unixtime)) {
    stringstream errmsg;
    errmsg << "Datetime string '" << iso8601_string << "' is not in ISO 8601 format" <<
      " ('YYYY-MM-DD hh:mm:ss') and/or contains invalid components.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

// Get methods
double fixedZoneTime::get() const {
  return(unixtime);
}

string fixedZoneTime::get(const string &sepDate, const string &sepTime, const string &sepDateTime) const {
  int year, month, day, hour, minute, second;
  if (!unixtime_to_datetime(unixtime, year, month, day, hour, minute, second)) {
    stringstream errmsg;
    errmsg << "Unix time '" << unixtime << "' not convertable into date/time components.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  stringstream ss;
  ss << setw(4) << setfill('0') << year << sepDate <<
        setw(2) << setfill('0') << month << sepDate <<
        setw(2) << setfill('0') << day << sepDateTime <<
        setw(2) << setfill('0') << hour << sepTime <<
        setw(2) << setfill('0') << minute << sepTime <<
        setw(2) << setfill('0') << second;
  return(ss.str());
}

void fixedZoneTime::get(int &year, int &month, int &day, int &hour, int &minute, int &second) {
  if (!unixtime_to_datetime(unixtime, year, month, day, hour, minute, second)) {
    stringstream errmsg;
    errmsg << "Unix time '" << unixtime << "' not convertable into date/time components.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// SECTION 3: Non-class methods used by the class methods
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Return the number of days in a year (full leap year rules)
////////////////////////////////////////////////////////////////////////////////
// 1) Every year that can be divided by 4 without remainder is a leap year
// 2) If the year can be divided by 100 without remainder it is not a leap year (exception from 1)
// 3) If the year can be divided by 400 without remainder it is a leap year (exception from 2)
int days_in_year(const int &year) {
  if ((((year%4) == 0) && ((year%100) != 0)) || ((year%400) == 0)) {
    return(366);
  } else {
    return(365);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Verify date
////////////////////////////////////////////////////////////////////////////////

bool is_date(const int &year, const int &month, const int &day) {
  const int D1[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
  const int D2[12]= {31,29,31,30,31,30,31,31,30,31,30,31};
  if ((month < 1) || (month > 12)) return(false);
  if (day < 1) return(false);
  if (days_in_year(year) == 365) {
    if (day > D1[month-1]) return(false);
  } else {
    if (day > D2[month-1]) return(false);
  }
  return(true);
}

////////////////////////////////////////////////////////////////////////////////
// Verify time
////////////////////////////////////////////////////////////////////////////////

bool is_time(const int &hour, const int &minute, const int &second) {
  if ((hour < 0) || (minute < 0) || (second < 0)) return(false);
  if ((hour > 23) || (minute > 59) || (second > 59)) return(false);
  return(true);
}

////////////////////////////////////////////////////////////////////////////////
// Compute month and day for a given daynumber in a given year.
// If successful, the function returns TRUE. On error, the returned value is FALSE.
////////////////////////////////////////////////////////////////////////////////

bool daynumber_to_date(const int &daynumber, const int &year, int &month, int &day) {
  const int D1[13]= {0,31,59,90,120,151,181,212,243,273,304,334,365};
  const int D2[13]= {0,31,60,91,121,152,182,213,244,274,305,335,366};
  // Initialize output with invalid data
  month= 0;
  day= 0;
  // Check input
  if (daynumber < 1) return(false);
  int ndays= days_in_year(year);
  if (daynumber > ndays) return(false);
  // Compute
  if (ndays == 365) {
    for (unsigned int i=1; i<13; i++) {
      if (daynumber <= D1[i]) {
        month= i;
        day= daynumber - D1[i-1];
        break;
      }
    }
  } else {
    for (unsigned int i=1; i<13; i++) {
      if (daynumber <= D2[i]) {
        month= i;
        day= daynumber - D2[i-1];
        break;
      }
    }
  }
  return(true);
}

////////////////////////////////////////////////////////////////////////////////
// Compute the daynumber in a year for a given date.
// If successful, the function returns TRUE. On error, the returned value is FALSE.
////////////////////////////////////////////////////////////////////////////////

bool date_to_daynumber(const int &year, const int &month,
  const int &day, int &daynumber) {
  const int D1[12]= {0,31,59,90,120,151,181,212,243,273,304,334};
  const int D2[12]= {0,31,60,91,121,152,182,213,244,274,305,335};
  // Initialize output with invalid data
  daynumber=0;
  // Check input
  if (!is_date(year,month,day)) return(false);
  if (days_in_year(year) == 365) {
    daynumber= D1[month-1] + day;
  } else {
    daynumber= D2[month-1] + day;
  }
  return(true);
}

////////////////////////////////////////////////////////////////////////////////
// Function with the same behavior as FORTRAN's int() function
//     truncate(1.7)= 1
//     truncate(-3.7)= -3
// Note: The functions truncate() and floor() give different results for
//       negative arguments only ( floor(-3.7)= -4 ).
////////////////////////////////////////////////////////////////////////////////

int truncate(const double &x) {
  return(static_cast<int>(x));
}

////////////////////////////////////////////////////////////////////////////////
// Convert a given date and time to unixtime (not accounting for leap seconds)
// If successful, the function returns TRUE. On error, the returned value is FALSE.
////////////////////////////////////////////////////////////////////////////////

bool datetime_to_unixtime(const int &year, const int &month, const int &day,
  const int &hour, const int &minute, const int &second, double &unixtime) {
  const int D1[12]= {0,31,59,90,120,151,181,212,243,273,304,334};
  const int D2[12]= {0,31,60,91,121,152,182,213,244,274,305,335};
  // Initialize output to obvious nonsense
  unixtime= 0;
  // Check input
  if (!is_date(year,month,day)) return(false);
  if (!is_time(hour,minute,second)) return(false);
  // Initialize day counter
  int d= 0;
  // Dates after start of the unix epoche (1970-01-01 00:00:00)
  if (year >= 1970) {
    // Full years
    if (year > 1970) {
      // Simplified leap year rules for 1970...2099
      if (year < 2099) {
        d= (year-1970) * 365 + truncate((year-1-1968)/4.0);
      // Full rules
      } else {
        for (int i=1970; i<=(year-1); i++) d= d + days_in_year(i);
      }
    }
    // Full days
    if (days_in_year(year) == 365) {
      d= d + (D1[month-1]+day-1);
    } else {
      d= d + (D2[month-1]+day-1);
    }
    // Time of the day
    unixtime= d*86400. + (hour*3600. + minute*60. + second);
  // Dates before the unix epoche (1970-01-01 00:00:00)
  } else {
    // Full years
    if (year < 1969) {
      for (int i=(year+1); i<=1969; i++) d= d - days_in_year(i);
    }
    // Full days
    if (days_in_year(year) == 365) {
      d= d - (365-D1[month-1]-day);
    } else {
      d= d - (366-D2[month-1]-day);
    }
    // Time of the day
    unixtime= d*86400. - (86400. - hour*3600. - minute*60. - second);
  }
  return(true);
}

////////////////////////////////////////////////////////////////////////////////
// Convert a given unixtime to date and time (not accounting for leap seconds)
// If successful, the function returns TRUE. On error, the returned value is FALSE.
////////////////////////////////////////////////////////////////////////////////

bool unixtime_to_datetime(const double &unixtime,
  int &year, int &month, int &day, int &hour, int &minute, int &second) {
  const double ZERO= 0.;
  // ########## BEGIN NEW (Speed up determination of the year for common cases) 
  // Precomputed unix times for 01 Jan 00:00:00 of selected years
  const unsigned int NUMYEARS= 4;
  const int    YEAR[NUMYEARS]= {1990, 2000, 2010, 2020};
  const double SECS[NUMYEARS]= {631152000., 946684800., 1262304000., 1577836800.};
  double elapsedsecs;
  // ########## END NEW
  double remainsecs;
  int daynumber;
  bool leapyear;
  // Initialize output
  year= 0; month= 0; day= 0; hour= 0; minute= 0; second= 0;
  // CASE 1 --- Dates after start of the unix epoche (1970-01-01 00:00:00)
  if (unixtime >= ZERO) {
    remainsecs= unixtime + 0.1; // Adding 10 ms avoids rounding problems 
    // Determine the year
    year= 1970;
    // ########## BEGIN NEW (Speed up determination of the year for common cases)
    elapsedsecs= 0.;
    for (unsigned int i=0; i<NUMYEARS; i++) {
      if (remainsecs < SECS[i]) {
        break;
      } else {
        year= YEAR[i];
        elapsedsecs= SECS[i];
      }
    }
    remainsecs= remainsecs - elapsedsecs;
    // ########## END NEW
    while (true) {
      if ((remainsecs - days_in_year(year)*86400) < ZERO) {
        break;
      } else {
        remainsecs= remainsecs - days_in_year(year)*86400;
        year++;
      }
    }
    // Determine month and day
    daynumber= truncate(remainsecs/86400)+1;
    if (!daynumber_to_date(daynumber,year,month,day)) {
      year= 0; month= 0; day= 0; hour= 0; minute= 0; second= 0;
      return(false);
    }
    // Determine time components
    remainsecs= remainsecs - (daynumber-1)*86400;
    hour= truncate(remainsecs/3600);
    remainsecs= remainsecs - hour*3600;
    minute= truncate(remainsecs/60);
    second= truncate(remainsecs - minute*60);
  // CASE 2 --- Dates before the unix epoche (1970-01-01 00:00:00)
  } else {
    remainsecs= abs(unixtime) - 0.1; // Subtracting 10 ms avoids rounding problems
    // Determine the year
    year= 1969;
    while (true) {
      if ((remainsecs - days_in_year(year)*86400) < ZERO) {
        break;
      } else {
        remainsecs= remainsecs - days_in_year(year)*86400;
        year--;
      }
    }
    // Determine month and day
    daynumber= days_in_year(year) - truncate(remainsecs/86400);
    if (!daynumber_to_date(daynumber,year,month,day)) {
      year= 0; month= 0; day= 0; hour= 0; minute= 0; second= 0;
      return(false);
    }
    // Determine time components
    remainsecs= remainsecs - (days_in_year(year)*86400 - daynumber*86400);
    hour= truncate((86400 - remainsecs)/3600);
    remainsecs= remainsecs - (23 - hour)*3600;
    minute= truncate((3600-remainsecs)/60);
    remainsecs= remainsecs - (59 - minute)*60;
    second= truncate(60 - remainsecs);
  }
  // Convert 24:00:00 to the next day 00:00:00
  if (hour == 24) {
    if (!date_to_daynumber(year,month,day,daynumber)) {
      year= 0; month= 0; day= 0; hour= 0; minute= 0; second= 0;
      return(false);
    }
    leapyear= (days_in_year(year) == 366);
    if ((leapyear && (daynumber == 366)) || ((!leapyear) && (daynumber == 365))) {
      year++;
      month= 1;
      day= 1;
    } else {
      if (!daynumber_to_date((daynumber+1),year,month,day)) {
        year= 0; month= 0; day= 0; hour= 0; minute= 0; second= 0;
        return(false);
      }
    }
    hour= 0;
    minute= 0;
    second= 0;
  }
  // Check results and return
  if ((!is_date(year,month,day)) || (!is_time(hour,minute,second))) {
    year= 0; month= 0; day= 0; hour= 0; minute= 0; second= 0;
    return(false);
  }
  return(true);
}

