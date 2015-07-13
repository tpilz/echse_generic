
#ifndef ECHSE_CORECLASS_ABSTRACTOBJECTGROUP_H
#define ECHSE_CORECLASS_ABSTRACTOBJECTGROUP_H

#include <iostream>
#include <sstream>
#include <string>
#include <sstream>
#include <map>

#include "table/table.h"
#include "functions/functions.h"
#include "arrays/arrays.h"
#include "except/except.h"

#include "echse_options.h"
#include "echse_globalConst.h"

#ifndef CHECK_RANGE
#  error "Macro CHECK_RANGE is undefined."
#endif

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Abstract base class 'abstractObjectGroup'
////////////////////////////////////////////////////////////////////////////////

// Forward declaration of 'abstractObject' to avoid cyclic dependency when
// using #include "echse_coreClass_abstractObject.h"
class abstractObject;

class abstractObjectGroup {
  private:
    // ID of the object group
    string idObjectGroup;
    // Names of static items whose (numeric) values are individual for each object
    // (--> value-arrays are members of the object class)
    //   Note: These names could also be STATIC data members of
    //   the object classes. Defining them as members of the objectGroup is
    //   advantageous because we don't need to re-implement the get-methods
    //   in every object class. 
    vector<string> namesParamsFun;
    vector<string> namesParamsNum;
    vector<string> namesInputsExt;
    vector<string> namesInputsSim;
    vector<string> namesStatesScal;
    vector<string> namesStatesVect;
    vector<string> namesOutputs;
    // Names of static items whose (numeric) values are shared between the objects
    // of this group (--> value-arrays are members of the objectGroup)
    //   Note: These names could also be STATIC data members of
    //   the object classes. Defining them as members of the objectGroup is
    //   advantageous because we don't need to re-implement the get-methods
    //   in every object class. 
    vector<string> namesSharedParamsFun;
    vector<string> namesSharedParamsNum;
    // Maps to quickly identify array positions by item name
    typedef map<string, unsigned int> name_index_map;
    typedef pair<string, unsigned int> name_index_pair;
    name_index_map mapParamsFun;
    name_index_map mapParamsNum;
    name_index_map mapSharedParamsFun;
    name_index_map mapSharedParamsNum;
    name_index_map mapInputsExt;
    name_index_map mapInputsSim;
    name_index_map mapStatesScal;
    name_index_map mapStatesVect;
    name_index_map mapOutputs;
    // Tables with parameters for all objects of a group
    table table_paramsFun;
    table table_paramsNum;
    // Vectors of parameters of the object group
    vector<double> sharedParamsNum;
    vector<tblFunction> sharedParamsFun;
  public:
    typedef unsigned int size_type;
    // Constructor/desctructor
    abstractObjectGroup ();
    virtual ~abstractObjectGroup ();
    // Set and get object group ID
    void set_idObjectGroup(const string id);
    string get_idObjectGroup() const;
    // Set and get names of parameters etc.
    void set_namesParamsFun(const vector<string> &names);
    void set_namesParamsNum(const vector<string> &names);
    void set_namesSharedParamsFun(const vector<string> &names);
    void set_namesSharedParamsNum(const vector<string> &names);
    void set_namesInputsExt(const vector<string> &names);
    void set_namesInputsSim(const vector<string> &names);
    void set_namesStatesScal(const vector<string> &names);
    void set_namesStatesVect(const vector<string> &names);
    void set_namesOutputs(const vector<string> &names);
    const vector<string>& get_namesParamsFun() const;
    const vector<string>& get_namesParamsNum() const;
    const vector<string>& get_namesSharedParamsFun() const;
    const vector<string>& get_namesSharedParamsNum() const;
    const vector<string>& get_namesInputsExt() const;
    const vector<string>& get_namesInputsSim() const;
    const vector<string>& get_namesStatesScal() const;
    const vector<string>& get_namesStatesVect() const;
    const vector<string>& get_namesOutputs() const;
    // Determine array index to allow parameters etc. to be accessed by name
    size_type indexParamsFun(const string &name) const;
    size_type indexParamsNum(const string &name) const;
    size_type indexSharedParamsFun(const string &name) const;
    size_type indexSharedParamsNum(const string &name) const;
    size_type indexInputsExt(const string &name) const;
    size_type indexInputsSim(const string &name) const;
    size_type indexStatesScal(const string &name) const;
    size_type indexStatesVect(const string &name) const;
    size_type indexOutputs(const string &name) const;
    // Set and get tables of individual parameters for all objects of this group
    // Note: These are temporary tables which are read only once (instead of by
    //       every object of this group). The individual objects use the
    //       get-methods to copy the required data.After the data are copied to
    //       the individual objects these tables are no longer needed.
    //       The table of functions contains references to files only. Reading
    //       of the actual function data is managed by the individual objects. 
    void set_tableParamsFun (const string file, const string chars_colsep,
      const string chars_comment);
    void set_tableParamsNum (const string file, const string chars_colsep,
      const string chars_comment);
    const table & get_tableParamsFun() const;
    const table & get_tableParamsNum() const;
    void clear_tableParamsFun();
    void clear_tableParamsNum();
    // Set shared parameters
    // Note: These data are shared among all objects of this group. They exist
    //       for the livetime of the object group and clean-up is done by the
    //       destructor.
    //       As opposed to the objects individual functions, the actual function
    //       data are read right here. 
    void set_sharedParamsFun(const string file, const string chars_colsep,
      const string chars_comment);
    void set_sharedParamsNum(const string file, const string chars_colsep,
      const string chars_comment);
    // Get values of shared parameters
    double get_sharedParamNum(const size_type index) const {
      #if CHECK_RANGE
      if (index >= sharedParamsNum.size()) {
        stringstream errmsg;
        errmsg << "Attempt to access shared parameter with index " << index <<
          " of object group '" << idObjectGroup << "'. Index must be in range [0," <<
          (sharedParamsNum.size()-1) << "].";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      return(sharedParamsNum[index]);
    }
    double get_sharedParamFun(const size_type index, const double &arg) {
      // Note: Can't be const due to lookup index update
      #if CHECK_RANGE
      if (index >= sharedParamsFun.size()) {
        stringstream errmsg;
        errmsg << "Attempt to access shared parameter function with index " << index <<
          " of object group '" << idObjectGroup << "'. Index must be in range [0," <<
          (sharedParamsFun.size()-1) << "].";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      #endif
      try {
        return(sharedParamsFun[index].eval(arg));
      } catch (except) {
        stringstream errmsg;
        errmsg << "Cannot return value of shared parameter function with index '" <<
          index << "' of object group '" << idObjectGroup << "' for argument " <<
          arg << ". The function's name is '" << namesSharedParamsFun[index] << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
    // Virtual methods to be redefined in derived classes
    virtual void addObject(const string idObject)= 0;
    virtual unsigned int numObjects() const = 0;
    virtual abstractObject* get_objectAddress(const size_type index)= 0;
};

#endif

