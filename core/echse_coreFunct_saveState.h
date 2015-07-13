
#ifndef ECHSE_COREFUNCT_SAVESTATE_H
#define ECHSE_COREFUNCT_SAVESTATE_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "except/except.h"
#include "typeconv/typeconv.h"
#include "fixedZoneTime/fixedZoneTime.h"

#include "echse_coreClass_abstractObject.h"
#include "echse_globalConst.h"

using namespace std;

void saveState(
  const string &outdir,
  const fixedZoneTime &now,
  const vector<fixedZoneTime> &times_stateOutput,
  const string chars_colsep, const string chars_comment,
  const vector<abstractObject*> &objects
);

#endif

