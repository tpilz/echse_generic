
#include "echse_coreClass_spaceTimeDataCollection.h"

////////////////////////////////////////////////////////////////////////////////
// Clear method
////////////////////////////////////////////////////////////////////////////////

void spaceTimeDataCollection::clear() {
  variableNames.clear();
  if (dataPointers.size() > 0) {
    // Delete data objects from heap created by the init-method
    for (unsigned int i=0; i<dataPointers.size(); i++) {
      dataPointers[i]->clear();
      delete dataPointers[i];
    }
    // Remove pointers
    dataPointers.clear();
  }
}

////////////////////////////////////////////////////////////////////////////////
// Ctor
////////////////////////////////////////////////////////////////////////////////

spaceTimeDataCollection::spaceTimeDataCollection() {
  clear();
}

////////////////////////////////////////////////////////////////////////////////
// Dtor
////////////////////////////////////////////////////////////////////////////////

spaceTimeDataCollection::~spaceTimeDataCollection() {
  clear();
}

////////////////////////////////////////////////////////////////////////////////
// Initialization method
////////////////////////////////////////////////////////////////////////////////

void spaceTimeDataCollection::init(const string &file,
  const string &chars_colsep, const string &chars_comment,
  const unsigned int bufferSize) {
  table tab;
  table::size_type colindex_vari, colindex_file, colindex_sums, colindex_past;
  vector<string> strvect;
  bool past, sums;
  try {
    // Initial clear
    clear();
    // Read table and identify columns
    tab.read(file, true, chars_colsep, chars_comment);
    try {
      colindex_vari= tab.colindex(globalConst::colNames.variableName);
      colindex_file= tab.colindex(globalConst::colNames.fileName);
      colindex_sums= tab.colindex(globalConst::colNames.sums);
      colindex_past= tab.colindex(globalConst::colNames.past);
    } catch (except) {
      stringstream errmsg;
      errmsg << "Missing columns in external inputs assignment table (file '" <<
        file << "'). Expecting columns '" <<
        globalConst::colNames.variableName << "', " <<
        globalConst::colNames.fileName << "', " <<
        globalConst::colNames.sums << "', and " <<
        globalConst::colNames.past << "'.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
    // Check for duplicate entries for a variable
    tab.get_col(colindex_vari, strvect);
    if (has_duplicates(strvect)) {
      stringstream errmsg;
      errmsg << "Duplicates found in column '" << globalConst::colNames.variableName << "' of file '" <<
        file << "'. Names of external input variables must be unique.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
    // Create and initialize spaceTimeData objects on the heap
    variableNames.resize(tab.nrow());
    dataPointers.resize(tab.nrow());
    for (unsigned int i=1; i<=tab.nrow(); i++) {
      dataPointers[i-1]= new(nothrow) spaceTimeData();
      if (!dataPointers[i-1]) {
        except e(__PRETTY_FUNCTION__,"Memory allocation failed.",__FILE__,__LINE__);
        throw(e);
      }
      try {
        past= as_logical(tab.get_element(i,colindex_past));
        sums= as_logical(tab.get_element(i,colindex_sums));
      } catch (except) {
        stringstream errmsg;
        errmsg << "Cannot set time series attributes for external input variable '" <<
          tab.get_element(i,colindex_vari) << "' to be read from file '" <<
          tab.get_element(i,colindex_file) << "'. Expecting logical values in columns '" <<
          globalConst::colNames.sums << "' and '" << globalConst::colNames.past <<
          "' of file '" << file << "'.";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      try {
        dataPointers[i-1]->init(tab.get_element(i,colindex_file),
          chars_colsep, chars_comment, past, sums, bufferSize);
        variableNames[i-1]= tab.get_element(i,colindex_vari);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Cannot initialize external input variable '" <<
          tab.get_element(i,colindex_vari) << "' using file '" <<
          tab.get_element(i,colindex_file) << "'.";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
    }
    // Clear table
    tab.clear();
  } catch (except) {
    stringstream errmsg;
    errmsg << "Cannot initialize external inputs based on file '" << file <<
      "' and/or files referenced therein.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Return variable names
////////////////////////////////////////////////////////////////////////////////

const vector<string>& spaceTimeDataCollection::get_variableNames() const {
  return(variableNames);
}

////////////////////////////////////////////////////////////////////////////////
// Return locations for a variable (specified by index)
////////////////////////////////////////////////////////////////////////////////

const vector<string>& spaceTimeDataCollection::get_locations(const unsigned int index) const {
  if (index >= dataPointers.size()) {
    stringstream errmsg;
    errmsg << "Variable index " << index << " out of range. Must be in [0," <<
      (dataPointers.size()-1) << "].";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  try {
    return(dataPointers[index]->get_locations());
  } catch (except) {
    stringstream errmsg;
    errmsg << "Cannot return locations for variable '" << variableNames[index] <<
      "' (index " << index << ").";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Return current value for a given variable, location
////////////////////////////////////////////////////////////////////////////////

double spaceTimeDataCollection::get_value(const unsigned int indexVariable,
  const unsigned int indexLocation) const {
  // Only check variable index; Location index is checked in sub-routine get_value()
  if (indexVariable >= variableNames.size()) {
    stringstream errmsg;
    errmsg << "Variable index out of range [0," << (variableNames.size()-1) << "].";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  try {
    return(dataPointers[indexVariable]->get_value(indexLocation));
  } catch (except) {
    stringstream errmsg;
    errmsg << "Cannot return value for variable '" << variableNames[indexVariable] <<
      "' at location with index " << indexLocation << ".";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Return the address of the value for a given variable and location
////////////////////////////////////////////////////////////////////////////////

const double* spaceTimeDataCollection::get_address(const unsigned int indexVariable,
  const unsigned int indexLocation) const {
  // Only check variable index; Location index is checked in sub-routine get_value()
  if (indexVariable >= variableNames.size()) {
    stringstream errmsg;
    errmsg << "Variable index out of range [0," << (variableNames.size()-1) << "].";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  try {
    return(dataPointers[indexVariable]->get_address(indexLocation));
  } catch (except) {
    stringstream errmsg;
    errmsg << "Cannot return address of data for variable '" << variableNames[indexVariable] <<
      "' at location with index " << indexLocation << ".";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Update method
////////////////////////////////////////////////////////////////////////////////

void spaceTimeDataCollection::update(const fixedZoneTime &timeStart, const fixedZoneTime &timeEnd) const {
  // Note: Difference between timeStart and timeEnd is not checked here but
  //       in the called method spaceTimeData::update(timeStart, timeEnd)
  for (unsigned int i=0; i<dataPointers.size(); i++) {
    try {
      dataPointers[i]->update(timeStart, timeEnd);
    } catch (except) {
      stringstream errmsg;
      errmsg << "Update of variable '" << variableNames[i] <<
        "' for target time window starting at " << timeStart.get("-",":"," ") <<
        " and ending at " << timeEnd.get("-",":"," ") << " failed.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
}

