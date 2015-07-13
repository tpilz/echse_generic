#ifndef ECHSE_GLOBALCONST_H
#define ECHSE_GLOBALCONST_H

#include <string>

using namespace std;

// Definition of 'global' constants. The constants are defined in a separate
// namespace to force the use of the scope operator (makes code clearer) and to
// avoid possible name conflicts.

namespace globalConst {

  // Names of colums to be used in ALL input files
  struct T_colNames {
    string objectID;           // Column with object IDs (formerly called model IDs)
    string objectGroupID;      // Column with object group IDs (formerly called modelFamily IDs)
    string functionName;       // Column with function names
    string argumentColumnName; // Column with names of columns containg a function's arguments
    string valueColumnName;    // Column with names of columns containg a function's values
    string parameterName;      // Column with parameter names
    string variableName;       // Column with variable names
    string value;              // Column with numerical values
    string sums;               // Column with logical values (true, if time series data represent sums)
    string past;               // Column with logical values (true, if time series timestamps refer to end of interval)
    string fileName;           // Column with file names
    string time;               // Column with times (as strings)
    string sourceObjectID;     // Column with object IDs (objects that publish data to be used by other objects)
    string sourceVariableName; // Column with variable names (variables, whose value are published for use by objects)
    string targetObjectID;     // Column with object IDs (objects that use public data of other objects)
    string targetVariableName; // Column with variable names (variables, initialized by public data of other objects)
    string forwardType;        // Column with logical values (true, if object relation is of forward type)
    string weight;             // Column with weights
    string locationID;         // Column with location IDs
    string index;              // Column with integer indices
    string digits;             // Column specifying a number of digits for output formatting
  } const colNames= {
  // The names are defined below
    "object",
    "objectGroup",
    "function",
    "col_arg",
    "col_val",
    "parameter",
    "variable",
    "value",
    "sums",
    "past",
    "file",
    "time",
    "sourceObject",
    "sourceVariable",
    "targetObject",
    "targetVariable",
    "forwardType",
    "weight",
    "location",
    "index",
    "digits"
  };

  // File extenstions used for output files
  struct T_fileExtensions {
    string tabular;
    string debug;
    string json;
  } const fileExtensions= {
  // The names are defined below
    ".txt",
    ".dbg",
    ".json"
  };

  // Thresholds where the program's state should be printed (% done)
  const vector<unsigned int> progressSteps= {1, 2, 5, 10, 20, 50};

} // End of namespace 'globalConst'

#endif
