
#ifndef ECHSE_CORECLASS_ABSTRACTOBJECT_H
#define ECHSE_CORECLASS_ABSTRACTOBJECT_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <cmath>

#include "except/except.h"
#include "table/table.h"
#include "functions/functions.h"
#include "typeconv/typeconv.h"

#include "echse_coreClass_abstractObjectGroup.h"
#include "echse_coreClass_spaceTimeDataCollection.h"
#include "echse_coreClass_multiState.h"
#include "echse_options.h"
#include "echse_globalConst.h"

#ifndef CHECK_RANGE
#  error "Macro CHECK_RANGE is undefined."
#endif

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Index types for access to an objects data vectors via the public access
// methods used by the developer of a specific object (= user interface).
// Note:
// Structs are used here to get 'opaque' types, i.e. to make the index specific
// for a particular vector (so that the index for one vector cannot be used for
// another vector). Note that this cannot be achieved by 'typedef' because the
// types created by 'typedef' are 'transparent', i.e. they are interchangeable
// if they point to the same base type (like unsigned int, for example).
////////////////////////////////////////////////////////////////////////////////

struct T_index_stateScal { unsigned int index; };
struct T_index_stateVect { unsigned int index; };
struct T_index_paramNum { unsigned int index; };
struct T_index_paramFun { unsigned int index; };
struct T_index_sharedParamNum { unsigned int index; };
struct T_index_sharedParamFun { unsigned int index; };
struct T_index_inputExt { unsigned int index; };
struct T_index_inputSim { unsigned int index; };
struct T_index_output { unsigned int index; };

////////////////////////////////////////////////////////////////////////////////
// Abstract base class 'abstractObject'
////////////////////////////////////////////////////////////////////////////////

class abstractObject {
  private:
    // Type for external inputs
    struct weightedValue {
      const double* valPtr;
      double weight;
    };
    // Object id
    string idObject;
    // Object level: This is an index which may be shared by a group of objects.
    //              Objects which share the same level can be simulated in parallel.
    unsigned int objectLevel;
    // Pointer to object group
    abstractObjectGroup* objectGroupPointer;
    // Object inputs, states, and outputs
    vector<double> paramsNum;
    vector<tblFunction> paramsFun;
    vector< vector<weightedValue> > inputsExt;
    vector<const double*> inputsSim;
    vector<double> statesScal;
    multiState statesVect;
    vector<double> outputs;
    // Vector of input objects: Keeping this info (1) speeds up determination of
    //                         the object level and (2) allows for checking whether
    //                         the selection of objects for simulation is reasonable. 
    vector<abstractObject*> forwardInputObjectPointers;  // Inputs from upstream
    vector<abstractObject*> backwardInputObjectPointers; // Inputs from downstream
    // Indices and formats of printed outputs
    vector<unsigned int> selectedOutputIndices;
    vector<int> selectedOutputDigits;
    // Debug mode flag
    bool debugMode;
    // Pointers to output streams
    ofstream* osPtrSel; // standard output of selected variables
    ofstream* osPtrDbg; // debug output
  protected:
    // FULL access to states and outputs for use at the LEFT hand side of expressions
    // in the simulate() method of derived classes. This is accomplished by the
    // use of non-const references.
    double& set_output(const T_index_output &index) {
      #if CHECK_RANGE
      if (index.index >= outputs.size()) {
        stringstream errmsg;
        errmsg << "Attempt to access output variable with index " << index.index <<
          " in object '" << idObject << "'. Index must be in range [0," <<
          (outputs.size()-1) << "] for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      return(outputs[index.index]);
    }
    // Access to single item
    double& set_stateScal(const T_index_stateScal &index) {
      #if CHECK_RANGE
      if (index.index >= statesScal.size()) {
        stringstream errmsg;
        errmsg << "Attempt to access scalar state variable with index " << index.index <<
          " in object '" << idObject << "'. Index must be in range [0," <<
          (statesScal.size()-1) << "] for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      return(statesScal[index.index]);
    }
    // Access to entire vector
    vector<double>& set_stateScal_all() {
      return(statesScal);
    }
    vector<double>& set_stateVect(const T_index_stateVect &index) {
      #if CHECK_RANGE
      if (index.index >= statesVect.nvars()) {
        stringstream errmsg;
        errmsg << "Attempt to access vector state variable with index " << index.index <<
          " in object '" << idObject << "'. Index must be in range [0," <<
          (statesVect.nvars()-1) << "] for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      return(statesVect.full_access(index.index));
    }
  public:
    // Constructor
    abstractObject ();
    // Destructor (base class --> virtual destructor)
    virtual ~abstractObject ();
    // Delete method for external inputs
    void clear_inputsExt();
    // Set and get id
    void set_idObject(const string id);
    string get_idObject() const;
    // Set and get object level
    void set_objectLevel (const unsigned int level);
    unsigned int get_objectLevel() const;
    // Get input objects (values are set by assign_inputsSim() method)
    const vector<abstractObject*>& get_forwardInputObjectPointers() const;
    const vector<abstractObject*>& get_backwardInputObjectPointers() const;
    // Set and get object group
    void set_objectGroupPointer (abstractObjectGroup* p);
    abstractObjectGroup* get_objectGroupPointer() const;
    // Set and query debug mode
    bool debugMode_isOn() const;
    void set_debugMode(const table &tab);  
    // Allocate the vector of object outputs
    void init_outputs(const table &tab);
    // Set parameters
    void init_paramsFun(const string chars_colsep, const string chars_comment);
    void init_paramsNum();
    // Initialize state variables
    void init_statesScal(const table &tab);
    void init_statesVect(const table &tab);
    // Get the address of a object output
    const double* get_outputAddress(const unsigned int index) const;
    // Set simulated inputs (internal boundary conditions)
    void assign_inputsSim(const table &tab, const vector<abstractObject*> &objects);
    // Set external inputs (external boundary conditions)
    void assign_inputsExt(const table &tab, const spaceTimeDataCollection &externalInputs);
    // Printing of output
    void output_selected(const bool firstCall, const bool finalCall, const string &outdir,
      const string &outfmt, const string chars_colsep, const string &timestamp, const unsigned int timestep);
    void output_debug(const bool firstCall, const string &outdir,
      const string chars_colsep, const string &timestamp);
    // Explicit closing out output files
    void closeOutput_selected();
    void closeOutput_debug();
    // READ-ONLY access to parameters, states, inputs, and outputs for use at
    // the RIGHT hand side of expressions in the simulate() method of derived classes.
    // Scalar numbers and function results are returned by value, vectors as const references.
    // Individual
    double paramNum(const T_index_paramNum &index) const {
      #if CHECK_RANGE
      if (index.index >= paramsNum.size()) {
        stringstream errmsg;
        errmsg << "Attempt to access individual scalar parameter with index " <<
          index.index << " in object '" << idObject << "'. Index must be in range [0," <<
          (paramsNum.size()-1) << "] for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      return(paramsNum[index.index]);
    }
    // Shared
    double sharedParamNum(const T_index_sharedParamNum &index) const {
      try {
        return( objectGroupPointer->get_sharedParamNum(index.index) );
      } catch (except) {
        stringstream errmsg;
        errmsg << "Failed to return value of shared scalar parameter with index " <<
          index.index << " in object '" << idObject << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
    // Individual
    double paramFun(const T_index_paramFun &index, const double &arg) {
      // Note: Can't be const due to lookup index update
      #if CHECK_RANGE
      if (index.index >= paramsFun.size()) {
        stringstream errmsg;
        errmsg << "Attempt to access individual parameter function with index " <<
          index.index << " in object '" << idObject << "'. Index must be in range [0," <<
          (paramsFun.size()-1) << "] for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      try {
        return(paramsFun[index.index].eval(arg));
      } catch (except) {
        stringstream errmsg;
        errmsg << "Cannot return value of individual parameter function with index '" <<
          index.index << "' of object '" << idObject << "' (object group '" <<
          objectGroupPointer->get_idObjectGroup() << "') for argument " <<
          arg << ".";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
    // Shared
    double sharedParamFun(const T_index_sharedParamFun &index, const double &arg) {
      // Note: Can't be const due to lookup index update
      try {
        return( objectGroupPointer->get_sharedParamFun(index.index,arg) );
      } catch (except) {
        stringstream errmsg;
        errmsg << "Failed to return value of shared parameter function with index " <<
          index.index << "in object '" << idObject << "' for argument " << arg << ".";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
    double inputExt(const T_index_inputExt &index) const {
      double result;
      #if CHECK_RANGE
      if (index.index >= inputsExt.size()) {
        stringstream errmsg;
        errmsg << "Attempt to access external input variable with index " << index.index <<
          " in object '" << idObject << "'. Index must be in range [0," <<
          (inputsExt.size()-1) << "] for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      result= 0.;
      for (unsigned int i=0; i<inputsExt[index.index].size(); i++) {
        result= result + (*inputsExt[index.index][i].valPtr) * inputsExt[index.index][i].weight;
      }
      return(result);
    }
    double inputSim(const T_index_inputSim &index) const {
      #if CHECK_RANGE
      if (index.index >= inputsSim.size()) {
        stringstream errmsg;
        errmsg << "Attempt to access simulated input variable with index " << index.index <<
          " in object '" << idObject << "'. Index must be in range [0," <<
          (inputsSim.size()-1) << "] for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      return(*inputsSim[index.index]);
    }
    // Single item
    double stateScal(const T_index_stateScal &index) const {
      #if CHECK_RANGE
      if (index.index >= statesScal.size()) {
        stringstream errmsg;
        errmsg << "Attempt to access scalar state variable with index " << index.index <<
          " in object '" << idObject << "'. Index must be in range [0," <<
          (statesScal.size()-1) << "] for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      return(statesScal[index.index]);
    }
    // Entire vector
    const vector<double>& stateScal_all() const {
      return(statesScal);
    }
    const vector<double>& stateVect(const T_index_stateVect &index) const {
      #if CHECK_RANGE
      if (index.index >= statesVect.nvars()) {
        stringstream errmsg;
        errmsg << "Attempt to access vector state variable with index " << index.index <<
          " in object '" << idObject << "'. Index must be in range [0," <<
          (statesVect.nvars()-1) << "] for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      return(statesVect.read_access(index.index));
    }
    double output(const T_index_output &index) const {
      #if CHECK_RANGE
      if (index.index >= outputs.size()) {
        stringstream errmsg;
        errmsg << "Attempt to access output variable with index " << index.index <<
          " in object '" << idObject << "'. Index must be in range [0," <<
          (outputs.size()-1) << "] for object group '" <<
          objectGroupPointer->get_idObjectGroup() << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      return(outputs[index.index]);
    }
    // Check states and outputs for floating point exceptions
    void checkFPE() const;
    // Virtual methods defined in the derived classes
    virtual void simulate(const unsigned int delta_t)= 0;
    virtual void derivsScal(const double t, const vector<double> &u,
      vector<double> &dudt, const unsigned int delta_t)= 0;
};

#endif

