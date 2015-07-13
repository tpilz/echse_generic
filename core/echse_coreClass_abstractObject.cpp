
#include "echse_coreClass_abstractObject.h"

////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////
abstractObject::abstractObject() {
  debugMode= false;
  idObject="";
  objectLevel=0;
  objectGroupPointer=NULL;
  osPtrSel=NULL;
  osPtrDbg=NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////
abstractObject::~abstractObject() {
  paramsNum.clear();
  paramsFun.clear();
  clear_inputsExt();
  inputsSim.clear();
  statesScal.clear();
  statesVect.clear();
  outputs.clear();
  forwardInputObjectPointers.clear();
  backwardInputObjectPointers.clear();
  objectGroupPointer=NULL;
  if (osPtrSel) {
    if (osPtrSel->is_open()) osPtrSel->close();
    delete osPtrSel;
    osPtrSel=NULL;
  }
  if (osPtrDbg) {
    if (osPtrDbg->is_open()) osPtrDbg->close();
    delete osPtrDbg;
    osPtrDbg=NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Method to delete the external inputs (vector of vectors)
////////////////////////////////////////////////////////////////////////////////
void abstractObject::clear_inputsExt() {
  if (inputsExt.size() > 0) {
    for (unsigned int i=0; i<inputsExt.size(); i++) {
      inputsExt[i].clear();
    }
    inputsExt.clear();
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set and get object id
////////////////////////////////////////////////////////////////////////////////
void abstractObject::set_idObject (const string id) {
  idObject= id;
}
string abstractObject::get_idObject() const {
  return(idObject);
}

////////////////////////////////////////////////////////////////////////////////
// Set and get object level
////////////////////////////////////////////////////////////////////////////////
void abstractObject::set_objectLevel (const unsigned int level) {
  objectLevel= level;
}
unsigned int abstractObject::get_objectLevel() const {
  return(objectLevel);
}

////////////////////////////////////////////////////////////////////////////////
// Get IDs of input objects (values are set by assign_inputsSim() method)
////////////////////////////////////////////////////////////////////////////////
const vector<abstractObject*>& abstractObject::get_forwardInputObjectPointers() const {
  return(forwardInputObjectPointers);
}
const vector<abstractObject*>& abstractObject::get_backwardInputObjectPointers() const {
  return(backwardInputObjectPointers);
}

////////////////////////////////////////////////////////////////////////////////
// Set and get pointer to object group
////////////////////////////////////////////////////////////////////////////////

void abstractObject::set_objectGroupPointer (abstractObjectGroup* p) {
  objectGroupPointer= p;
}

abstractObjectGroup* abstractObject::get_objectGroupPointer() const {
  return(objectGroupPointer);
}

////////////////////////////////////////////////////////////////////////////////
// Set and query debug mode
////////////////////////////////////////////////////////////////////////////////

bool abstractObject::debugMode_isOn() const {
  return(debugMode);
}

void abstractObject::set_debugMode(const table &tab) {
  vector<table::size_type> rownums_object;
  unsigned int colindex;
  try {
    colindex= tab.colindex(globalConst::colNames.objectID);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Missing column '" << globalConst::colNames.objectID <<
      "' in table. Selection of objects for debug output failed.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  try {
    tab.which_rows(colindex, idObject, rownums_object);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Selection of objects for debug output failed.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  if (rownums_object.size() > 0) {
    debugMode= true;
  } else {
    debugMode= false;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Get the address of a object output
////////////////////////////////////////////////////////////////////////////////

const double* abstractObject::get_outputAddress(const unsigned int index) const {
  if (outputs.size() == 0) {
    except e(__PRETTY_FUNCTION__, "Vector(s) of object outputs not yet allocated.", __FILE__, __LINE__);
    throw(e);
  }
  if (index >= outputs.size()) {
    stringstream errmsg;
    errmsg << "Cannot return address of output with index " << index <<
      " for object '" << idObject << "' which belongs to group '" <<
      objectGroupPointer->get_idObjectGroup() << "'. Index must be in" <<
      " range [0," << (outputs.size()-1) << "].";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(&outputs[index]);
}

////////////////////////////////////////////////////////////////////////////////
// Init vector of object outputs
////////////////////////////////////////////////////////////////////////////////

void abstractObject::init_outputs(const table &tab) {
  table::size_type colindex_idObj, colindex_idVar, colindex_digits;
  vector<table::size_type> rownums_object;
  string name;
  unsigned int index;
  int digits;
  try {
    // Allocate vector of object outputs
    const vector<string>& names= objectGroupPointer->get_namesOutputs();
    outputs.resize(names.size());
    // Initialize vector holding the indices of the outputs to be printed
    if (outputs.size() > 0) {
      try {
        colindex_idObj= tab.colindex(globalConst::colNames.objectID);
        colindex_idVar= tab.colindex(globalConst::colNames.variableName);
	colindex_digits= tab.colindex(globalConst::colNames.digits);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Missing column(s) in output selection table. Expected column names are '" <<
        globalConst::colNames.objectID << "', '" <<
        globalConst::colNames.variableName << "', and '" <<
        globalConst::colNames.digits << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      tab.which_rows(colindex_idObj, idObject, rownums_object);
      selectedOutputIndices.resize(rownums_object.size());
      selectedOutputDigits.resize(rownums_object.size());      
      for (unsigned int i=0; i<rownums_object.size(); i++) {
	// Set variable
        name= tab.get_element(rownums_object[i],colindex_idVar);
        try {
          index= objectGroupPointer->indexOutputs(name);
        } catch (except) {
          stringstream errmsg;
          errmsg << "Printing of unknown output variable '" << name << "' has been" <<
            " requested for object '" << idObject << "' in record " << rownums_object[i] <<
            " of the output selection table. This output variable is undefined for" <<
            " object group '" << objectGroupPointer->get_idObjectGroup() << "'.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);          
        }
        selectedOutputIndices[i]= index;
	// Set format
        try {
          digits= as_integer(tab.get_element(rownums_object[i],colindex_digits));
        } catch (except) {
          stringstream errmsg;
          errmsg << "Invalid output format for variable '" << name << "' of" <<
            " object '" << idObject << "' detected in record " << rownums_object[i] <<
            " of the output selection table. The number of digits specified in column '" <<
             globalConst::colNames.digits << "' must be a positive integer.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        selectedOutputDigits[i]= max(0, digits);
      }
    }
  } catch (except) {
    stringstream errmsg;
    errmsg << "Could not initialize outputs of object with" <<
      " id '" << idObject << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set parameter functions
////////////////////////////////////////////////////////////////////////////////

void abstractObject::init_paramsFun(const string chars_colsep, const string chars_comment) {
  unsigned int colindex_idObject;
  unsigned int colindex_idFunction;
  unsigned int colindex_file;
  unsigned int colindex_arg;
  unsigned int colindex_val;
  string colname_inFile_arg;
  string colname_inFile_val;
  string file;
  table tab_thisObject;
  vector<table::size_type> rownums_object;
  vector<table::size_type> rownums_funct;
  // Get function names and allocate functions
  const vector<string>& namesParamsFun= objectGroupPointer->get_namesParamsFun();
  paramsFun.resize(namesParamsFun.size());
  // Set functions
  if (paramsFun.size() > 0) {
    try {
      // Get table for the object group
      const table &tab_allObjects= objectGroupPointer->get_tableParamsFun();
      // Check column names
      try {
        colindex_idObject= tab_allObjects.colindex(globalConst::colNames.objectID);
        colindex_idFunction= tab_allObjects.colindex(globalConst::colNames.functionName);
        colindex_file= tab_allObjects.colindex(globalConst::colNames.fileName);
        colindex_arg= tab_allObjects.colindex(globalConst::colNames.argumentColumnName);
        colindex_val= tab_allObjects.colindex(globalConst::colNames.valueColumnName);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Could not initialize parameter function(s) for object with" <<
          " id '" << idObject << "'. Missing column(s) in the table of parameter" <<
          " functions for object group '" << objectGroupPointer->get_idObjectGroup() <<
          "'. Expecting columns '" <<
          globalConst::colNames.objectID << "', '" <<
          globalConst::colNames.functionName << "', '" <<
          globalConst::colNames.fileName << "', '" <<
          globalConst::colNames.argumentColumnName << "', and '" <<
          globalConst::colNames.valueColumnName << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Filter records using the object id
      try {
        tab_allObjects.which_rows(colindex_idObject,idObject,rownums_object);
        tab_allObjects.subset(rownums_object, tab_thisObject);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Could not initialize parameter function(s) for object with" <<
          " id '" << idObject << "'. Subsetting of the table of parameter" <<
          " functions for object group '" << objectGroupPointer->get_idObjectGroup() <<
          "' using the object id failed.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    } catch (except) {
      stringstream errmsg;
      errmsg << "Could not initialize parameter function(s) for object with" <<
        " id '" << idObject << "'. Failed to evaluate the table of parameter" <<
        " functions for object group '" << objectGroupPointer->get_idObjectGroup() << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    // Select the search column in the subset
    try {
      tab_thisObject.update_searchcolumn(colindex_idFunction);
    } catch (except) {
      stringstream errmsg;
      errmsg << "Could not initialize parameter function(s) for object with" <<
        " id '" << idObject << "'. Could not select the column containing the" <<
        " function name in the table of parameter functions for object group '" <<
        objectGroupPointer->get_idObjectGroup() << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    for (unsigned int i=0; i<paramsFun.size(); i++) {
      // Find record with function definition using the function's name
      try {
        tab_thisObject.which_rows(colindex_idFunction, namesParamsFun[i], rownums_funct);
        if (rownums_funct.size() != 1) {
          stringstream errmsg;
          errmsg << "Number of matching records for parameter function '" << 
            namesParamsFun[i] << "' and object id '" << idObject << "' is " <<
            rownums_funct.size() << " in the table of parameter functions" <<
            " for object group '" << objectGroupPointer->get_idObjectGroup() <<
            "'. Expected 1 matching record.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
      } catch (except) {
        stringstream errmsg;
        errmsg << "Could not initialize parameter function '" << 
          namesParamsFun[i] << "' for object with id '" << idObject << "'." <<
          " Subsetting of the table of parameter functions for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "' using the" <<
          " function name failed.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Get file name, and names of argument and value columns
      try {
        file= tab_thisObject.get_element(rownums_funct[0], colindex_file);
        colname_inFile_arg= tab_thisObject.get_element(rownums_funct[0], colindex_arg);
        colname_inFile_val= tab_thisObject.get_element(rownums_funct[0], colindex_val);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Could not initialize parameter function '" << 
          namesParamsFun[i] << "' for object with id '" << idObject << "'." <<
          " Failed to retrieve data from the table of parameter functions for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Read the function data
      try {
        paramsFun[i].read(file, true, chars_colsep, chars_comment,
          namesParamsFun[i], colname_inFile_arg, colname_inFile_val);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Could not initialize parameter function '" << 
          namesParamsFun[i] << "' for object with id '" << idObject << "'." <<
          " Initialization using the argument column '" << colname_inFile_arg <<
          "' and the values column '" << colname_inFile_val << "' of file '" <<
          file << "' failed.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set scalar parameters
////////////////////////////////////////////////////////////////////////////////

void abstractObject::init_paramsNum() {
  table::size_type colindex_idObject, colindex_param;
  table::size_type rowindex_object;
  // Get parameter names and allocate parameters
  const vector<string>& namesParamsNum= objectGroupPointer->get_namesParamsNum();
  paramsNum.resize(namesParamsNum.size());
  // Set parameters
  if (paramsNum.size() > 0) {
    try {
      // Get table for the object group and filter records using the object id
      const table &tab= objectGroupPointer->get_tableParamsNum();
      try {
        colindex_idObject= tab.colindex(globalConst::colNames.objectID);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Missing column '" << globalConst::colNames.objectID << "' in table of" <<
          " scalar parameters for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      rowindex_object= tab.first_row(colindex_idObject, idObject);
      if (rowindex_object == 0) {
        stringstream errmsg;
        errmsg << "Missing record for object with id '" << idObject <<
          "' in table of scalar parameters for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Loop through parameters
      for (unsigned int i=0; i<paramsNum.size(); i++) {
        // Find column
        try {
          colindex_param= tab.colindex(namesParamsNum[i]);
        } catch (except) {
          stringstream errmsg;
          errmsg << "Missing column '" << namesParamsNum[i] << "' in table of" <<
            " scalar parameters for object group '" <<
            objectGroupPointer->get_idObjectGroup() << "'.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        // Assign value
        try {
          paramsNum[i]= as_double(tab.get_element(rowindex_object, colindex_param));
        } catch (except) {
          stringstream errmsg;
          errmsg << "Could not set value of parameter '" << namesParamsNum[i] <<
            "' for object with id '" << idObject << "'. The corresponding value" <<
            " in the table of scalar parameters for object group '" <<
            objectGroupPointer->get_idObjectGroup() << "' is invalid.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        } 
      }
    } catch (except) {
      stringstream errmsg;
      errmsg << "Could not set scalar parameters for object with" <<
        " id '" << idObject << "'. Errors were detected in the table for object" <<
        " group '" << objectGroupPointer->get_idObjectGroup() << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set simulated inputs (linkage of objects)
////////////////////////////////////////////////////////////////////////////////

void abstractObject::assign_inputsSim(const table &tab,
  const vector<abstractObject*> &objects
) {
  typedef map<string, unsigned int> name_index_map;
  typedef pair<string, unsigned int> name_index_pair;
  table::size_type colindex_idTargetObj, colindex_idSourceObj,
    colindex_idTargetVar, colindex_idSourceVar, colindex_forwardType;
  vector<table::size_type> rowinds_object;
  vector<string>::const_iterator iter;
  vector<string>::difference_type pos_targetVar, pos_sourceVar;
  unsigned int pos_sourceObj;
  static name_index_map id2indexMap;
  static bool id2indexMap_isInitialized=false;
  bool forwardType;
  try {
    // Allocate simulated inputs
    const vector<string>& namesInputsSim= objectGroupPointer->get_namesInputsSim();
    inputsSim.resize(namesInputsSim.size());
    if (namesInputsSim.size() > 0) {
      // Determine position of required columns in table
      try {
        colindex_idTargetObj= tab.colindex(globalConst::colNames.targetObjectID);
        colindex_idTargetVar= tab.colindex(globalConst::colNames.targetVariableName);
        colindex_idSourceObj= tab.colindex(globalConst::colNames.sourceObjectID);
        colindex_idSourceVar= tab.colindex(globalConst::colNames.sourceVariableName);
        colindex_forwardType= tab.colindex(globalConst::colNames.forwardType);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Missing or badly named column(s) in object linkage table." <<
          " Expecting columns '" <<
          globalConst::colNames.targetObjectID << "', '" <<
          globalConst::colNames.targetVariableName << "', '" <<
          globalConst::colNames.sourceObjectID << "', '" <<
          globalConst::colNames.sourceVariableName << "', and '" <<
          globalConst::colNames.forwardType << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Subset table by object id
      tab.which_rows(colindex_idTargetObj, idObject, rowinds_object);
      if (rowinds_object.size() != namesInputsSim.size()) {
        stringstream errmsg;
        errmsg << "Found " << rowinds_object.size() << " record(s) for object" <<
          " with id '" << idObject << "' in the object linkage table but " <<
          namesInputsSim.size() << " record(s) are expected for objects of" <<
          " object group '" << objectGroupPointer->get_idObjectGroup() <<
          "'. There must be one record for each simulated input required by" <<
          " a object of this group."; 
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Loop through records
      for (unsigned int i=0; i<rowinds_object.size(); i++) {
        // Determine the index in this object's array of simulated inputs
        // Check whether the current object actually requires that input
        iter= find(namesInputsSim.begin(), namesInputsSim.end(),
          tab.get_element(rowinds_object[i], colindex_idTargetVar));

        if (iter == namesInputsSim.end()) {
          stringstream errmsg;
          errmsg << "Error in object linkage table at record " <<
            rowinds_object[i] << ". The object with id '" << idObject  << "' belongs" <<
            " to object group '" << objectGroupPointer->get_idObjectGroup() <<
            "'. This group does not require a simulated input '" <<
            tab.get_element(rowinds_object[i], colindex_idTargetVar) << "'.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        pos_targetVar= distance(namesInputsSim.begin(), iter);
        // Determine index of the source object and check that it is not identical
        // with the target object (loop). To speed up the lookup of the object index
        // a map is used which is initialized on the first call of this function.
        // We rely on (and this seems legitimate) that the order of the objects in
        // the objects vector does not change between function calls.
        if (!id2indexMap_isInitialized) {
          pair<name_index_map::const_iterator, bool> insert_status;
          for (unsigned int k=0; k<objects.size(); k++) {
            insert_status= id2indexMap.insert(name_index_pair(objects[k]->get_idObject(),k));
            if (!insert_status.second) {
              except e(__PRETTY_FUNCTION__, "Cannot create map of object IDs.", __FILE__, __LINE__);
              throw(e);
            }
          }
          id2indexMap_isInitialized= true;
        }
        name_index_map::const_iterator id2indexMap_iter= id2indexMap.find(
          tab.get_element(rowinds_object[i], colindex_idSourceObj));
        if (id2indexMap_iter == id2indexMap.end()) {
          stringstream errmsg;
          errmsg << "Error in object linkage table at record " <<
            rowinds_object[i] << ". Cannot determine index of object with ID '" <<
            tab.get_element(rowinds_object[i], colindex_idSourceObj) <<
            "'. A object with that ID does not exist (has not been declared) and," <<
            " therefore, cannot provide input for object '" << idObject << "'.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        pos_sourceObj= id2indexMap_iter->second;
        if (objects[pos_sourceObj]->get_idObject() == idObject) {
          stringstream errmsg;
          errmsg << "Error in object linkage table at record " <<
            rowinds_object[i] << ". The object with ID '" << idObject  << "' cannot" <<
            " use its own output as input.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        // Register source objects. This info is required for fast determination
        // of the object level in a separate function
        // Notes:
        // - The source objects are registered separately for 'forward' and
        //   'backward' relations. Reason: For determination of the level,
        //   only forward relations count.
        // - Each source object is registered ONLY ONCE (even if the target
        //   object uses more than one output variable of the source object).
        // - A source object can only be part of a forward _OR_ backward relation
        //   (i.e. the relation type must be the same for all exchanged variables)
        try {
          forwardType= as_logical(tab.get_element(rowinds_object[i],colindex_forwardType));
        } catch (except) {
          stringstream errmsg;
          errmsg << "Error in object linkage table at record " << rowinds_object[i] <<
            ". Expecting a logical value in column '" << globalConst::colNames.forwardType << "'.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        if (forwardType) {
          if (forwardInputObjectPointers.size() == 0) {
            forwardInputObjectPointers.push_back(objects[pos_sourceObj]);        
          } else {
            vector<abstractObject*>::const_iterator it= find(
              forwardInputObjectPointers.begin(), forwardInputObjectPointers.end(),
              objects[pos_sourceObj]);
            if (it == forwardInputObjectPointers.end()) {
              forwardInputObjectPointers.push_back(objects[pos_sourceObj]);
            }
          }
          // Already registered in a backward relation?
          vector<abstractObject*>::const_iterator it= find(
            backwardInputObjectPointers.begin(), backwardInputObjectPointers.end(),
            objects[pos_sourceObj]);
          if (it != backwardInputObjectPointers.end()) {
            stringstream errmsg;
            errmsg << "Error in object linkage table at record " << rowinds_object[i] <<
              ". Object '" << idObject << "' cannot get input from object '" << 
              tab.get_element(rowinds_object[i], colindex_idSourceObj) <<
              "' in a forward relation because a backward relation has already been" <<
              " declared for another exchanged variable.";
            except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
            throw(e);
          }
        } else {
          if (backwardInputObjectPointers.size() == 0) {
            backwardInputObjectPointers.push_back(objects[pos_sourceObj]);        
          } else {
            vector<abstractObject*>::const_iterator it= find(
              backwardInputObjectPointers.begin(), backwardInputObjectPointers.end(),
              objects[pos_sourceObj]);
            if (it == backwardInputObjectPointers.end()) {
              backwardInputObjectPointers.push_back(objects[pos_sourceObj]);
            }
          }
          // Already registered in a forward relation?
          vector<abstractObject*>::const_iterator it= find(
            forwardInputObjectPointers.begin(), forwardInputObjectPointers.end(),
            objects[pos_sourceObj]);
          if (it != forwardInputObjectPointers.end()) {
            stringstream errmsg;
            errmsg << "Error in object linkage table at record " << rowinds_object[i] <<
              ". Object '" << idObject << "' cannot get input from object '" << 
              tab.get_element(rowinds_object[i], colindex_idSourceObj) <<
              "' in a backward relation because a forward relation has already been" <<
              " declared for another exchanged variable.";
            except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
            throw(e);
          }
        }
        // Determine index of the source object's output
        // Check whether the source object supplies the requested output
        const vector<string>& namesOutputs= objects[pos_sourceObj]->
          get_objectGroupPointer()->get_namesOutputs();
        iter= find(namesOutputs.begin(), namesOutputs.end(),
          tab.get_element(rowinds_object[i], colindex_idSourceVar));
        if (iter == namesOutputs.end()) {
          stringstream errmsg;
          errmsg << "Error in object linkage table at record " <<
            rowinds_object[i] << ". The object with id '" << idObject  << "' cannot" <<
            " use output '" << 
            tab.get_element(rowinds_object[i], colindex_idSourceVar) <<
            "' of the object '" <<
            tab.get_element(rowinds_object[i], colindex_idSourceObj) << "' as" <<
            " input. The latter object belongs to object group '" <<
            objects[pos_sourceObj]->get_objectGroupPointer()->get_idObjectGroup() <<
            "' and does not supply the requested output.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        pos_sourceVar= distance(namesOutputs.begin(), iter);
        // Establish the link between the target variable of this object and
        // the source variable of the source object (by setting the pointer)
        inputsSim[pos_targetVar]= objects[pos_sourceObj]->get_outputAddress(pos_sourceVar);
      } // End of loop over simulated inputs
    }
  } catch (except) {
    stringstream errmsg;
    errmsg << "Failed to assign simulated object inputs.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set external inputs (external boundary conditions)
////////////////////////////////////////////////////////////////////////////////

void abstractObject::assign_inputsExt(const table &tab,
  const spaceTimeDataCollection &externalInputs) {
  table::size_type colindex_idObj, colindex_idVar, colindex_idWgt, colindex_idLoc;
  vector<table::size_type> rowinds_object;
  vector<string>::const_iterator iter;
  vector<string>::difference_type pos_target, pos_sourceVar, pos_sourceLoc;
  struct weightedValue link;
  try {
    // Initial clean-up
    clear_inputsExt();
    // Allocate first dimension of vector
    const vector<string>& namesInputsExt= objectGroupPointer->get_namesInputsExt();
    inputsExt.resize(namesInputsExt.size());
    if (namesInputsExt.size() > 0) {
      // Determine position of required items in table records
      try {
        colindex_idObj= tab.colindex(globalConst::colNames.objectID);
        colindex_idVar= tab.colindex(globalConst::colNames.variableName);
        colindex_idWgt= tab.colindex(globalConst::colNames.weight);
        colindex_idLoc= tab.colindex(globalConst::colNames.locationID);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Missing or badly named column(s) in locations assignment table." <<
          " Expecting columns '" <<
          globalConst::colNames.objectID << "', '" <<
          globalConst::colNames.variableName << "', '" <<
          globalConst::colNames.weight << "', and '" <<
          globalConst::colNames.locationID << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Filter table by object id
      tab.which_rows(colindex_idObj, idObject, rowinds_object);
      if (rowinds_object.size() == 0) {
        stringstream errmsg;
        errmsg << "No record(s) found for object with id '" << idObject <<
          "' in the locations assignment table. This object is of group '" <<
          objectGroupPointer->get_idObjectGroup() <<
          "' and, therefore, external input(s) must be specified."; 
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Loop through this object's records
      for (unsigned int i=0; i<rowinds_object.size(); i++) {
        // Determine the index in this object's array of external inputs
        // Check whether the current object actually requires that input
        iter= find(namesInputsExt.begin(), namesInputsExt.end(),
          tab.get_element(rowinds_object[i], colindex_idVar));
        if (iter == namesInputsExt.end()) {
          stringstream errmsg;
          errmsg << "Error in locations assignment table at record " <<
            rowinds_object[i] << ". The object with id '" << idObject  << "' belongs" <<
            " to object group '" << objectGroupPointer->get_idObjectGroup() <<
            "'. This group does not require an external input '" <<
            tab.get_element(rowinds_object[i], colindex_idVar) << "'.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        pos_target= distance(namesInputsExt.begin(), iter);
        // Determine the index of the source variable in the externalInputs object
        const vector<string>& namesSourceVar= externalInputs.get_variableNames();
        iter= find(namesSourceVar.begin(), namesSourceVar.end(),
          tab.get_element(rowinds_object[i], colindex_idVar));
        if (iter == namesSourceVar.end()) {
          stringstream errmsg;
          errmsg << "The object with id '" << idObject << "' belongs to object group '" <<
            objectGroupPointer->get_idObjectGroup() << "' and requires external" <<
            " input for variable '" << tab.get_element(rowinds_object[i], colindex_idVar) <<
            "'. The input is not available, possibly because no time series file has been" <<
            " specified for this variable.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        pos_sourceVar= distance(namesSourceVar.begin(), iter);
        // Determine the index of the source location
        const vector<string>& namesSourceLoc= externalInputs.get_locations(pos_sourceVar);
        iter= find(namesSourceLoc.begin(), namesSourceLoc.end(),
          tab.get_element(rowinds_object[i], colindex_idLoc));
        if (iter == namesSourceLoc.end()) {
          stringstream errmsg;
          errmsg << "The object with id '" << idObject << "' (object group '" <<
            objectGroupPointer->get_idObjectGroup() << "') requires external" <<
            " input for variable '" << tab.get_element(rowinds_object[i], colindex_idVar) <<
            "' at location '" << tab.get_element(rowinds_object[i], colindex_idLoc) <<
            "' according to record " << rowinds_object[i] << " of the locations" <<
            " assignment table. Data for this location are not available in the" <<
            " corresponding time series file (unknown location).";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        pos_sourceLoc= distance(namesSourceLoc.begin(), iter);
        // Establish the link between the target variable of this object and
        // the source variable at the source location (by setting pointer and weight)
        link.valPtr= externalInputs.get_address(pos_sourceVar, pos_sourceLoc);
        try {
          link.weight= as_double(tab.get_element(rowinds_object[i], colindex_idWgt));
        } catch (except) {
          stringstream errmsg;
          errmsg << "Weight for location '" << tab.get_element(rowinds_object[i], colindex_idLoc) << 
            "' specified in record " << rowinds_object[i] << " of the locations" <<
            " assignment table is non-numeric. Could not assign external input" <<
            " to variable '" << namesInputsExt[i] << "' of object with id '" <<
            idObject << "' (object group '" << objectGroupPointer->get_idObjectGroup() << "').";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        inputsExt[pos_target].push_back(link);
      } // End of loop over this object's records
      // Make sure that data of at least one location were assigned to all variables
      for (unsigned int i=0; i<inputsExt.size(); i++) {
        if (inputsExt[i].size() == 0) {
          stringstream errmsg;
          errmsg << "No record(s) found for variable '" << namesInputsExt[i] <<
            "' of object with id '" << idObject <<
            "' (object group '" << objectGroupPointer->get_idObjectGroup() <<
            "') in locations assignment table. Expecting one or more record(s)" <<
            " corresponding to one or more location(s).";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
      }
    }
  } catch (except) {
    stringstream errmsg;
    errmsg << "Failed to assign external object inputs.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}


////////////////////////////////////////////////////////////////////////////////
// Initialization of scalar state variables
////////////////////////////////////////////////////////////////////////////////
void abstractObject::init_statesScal(const table &tab) {
  table::size_type colindex_idObj, colindex_idVar, colindex_value;
  vector<table::size_type> rowinds_object;
  vector<string>::const_iterator iter;
  vector<string>::difference_type pos_target;
  try {
    // Allocate vector of scalar states
    const vector<string>& namesStatesScal= objectGroupPointer->get_namesStatesScal();
    statesScal.resize(namesStatesScal.size());
    // Set initial values of scalar states
    if (statesScal.size() > 0) {
      try {
        colindex_idObj= tab.colindex(globalConst::colNames.objectID);
        colindex_idVar= tab.colindex(globalConst::colNames.variableName);
        colindex_value= tab.colindex(globalConst::colNames.value);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Missing column(s) in table of scalar state variables' initial values." <<
          " Expecting columns '" <<
          globalConst::colNames.objectID << "', '" <<
          globalConst::colNames.variableName << "', and '" <<
          globalConst::colNames.value << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      tab.which_rows(colindex_idObj, idObject, rowinds_object);
      if (rowinds_object.size() != namesStatesScal.size()) {
        stringstream errmsg;
        errmsg << "Found " << rowinds_object.size() << " record(s) for object" <<
          " with id '" << idObject << "' in the table of scalar state variables'" <<
          " initial values but " << namesStatesScal.size() << " record(s) are" <<
          " expected for objects of object group '" <<
          objectGroupPointer->get_idObjectGroup() <<
          "'. There must be one record for each scalar state variable."; 
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      for (unsigned int i=0; i<rowinds_object.size(); i++) {
        // Determine the index in this object's array of scalar states
        iter= find(namesStatesScal.begin(), namesStatesScal.end(),
          tab.get_element(rowinds_object[i], colindex_idVar));
        if (iter == namesStatesScal.end()) {
          stringstream errmsg;
          errmsg << "Error in table of scalar state variables' initial values at record " <<
            rowinds_object[i] << ". The object with id '" << idObject  << "' belongs" <<
            " to object group '" << objectGroupPointer->get_idObjectGroup() <<
            "'. Objects of this group do not use a scalar state variable '" <<
            tab.get_element(rowinds_object[i], colindex_idVar) << "'.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        pos_target= distance(namesStatesScal.begin(), iter);
        try {
          statesScal[pos_target]= as_double(tab.get_element(rowinds_object[i], colindex_value));
        } catch (except) {
          stringstream errmsg;
          errmsg << "Error in table of scalar state variables' initial values at record " <<
            rowinds_object[i] << ". The value to be assigned to the scalar state variable '" <<
            tab.get_element(rowinds_object[i], colindex_idVar) << "' of object '" <<
            idObject  << "' is invalid.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
      }
    }
  } catch (except) {
    stringstream errmsg;
    errmsg << "Could not initialize scalar state variables of object with" <<
      " id '" << idObject << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Initialization of vector state variables
////////////////////////////////////////////////////////////////////////////////
void abstractObject::init_statesVect(const table &tab) {
  table::size_type colindex_idObj, colindex_idVar, colindex_index, colindex_value;
  vector<table::size_type> rowinds;
  table tab_subset;
  vector<double> tmp;
  unsigned int index;
  try {
    // Initial clean-up
    statesVect.clear();
    // Get names of vector states
    const vector<string>& namesStatesVect= objectGroupPointer->get_namesStatesVect();
    if (namesStatesVect.size() > 0) {
      try {
        colindex_idObj= tab.colindex(globalConst::colNames.objectID);
        colindex_idVar= tab.colindex(globalConst::colNames.variableName);
        colindex_index= tab.colindex(globalConst::colNames.index);
        colindex_value= tab.colindex(globalConst::colNames.value);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Missing column(s) in table of vector state variables' initial values." <<
          " Expecting columns '" <<
          globalConst::colNames.objectID << "', '" <<
          globalConst::colNames.variableName << "', '" <<
          globalConst::colNames.index << "', and '" <<
          globalConst::colNames.value << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Filter records for this object
      tab.which_rows(colindex_idObj, idObject, rowinds);
      if (rowinds.size() < namesStatesVect.size()) {
        stringstream errmsg;
        errmsg << "Found " << rowinds.size() << " record(s) for object" <<
          " with id '" << idObject << "' in the table of vector state variables'" <<
          " initial values but at least " << namesStatesVect.size() <<
          " record(s) are expected for objects of object group '" <<
          objectGroupPointer->get_idObjectGroup() <<
          "'. There must be one or more record(s) for each vector state variable."; 
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Save subset as table
		  tab.subset(rowinds, tab_subset);
      // Select search column in the subset
      tab_subset.update_searchcolumn(colindex_idVar);  // Can't fail as index was already checked
      // Loop through state variables and filter for them
      for (unsigned int i=0; i<namesStatesVect.size(); i++) {
        tab_subset.which_rows(colindex_idVar, namesStatesVect[i], rowinds);
        if (rowinds.size() == 0) {
          stringstream errmsg;
          errmsg << "Missing initialization data for vector state variable '" <<
            namesStatesVect[i] << "' of object '" << idObject << "' in the table" <<
            " of vector state variables' initial values. Objects of group '" <<
            objectGroupPointer->get_idObjectGroup() <<
            "' require at least one initial value for this vector state variable.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        // Loop through records for current state variable
        tmp.resize(rowinds.size());
        for (unsigned int k=0; k<rowinds.size(); k++) {
          try {
            index= as_unsigned_integer(tab_subset.get_element(rowinds[k], colindex_index));
          } catch (except) {
            stringstream errmsg;
            errmsg << "Invalid index detected for vector state variable '" <<
              namesStatesVect[i] << "' of object '" << idObject << "' in the table" <<
              " of vector state variables' initial values. Expecting unsigned integer.";
            except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
            throw(e);
          }
          if (index != k) {
            stringstream errmsg;
            errmsg << "Bad index (" << index << ") detected for vector state variable '" <<
              namesStatesVect[i] << "' of object '" << idObject << "' in the table" <<
              " of vector state variables' initial values. Indices must start at" <<
              " zero and, if multiple values are given, increase with the record number" <<
              " using a step size of one.";
            except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
            throw(e);
          }
          try {
            tmp[k]= as_double(tab_subset.get_element(rowinds[k], colindex_value));
          } catch (except) {
            stringstream errmsg;
            errmsg << "Error in table of vector state variables' initial values." <<
              " The value to be assigned to the element with index " << k <<
              " of vector state variable '" << namesStatesVect[i] << "' of object '" <<
              idObject  << "' is invalid.";
            except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
            throw(e);
          }
        }
        // Save values and clear temporarys
        statesVect.add(tmp);        
        tmp.clear();
        tab_subset.clear();
      } // End of loop over state variables
    }
  } catch (except) {
    stringstream errmsg;
    errmsg << "Could not initialize vector state variables of object with" <<
      " id '" << idObject << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Print selected output
////////////////////////////////////////////////////////////////////////////////

void abstractObject::output_selected(const bool firstCall, const bool finalCall,
  const string &outdir, const string &outfmt,
  const string chars_colsep, const string &timestamp,
  const unsigned int timestep
) {
  unsigned int nItems;
  // Determine number of output items
  nItems= selectedOutputIndices.size();
  if (nItems > 0) {
    // On first use
    if (firstCall) {
      // Build file name and check existance
      string file;
      if (outfmt == "tab") {
         file= outdir + "/" + idObject + globalConst::fileExtensions.tabular;
      } else if (outfmt == "json") {
         file= outdir + "/" + idObject + globalConst::fileExtensions.json;
      } else {
        stringstream errmsg;
        errmsg << "Requested output file format '" << outfmt <<
	  "' is unknown. Currently supported formats are 'tab' for" <<
          " TAB-separated text or 'json' for Java-Script Object Notation.";
	except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
	throw(e);
      }
      if (file_exists(file)) {
        stringstream errmsg;
        errmsg << "Cannot print selected output for object '" << idObject <<
          "' to file '" << file << "'. File already exists.";
	except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
	throw(e);
      }
      // Set pointer to output stream
      try {
        osPtrSel= new(nothrow) ofstream;
      } catch (except) {
        stringstream errmsg;
        errmsg << "Cannot print selected output for object '" << idObject <<
          "' to file '" << file << "'. Allocation of output stream failed.";
		    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		    throw(e);
      }
      // Open stream
      osPtrSel->open(file.c_str());
      if (!osPtrSel->is_open()) {
        stringstream errmsg;
        errmsg << "Cannot print selected output for object '" << idObject <<
          "' to file '" << file << "'. File cannot be opened.";
	except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
	throw(e);
      }
      // Print header if neccessary
      if (outfmt == "tab") {
	*osPtrSel << "end_of_interval" << chars_colsep;
	if (nItems > 1) {
	  for (vector<string>::size_type i=0; i<(nItems-1); i++) *osPtrSel << 
	    objectGroupPointer->get_namesOutputs()[selectedOutputIndices[i]] << chars_colsep;
	}
	*osPtrSel << objectGroupPointer->get_namesOutputs()[selectedOutputIndices[nItems-1]];
	*osPtrSel << endl;
      } else if (outfmt == "json") {
	*osPtrSel << "{" << endl;
	*osPtrSel << "\"object\": \"" << idObject << "\"," << endl;
	*osPtrSel << "\"variables\": [" << endl;
	if (nItems > 1) {
	  for (vector<string>::size_type i=0; i<(nItems-1); i++) *osPtrSel << "\"" <<
	    objectGroupPointer->get_namesOutputs()[selectedOutputIndices[i]] << "\",";
	}
	*osPtrSel << '"' << objectGroupPointer->get_namesOutputs()[selectedOutputIndices[nItems-1]] << "\"";
	*osPtrSel << endl << "]," << endl;
	*osPtrSel << "\"end_of_first_interval\": \"" << timestamp << "\"," << endl;
	*osPtrSel << "\"seconds_per_interval\": " << timestep << "," << endl;
	*osPtrSel << "\"data\": [" << endl;
      } else {
        stringstream errmsg;
        errmsg << "Output format not supported. Please report this bug!";
	except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
	throw(e);
      }
    }
    // Print values
    if (outfmt == "tab") {
      *osPtrSel << timestamp << chars_colsep;
      if (nItems > 1) {
	for (vector<double>::size_type i=0; i<(nItems-1); i++) *osPtrSel <<
	  std::fixed << std::setprecision(selectedOutputDigits[i]) <<
	  outputs[selectedOutputIndices[i]] << chars_colsep;
      }
      *osPtrSel << std::fixed << std::setprecision(selectedOutputDigits[nItems-1]) <<
	 outputs[selectedOutputIndices[nItems-1]];
      *osPtrSel << endl;
    } else if (outfmt == "json") {
      if (!firstCall)
        *osPtrSel << "," << endl;
      if (nItems > 1) {
	for (vector<double>::size_type i=0; i<(nItems-1); i++) *osPtrSel <<
	  std::fixed << std::setprecision(selectedOutputDigits[i]) <<
	  outputs[selectedOutputIndices[i]] << ",";
      }
      *osPtrSel << std::fixed << std::setprecision(selectedOutputDigits[nItems-1]) <<
	 outputs[selectedOutputIndices[nItems-1]];
      if (finalCall)
        *osPtrSel << endl << "]" << endl << "}" << endl;
    } else {
      stringstream errmsg;
      errmsg << "Output format not supported. Please report this bug!";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
    // Let the destructor clean up (close file, free memory, ...)
  }
}

////////////////////////////////////////////////////////////////////////////////
// Print debug output
////////////////////////////////////////////////////////////////////////////////

void abstractObject::output_debug(const bool firstCall, const string &outdir,
  const string chars_colsep, const string &timestamp) {
  // If debug output has been requested
  if (debugMode) {
    // On first use
    if (firstCall) {
      // Build file name and check existance
      string file= outdir + "/" + idObject + globalConst::fileExtensions.debug;
      if (file_exists(file)) {
        stringstream errmsg;
        errmsg << "Cannot print debug output for object '" << idObject <<
          "' to file '" << file << "'. File already exists.";
		    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		    throw(e);
      }
      // Set pointer to output stream
      try {
        osPtrDbg= new(nothrow) ofstream;
      } catch (except) {
        stringstream errmsg;
        errmsg << "Cannot print debug output for object '" << idObject <<
          "' to file '" << file << "'. Allocation of output stream failed.";
		    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		    throw(e);
      }
      // Open stream
      osPtrDbg->open(file.c_str());
      if (!osPtrDbg->is_open()) {
        stringstream errmsg;
        errmsg << "Cannot print debug output for object '" << idObject <<
          "' to file '" << file << "'. File cannot be opened.";
		    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		    throw(e);
      }
      // Print header if neccessary
      *osPtrDbg << "end_of_interval" << chars_colsep << "type_of_item" << chars_colsep <<
                   "item_name" << chars_colsep << "index" << chars_colsep << "value";
      *osPtrDbg << endl;
    }
    // Scalar states
    try {
      const vector<string>& names= objectGroupPointer->get_namesStatesScal();
      if (names.size() > 0) {
        for (vector<string>::size_type i=0; i<names.size(); i++) {
          *osPtrDbg << timestamp << chars_colsep << "stateScal" << chars_colsep <<
            names[i] << chars_colsep << "0" << chars_colsep << 
            scientific << setprecision(3) << statesScal[i] << endl;
        }
      }
    } catch (except) {
      stringstream errmsg;
      errmsg << "Cannot print debug output for object '" << idObject <<
        "'. Output of scalar states failed.";
	    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
	    throw(e);
    }
    // Vector states
    try {
      const vector<string>& names= objectGroupPointer->get_namesStatesVect();
      if (names.size() > 0) {
        for (vector<string>::size_type i=0; i<names.size(); i++) {
          const vector<double>& values= statesVect.read_access(i);
          for (vector<string>::size_type k=0; k<values.size(); k++) {
            *osPtrDbg << timestamp << chars_colsep << "stateVect" << chars_colsep <<
              names[i] << chars_colsep << k << chars_colsep <<
              scientific << setprecision(3) << values[k] << endl;
          }
        }
      }
    } catch (except) {
      stringstream errmsg;
      errmsg << "Cannot print debug output for object '" << idObject <<
        "'. Output of vector states failed.";
	    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
	    throw(e);
    }
    // External inputs
    try {
      const vector<string>& names= objectGroupPointer->get_namesInputsExt();
      if (names.size() > 0) {
        double result;
        for (vector<string>::size_type i=0; i<names.size(); i++) {
          result= 0.;
          for (unsigned int k=0; k<inputsExt[i].size(); k++) {
            result= result + (*inputsExt[i][k].valPtr) * inputsExt[i][k].weight;
          }
          *osPtrDbg << timestamp << chars_colsep << "inputExt" << chars_colsep <<
            names[i] << chars_colsep << "0" << chars_colsep << 
            scientific << setprecision(3) << result << endl;
        }
      }
    } catch (except) {
      stringstream errmsg;
      errmsg << "Cannot print debug output for object '" << idObject <<
        "'. Output of external inputs failed.";
	    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
	    throw(e);
    }
    // Simulated inputs
    try {
      const vector<string>& names= objectGroupPointer->get_namesInputsSim();
      if (names.size() > 0) {
        for (vector<string>::size_type i=0; i<names.size(); i++) {
          *osPtrDbg << timestamp << chars_colsep << "inputSim" << chars_colsep <<
            names[i] << chars_colsep << "0" << chars_colsep << 
            scientific << setprecision(3) << inputsSim[i] << endl;
        }
      }
    } catch (except) {
      stringstream errmsg;
      errmsg << "Cannot print debug output for object '" << idObject <<
        "'. Output of simulated inputs failed.";
	    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
	    throw(e);
    }
    // Outputs
    try {
      const vector<string>& names= objectGroupPointer->get_namesOutputs();
      if (names.size() > 0) {
        for (vector<string>::size_type i=0; i<names.size(); i++) {
          *osPtrDbg << timestamp << chars_colsep << "output" << chars_colsep <<
            names[i] << chars_colsep << "0" << chars_colsep << 
            scientific << setprecision(3) << outputs[i] << endl;
        }
      }
    } catch (except) {
      stringstream errmsg;
      errmsg << "Cannot print debug output for object '" << idObject <<
        "'. Output of output variables failed.";
	    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
	    throw(e);
    }
    // Let the destructor clean up (close file, free memory, ...)
  }
}

////////////////////////////////////////////////////////////////////////////////
// Methods to explicitly close the output files
////////////////////////////////////////////////////////////////////////////////

void abstractObject::closeOutput_selected() {
  if (osPtrSel) {
    if (osPtrSel->is_open()) osPtrSel->close();
    delete osPtrSel;
    osPtrSel=NULL;
  }
}
void abstractObject::closeOutput_debug() {
  if (osPtrDbg) {
    if (osPtrDbg->is_open()) osPtrDbg->close();
    delete osPtrDbg;
    osPtrDbg=NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Method to check an object for invalid numerical values
////////////////////////////////////////////////////////////////////////////////
void abstractObject::checkFPE() const {
  unsigned int n;
  bool ex;
  ex= false;
  // Scalar state variables
  n= statesScal.size();
  if (n > 0) {
    for (unsigned int i=0; i<n; i++) {
      if (!isfinite(statesScal[i])) {
        stringstream errmsg;
        errmsg << "Invalid numerical value detected for scalar state variable '" <<
           get_objectGroupPointer()->get_namesStatesScal()[i] << "' of object '" <<
           idObject << "'.";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        ex= true;
      }
    }
  }
  // Vector state variables
  n= statesVect.nvars();
  if (n > 0) {
    for (unsigned int i=0; i<n; i++) {
      const vector<double>& v= statesVect.read_access(i);
      for (unsigned int k=0; k<v.size(); k++) {
        if (!isfinite(v[k])) {
          stringstream errmsg;
          errmsg << "Invalid numerical value detected for element with index " <<
             k << " of vector state variable '" <<
             get_objectGroupPointer()->get_namesStatesVect()[i] << "' of object '" <<
             idObject << "'.";
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          ex= true;
        }
      }
    }
  }
  // Output variables
  n= outputs.size();
  if (n > 0) {
    for (unsigned int i=0; i<n; i++) {	
      if (!isfinite(outputs[i])) {
        stringstream errmsg;
        errmsg << "Invalid numerical value detected for output variable '" <<
           get_objectGroupPointer()->get_namesOutputs()[i] << "' of object '" <<
           idObject << "'.";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        ex= true;
      }
    }
  }
  if (ex) {
    stringstream errmsg;
    errmsg << "Invalid numerical value(s) occurred in object '" <<
       idObject << "'. To get more info about the circumstances, re-run" <<
       " with debug output enabled for this object. Note that debug output cannot" <<
       " be created, however, for exceptions occurring in the very 1st time step.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}	

