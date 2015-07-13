
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>

#include "../except/except.h"
#include "../typeconv/typeconv.h"
#include "../stringmanip/stringmanip.h"
#include "../fixedZoneTime/fixedZoneTime.h"

using namespace std;

void read_syn (
  const string synfile,
  const bool syn_timeAtEOI,
  const bool location_lowercase,
  const bool location_removePrefix,
  string &location,
  vector<double> &values,
  fixedZoneTime &firstEOI,
  unsigned int &dt_seconds
);


