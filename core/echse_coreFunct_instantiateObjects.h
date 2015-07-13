
#ifndef ECHSE_COREFUNCT_INSTANTIATEOBJECTS_H
#define ECHSE_COREFUNCT_INSTANTIATEOBJECTS_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "except/except.h"
#include "table/table.h"
#include "typeconv/typeconv.h"

#include "echse_coreClass_abstractObject.h"
#include "echse_coreClass_abstractObjectGroup.h"
#include "echse_coreClass_templateObjectGroup.h"

#include "echse_globalConst.h"

using namespace std;

void instantiateObjects(
  const string file_objects, const string chars_colsep, const string chars_comment,
  vector<abstractObjectGroup*> &objectGroups,
  vector<abstractObject*> &objects
);

#endif

