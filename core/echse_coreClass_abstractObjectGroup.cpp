
#include "echse_coreClass_abstractObjectGroup.h"

////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////

abstractObjectGroup::abstractObjectGroup () { }

////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////

abstractObjectGroup::~abstractObjectGroup () {
  idObjectGroup= "";
  namesParamsFun.clear();
  namesParamsNum.clear();
  namesInputsExt.clear();
  namesInputsSim.clear();
  namesStatesScal.clear();
  namesStatesVect.clear();
  namesOutputs.clear();
  namesSharedParamsFun.clear();
  namesSharedParamsNum.clear();
  mapParamsFun.clear();
  mapParamsNum.clear();
  mapInputsExt.clear();
  mapInputsSim.clear();
  mapStatesScal.clear();
  mapStatesVect.clear();
  mapOutputs.clear();
  mapSharedParamsFun.clear();
  mapSharedParamsNum.clear();
  table_paramsFun.clear();
  table_paramsNum.clear();
  sharedParamsFun.clear();
  sharedParamsNum.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Set and get group ID
////////////////////////////////////////////////////////////////////////////////

void abstractObjectGroup::set_idObjectGroup(const string id) {
  idObjectGroup= id;
}
string abstractObjectGroup::get_idObjectGroup() const {
  return(idObjectGroup);
}

////////////////////////////////////////////////////////////////////////////////
// Set names of parameters, forcings, states, and outputs
// Also initialize the map which maps the name to the array index.
////////////////////////////////////////////////////////////////////////////////

// Individual
void abstractObjectGroup::set_namesParamsFun(const vector<string> &names) {
  pair<name_index_map::const_iterator, bool> insert_status;
  namesParamsFun.assign(names.begin(), names.end());
  for (unsigned int i=0; i<names.size(); i++) {
    insert_status= mapParamsFun.insert(name_index_pair(names[i],i));
    if (!insert_status.second) {
      stringstream errmsg;
      errmsg << "Cannot add '" << names[i] << "' to list of names of individual" <<
        " parameter functions for object group '" << idObjectGroup <<
        "'. This is probably due to non-unique names.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
}
// Shared
void abstractObjectGroup::set_namesSharedParamsFun(const vector<string> &names) {
  pair<name_index_map::const_iterator, bool> insert_status;
  namesSharedParamsFun.assign(names.begin(), names.end());
  for (unsigned int i=0; i<names.size(); i++) {
    insert_status= mapSharedParamsFun.insert(name_index_pair(names[i],i));
    if (!insert_status.second) {
      stringstream errmsg;
      errmsg << "Cannot add '" << names[i] << "' to list of names of shared" <<
        " parameter functions for object group '" << idObjectGroup <<
        "'. This is probably due to non-unique names.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
}
// Individual
void abstractObjectGroup::set_namesParamsNum(const vector<string> &names) {
  pair<name_index_map::const_iterator, bool> insert_status;
  namesParamsNum.assign(names.begin(), names.end());
  for (unsigned int i=0; i<names.size(); i++) {
    insert_status= mapParamsNum.insert(name_index_pair(names[i],i));
    if (!insert_status.second) {
      stringstream errmsg;
      errmsg << "Cannot add '" << names[i] << "' to list of names of individual" <<
        " scalar parameters for object group '" << idObjectGroup <<
        "'. This is probably due to non-unique names.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
}
// Shared
void abstractObjectGroup::set_namesSharedParamsNum(const vector<string> &names) {
  pair<name_index_map::const_iterator, bool> insert_status;
  namesSharedParamsNum.assign(names.begin(), names.end());
  for (unsigned int i=0; i<names.size(); i++) {
    insert_status= mapSharedParamsNum.insert(name_index_pair(names[i],i));
    if (!insert_status.second) {
      stringstream errmsg;
      errmsg << "Cannot add '" << names[i] << "' to list of names of shared" <<
        " scalar parameters for object group '" << idObjectGroup <<
        "'. This is probably due to non-unique names.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
}
void abstractObjectGroup::set_namesInputsExt(const vector<string> &names) {
  pair<name_index_map::const_iterator, bool> insert_status;
  namesInputsExt.assign(names.begin(), names.end());
  for (unsigned int i=0; i<names.size(); i++) {
    insert_status= mapInputsExt.insert(name_index_pair(names[i],i));
    if (!insert_status.second) {
      stringstream errmsg;
      errmsg << "Cannot add '" << names[i] << "' to list of names of external" <<
        " inputs for object group '" << idObjectGroup <<
        "'. This is probably due to non-unique names.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
}
void abstractObjectGroup::set_namesInputsSim(const vector<string> &names) {
  pair<name_index_map::const_iterator, bool> insert_status;
  namesInputsSim.assign(names.begin(), names.end());
  for (unsigned int i=0; i<names.size(); i++) {
    insert_status= mapInputsSim.insert(name_index_pair(names[i],i));
    if (!insert_status.second) {
      stringstream errmsg;
      errmsg << "Cannot add '" << names[i] << "' to list of names of simulated" <<
        " inputs for object group '" << idObjectGroup <<
        "'. This is probably due to non-unique names.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
}
void abstractObjectGroup::set_namesStatesScal(const vector<string> &names) {
  pair<name_index_map::const_iterator, bool> insert_status;
  namesStatesScal.assign(names.begin(), names.end());
  for (unsigned int i=0; i<names.size(); i++) {
    insert_status= mapStatesScal.insert(name_index_pair(names[i],i));
    if (!insert_status.second) {
      stringstream errmsg;
      errmsg << "Cannot add '" << names[i] << "' to list of names of scalar" <<
        " state variables for object group '" << idObjectGroup <<
        "'. This is probably due to non-unique names.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
}
void abstractObjectGroup::set_namesStatesVect(const vector<string> &names) {
  pair<name_index_map::const_iterator, bool> insert_status;
  namesStatesVect.assign(names.begin(), names.end());
  for (unsigned int i=0; i<names.size(); i++) {
    insert_status= mapStatesVect.insert(name_index_pair(names[i],i));
    if (!insert_status.second) {
      stringstream errmsg;
      errmsg << "Cannot add '" << names[i] << "' to list of names of vector" <<
        " state variables for object group '" << idObjectGroup <<
        "'. This is probably due to non-unique names.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
}
void abstractObjectGroup::set_namesOutputs(const vector<string> &names) {
  pair<name_index_map::const_iterator, bool> insert_status;
  namesOutputs.assign(names.begin(), names.end());
  for (unsigned int i=0; i<names.size(); i++) {
    insert_status= mapOutputs.insert(name_index_pair(names[i],i));
    if (!insert_status.second) {
      stringstream errmsg;
      errmsg << "Cannot add '" << names[i] << "' to list of names of object" <<
        " outputs for object group '" << idObjectGroup <<
        "'. This is probably due to non-unique names.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Get names of parameters, forcings, states, and outputs
////////////////////////////////////////////////////////////////////////////////

const vector<string>& abstractObjectGroup::get_namesParamsFun() const {return namesParamsFun;}
const vector<string>& abstractObjectGroup::get_namesParamsNum() const {return namesParamsNum;}
const vector<string>& abstractObjectGroup::get_namesSharedParamsFun() const {return namesSharedParamsFun;}
const vector<string>& abstractObjectGroup::get_namesSharedParamsNum() const {return namesSharedParamsNum;}
const vector<string>& abstractObjectGroup::get_namesInputsExt() const {return namesInputsExt;}
const vector<string>& abstractObjectGroup::get_namesInputsSim() const {return namesInputsSim;}
const vector<string>& abstractObjectGroup::get_namesStatesScal() const {return namesStatesScal;}
const vector<string>& abstractObjectGroup::get_namesStatesVect() const {return namesStatesVect;}
const vector<string>& abstractObjectGroup::get_namesOutputs() const {return namesOutputs;}

////////////////////////////////////////////////////////////////////////////////
// Determine the array index of a certain parameter, forcing, state, or output
// using the corresponding name. These convenience functions are not used, normally.
////////////////////////////////////////////////////////////////////////////////

// Individual
unsigned int abstractObjectGroup::indexParamsFun(const string &name) const {
  name_index_map::const_iterator iter= mapParamsFun.find(name);
  if (iter == mapParamsFun.end()) {
    stringstream errmsg;
    errmsg << "Cannot return index of individual parameter function '" << name << "'." <<
      " A function with that name does not exist for object group '" << idObjectGroup << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(iter->second);
}
// Shared
unsigned int abstractObjectGroup::indexSharedParamsFun(const string &name) const {
  name_index_map::const_iterator iter= mapSharedParamsFun.find(name);
  if (iter == mapSharedParamsFun.end()) {
    stringstream errmsg;
    errmsg << "Cannot return index of shared parameter function '" << name << "'." <<
      " A function with that name does not exist for object group '" << idObjectGroup << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(iter->second);
}
// Individual
unsigned int abstractObjectGroup::indexParamsNum(const string &name) const {
  name_index_map::const_iterator iter= mapParamsNum.find(name);
  if (iter == mapParamsNum.end()) {
    stringstream errmsg;
    errmsg << "Cannot return index of individual parameter '" << name << "'." <<
      " A parameter with that name does not exist for object group '" << idObjectGroup << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(iter->second);
}
// Shared
unsigned int abstractObjectGroup::indexSharedParamsNum(const string &name) const {
  name_index_map::const_iterator iter= mapSharedParamsNum.find(name);
  if (iter == mapSharedParamsNum.end()) {
    stringstream errmsg;
    errmsg << "Cannot return index of shared parameter '" << name << "'." <<
      " A parameter with that name does not exist for object group '" << idObjectGroup << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(iter->second);
}
unsigned int abstractObjectGroup::indexInputsExt(const string &name) const {
  name_index_map::const_iterator iter= mapInputsExt.find(name);
  if (iter == mapInputsExt.end()) {
    stringstream errmsg;
    errmsg << "Cannot return index of external input variable '" << name << "'." <<
      " An external input with that name does not exist for object group '" << idObjectGroup << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(iter->second);
}
unsigned int abstractObjectGroup::indexInputsSim(const string &name) const {
  name_index_map::const_iterator iter= mapInputsSim.find(name);
  if (iter == mapInputsSim.end()) {
    stringstream errmsg;
    errmsg << "Cannot return index of simulated input variable '" << name << "'." <<
      " A simulated input with that name does not exist for object group '" << idObjectGroup << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(iter->second);
}
unsigned int abstractObjectGroup::indexStatesScal(const string &name) const {
  name_index_map::const_iterator iter= mapStatesScal.find(name);
  if (iter == mapStatesScal.end()) {
    stringstream errmsg;
    errmsg << "Cannot return index of scalar state variable '" << name << "'." <<
      " A scalar state variable with that name does not exist for object group '" << idObjectGroup << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(iter->second);
}
unsigned int abstractObjectGroup::indexStatesVect(const string &name) const {
  name_index_map::const_iterator iter= mapStatesVect.find(name);
  if (iter == mapStatesVect.end()) {
    stringstream errmsg;
    errmsg << "Cannot return index of vector state variable '" << name << "'." <<
      " A vector state variable with that name does not exist for object group '" << idObjectGroup << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(iter->second);
}
unsigned int abstractObjectGroup::indexOutputs(const string &name) const {
  name_index_map::const_iterator iter= mapOutputs.find(name);
  if (iter == mapOutputs.end()) {
    stringstream errmsg;
    errmsg << "Cannot return index of output variable '" << name << "'." <<
      " An output with that name does not exist for object group '" << idObjectGroup << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(iter->second);
}

////////////////////////////////////////////////////////////////////////////////
// Set tables of individual parameters
////////////////////////////////////////////////////////////////////////////////

void abstractObjectGroup::set_tableParamsFun (const string file,
  const string chars_colsep, const string chars_comment
) {
  try {
    table_paramsFun.read(file, true, chars_colsep, chars_comment);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Could not read table of individual parameter functions for objects " <<
      "of object group '" << idObjectGroup << "' using file '" << file << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

void abstractObjectGroup::set_tableParamsNum (const string file,
  const string chars_colsep, const string chars_comment
) {
  vector<string> names;
  unsigned int colindex;
  try {
    table_paramsNum.read(file, true, chars_colsep, chars_comment);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Could not read table of individual scalar parameters for objects " <<
      "of object group '" << idObjectGroup << "' using file '" << file << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  try {
    colindex= table_paramsNum.colindex(globalConst::colNames.objectID);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Missing column '" << globalConst::colNames.objectID <<
      "' in table of individual scalar parameters read from file '" << file <<
      "' (object group '" << idObjectGroup << "').";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  // Check for duplicate rows (multiple entries for the same object)
  try {
    table_paramsNum.get_col(colindex, names);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Could not retrieve object IDs from table of individual scalar parameters" <<
      " read from file '" << file << "' (object group '" << idObjectGroup << "').";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  if (has_duplicates(names)) {
    stringstream errmsg;
    errmsg << "Duplicate object IDs detected in table of individual scalar parameters" <<
      " read from file '" << file << "' (object group '" << idObjectGroup <<
      "'). Expecting a single row per object.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  // Check for duplicate column names
  try {
    table_paramsNum.get_colnames(names);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Could not check column names in table of individual scalar parameters" <<
      " read from file '" << file << "' (object group '" << idObjectGroup <<
      "') for uniqueness.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  if (has_duplicates(names)) {
    stringstream errmsg;
    errmsg << "Duplicate column names detected in table of individual scalar parameters" <<
      " read from file '" << file << "' (object group '" << idObjectGroup <<
      "'). Expecting a single column per parameter.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set shared parameters
////////////////////////////////////////////////////////////////////////////////

void abstractObjectGroup::set_sharedParamsNum(const string file,
  const string chars_colsep, const string chars_comment) {
  unsigned int colindex_par;
  unsigned int colindex_val;
  table::size_type rowindex;
  // Allocate
  sharedParamsNum.resize(namesSharedParamsNum.size());
  if (sharedParamsNum.size() > 0) {
    table tab;
    // Read
    try {
      tab.read(file, true, chars_colsep, chars_comment);
    } catch (except) {
      stringstream errmsg;
      errmsg << "Could not read table of shared scalar parameters for objects " <<
        "of object group '" << idObjectGroup << "' using file '" << file << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    // Check column names and set column indices
    try {
      colindex_par= tab.colindex(globalConst::colNames.parameterName);
      colindex_val= tab.colindex(globalConst::colNames.value);
    } catch (except) {
      stringstream errmsg;
      errmsg << "Missing column(s) in table of shared scalar parameters" <<
        " read from file '" << file << "' (object group '" << idObjectGroup <<
        "'). Expecting columns '" << globalConst::colNames.parameterName <<
        "' and '" << globalConst::colNames.value << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    // Number of params must be table length
    if (tab.nrow() != sharedParamsNum.size()) {
      stringstream errmsg;
      errmsg << "The table of shared scalar parameters read from file '" <<
        file << "' (object group '" << idObjectGroup <<
        "') contains " << tab.nrow() << " records but " << sharedParamsNum.size() <<
        " are expected (one record for each shared parameter).";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    // Find values for all parameters
    for (unsigned int i=0; i<sharedParamsNum.size(); i++) {
      rowindex= tab.first_row(colindex_par, namesSharedParamsNum[i]);
      if (rowindex == 0) {
        stringstream errmsg;
        errmsg << "Missing record for parameter '" << namesSharedParamsNum[i] <<
          " in the table of shared scalar parameters read from file '" <<
          file << "' (object group '" << idObjectGroup << "').";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      try {
        sharedParamsNum[i]= as_double(tab.get_element(rowindex, colindex_val));
      } catch (except) {
        stringstream errmsg;
        errmsg << "Value of parameter '" << namesSharedParamsNum[i] <<
          " in the table of shared scalar parameters read from file '" <<
          file << "' (object group '" << idObjectGroup << "') is invalid.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
    tab.clear();  // Clean up
  }
}

void abstractObjectGroup::set_sharedParamsFun(const string file,
  const string chars_colsep, const string chars_comment) {
  string colname_inFile_arg;
  string colname_inFile_val;
  string funcfile;
  unsigned int colindex_fun;
  unsigned int colindex_fil;
  unsigned int colindex_arg;
  unsigned int colindex_val;
  table::size_type rowindex;
  // Allocate
  sharedParamsFun.resize(namesSharedParamsFun.size());
  if (sharedParamsFun.size() > 0) {
    table tab;
    // Read
    try {
      tab.read(file, true, chars_colsep, chars_comment);
    } catch (except) {
      stringstream errmsg;
      errmsg << "Could not read table of shared parameter functions for objects " <<
        "of object group '" << idObjectGroup << "' using file '" << file << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    // Check column names and set column indices
    try {
      colindex_fun= tab.colindex(globalConst::colNames.functionName);
      colindex_fil= tab.colindex(globalConst::colNames.fileName);
      colindex_arg= tab.colindex(globalConst::colNames.argumentColumnName);
      colindex_val= tab.colindex(globalConst::colNames.valueColumnName);
    } catch (except) {
      stringstream errmsg;
      errmsg << "Missing column(s) in table of shared parameter functions" <<
        " read from file '" << file << "' (object group '" << idObjectGroup <<
        "'). Expecting columns'" <<
        globalConst::colNames.functionName << "', '" <<
        globalConst::colNames.fileName << "', '" <<
        globalConst::colNames.argumentColumnName << "', and '" <<
        globalConst::colNames.valueColumnName << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    // Number of functions must be table length
    if (tab.nrow() != sharedParamsFun.size()) {
      stringstream errmsg;
      errmsg << "The table of shared parameter functions read from file '" <<
        file << "' (object group '" << idObjectGroup <<
        "') contains " << tab.nrow() << " records but " << sharedParamsFun.size() <<
        " are expected (one record for each shared function).";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    // Init all functions
    for (unsigned int i=0; i<sharedParamsFun.size(); i++) {
      rowindex= tab.first_row(colindex_fun, namesSharedParamsFun[i]);
      if (rowindex == 0) {
        stringstream errmsg;
        errmsg << "Missing record for function '" << namesSharedParamsFun[i] <<
          " in the table of shared parameter functions read from file '" <<
          file << "' (object group '" << idObjectGroup << "').";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      try { // This cannot actually fail since indices have already been checked
        funcfile= tab.get_element(rowindex, colindex_fil);
        colname_inFile_arg= tab.get_element(rowindex, colindex_arg);
        colname_inFile_val= tab.get_element(rowindex, colindex_val);
      } catch (except) {
        stringstream errmsg;
        errmsg << "This is a bug in the source code. Please report this.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Try to init the current function
      try {
        sharedParamsFun[i].read(funcfile, true, chars_colsep, chars_comment,
          namesSharedParamsFun[i], colname_inFile_arg, colname_inFile_val);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Could not initialize shared parameter function '" << 
          namesSharedParamsFun[i] << "' for object group '" << idObjectGroup <<
          "'. Initialization using the argument column '" << colname_inFile_arg <<
          "' and the values column '" << colname_inFile_val << "' of file '" <<
          funcfile << "' failed.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
    tab.clear();  // Clean up
  }
}

////////////////////////////////////////////////////////////////////////////////
// Methods that return tables of individual parameters
////////////////////////////////////////////////////////////////////////////////

const table & abstractObjectGroup::get_tableParamsFun() const {
  if (table_paramsFun.empty()) {
    stringstream errmsg;
    errmsg << "Table of individual parameter functions for objects " <<
      "of object group '" << idObjectGroup << "' is empty.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(table_paramsFun);
}

const table & abstractObjectGroup::get_tableParamsNum() const {
  if (table_paramsNum.empty()) {
    stringstream errmsg;
    errmsg << "Table of individual scalar parameters for objects " <<
      "of object group '" << idObjectGroup << "' is empty.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(table_paramsNum);
}

////////////////////////////////////////////////////////////////////////////////
// Methods to clear the tables of individual parameters
////////////////////////////////////////////////////////////////////////////////

void abstractObjectGroup::clear_tableParamsFun() {
  table_paramsFun.clear();
}

void abstractObjectGroup::clear_tableParamsNum() {
  table_paramsNum.clear();
}

