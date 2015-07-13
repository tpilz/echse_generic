
#ifndef ECHSE_CORECLASS_SPACETIMEDATACOLLECTION_H
#define ECHSE_CORECLASS_SPACETIMEDATACOLLECTION_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "except/except.h"
#include "table/table.h"
#include "arrays/arrays.h"
#include "typeconv/typeconv.h"
#include "spaceTimeData/spaceTimeData.h"

#include "echse_globalConst.h"

using namespace std;

class spaceTimeDataCollection {
  private:
    // Names of variables
    vector<string> variableNames;
    // Pointers to spaceTimeData objects created on the heap
    vector<spaceTimeData*> dataPointers;
    // Don't allow assignment or copy construction (made private + not implemented)
    spaceTimeDataCollection& operator=(const spaceTimeDataCollection &x);
    spaceTimeDataCollection(const spaceTimeDataCollection &x);
  public:
    // Ctor & dtor
    spaceTimeDataCollection();
    ~spaceTimeDataCollection();
    // Methods
    void init(const string &file, const string &chars_colsep,
      const string &chars_comment, const unsigned int bufferSize);
    const vector<string>& get_variableNames() const;
    const vector<string>& get_locations(const unsigned int index) const;
    double get_value(const unsigned int indexVariable,
      const unsigned int indexLocation) const;
    const double* get_address(const unsigned int indexVariable,
      const unsigned int indexLocation) const;
    void update(const fixedZoneTime &timeStart,
      const fixedZoneTime &timeEnd) const;
    void clear();
};

#endif

