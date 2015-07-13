
#ifndef ECHSE_CORECLASS_TEMPLATEOBJECTGROUP
#define ECHSE_CORECLASS_TEMPLATEOBJECTGROUP


#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "except/except.h"
#include "arrays/arrays.h"

#include "echse_coreClass_abstractObjectGroup.h"
#include "echse_coreClass_abstractObject.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Template class derived from 'abstractObjectGroup'
////////////////////////////////////////////////////////////////////////////////

template <class T>
class templateObjectGroup: public abstractObjectGroup {
  private:
    vector<T> objects;
    static unsigned int n_instances;
  public:
    // Constructor
    templateObjectGroup(const string id,
      // Names of items stored in the objects (indiviual values)
      const vector<string> namesParamsFun,
      const vector<string> namesParamsNum,
      const vector<string> namesInputsExt,
      const vector<string> namesInputsSim,
      const vector<string> namesStatesScal,
      const vector<string> namesStatesVect,
      const vector<string> namesOutputs,
      // Names of items stored in the objectGroup (shared values)
      const vector<string> namesSharedParamsFun,
      const vector<string> namesSharedParamsNum
    ) {
      if (n_instances == 1) {
        stringstream errmsg;
        errmsg << "Attempt to instantiate more than one object group" <<
          " with id '" << id << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      n_instances++;
      abstractObjectGroup::set_idObjectGroup(id);
      try {
        // Names of items stored in the objects
        abstractObjectGroup::set_namesParamsFun(namesParamsFun);
        abstractObjectGroup::set_namesParamsNum(namesParamsNum);
        abstractObjectGroup::set_namesSharedParamsFun(namesSharedParamsFun);
        abstractObjectGroup::set_namesSharedParamsNum(namesSharedParamsNum);
        abstractObjectGroup::set_namesInputsExt(namesInputsExt);
        abstractObjectGroup::set_namesInputsSim(namesInputsSim);
        abstractObjectGroup::set_namesStatesScal(namesStatesScal);
        abstractObjectGroup::set_namesStatesVect(namesStatesVect);
        abstractObjectGroup::set_namesOutputs(namesOutputs);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Failed to set item names for object group with id '" << id << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
    // Destructor
    ~templateObjectGroup() { };
    // Redefined methods of the abstract base class
    void addObject(const string idObject) {
      abstractObjectGroup::size_type n;
      n= objects.size();
      objects.resize(n+1);
      static_cast<abstractObject*>(&objects[n])->set_idObject(idObject);
      static_cast<abstractObject*>(&objects[n])->set_objectGroupPointer(
        static_cast<abstractObjectGroup*>(this));
    }
    unsigned int numObjects() const {
      return(objects.size());
    }
    abstractObject* get_objectAddress(const abstractObjectGroup::size_type index) {
      if (index > (objects.size()-1)) {
        stringstream errmsg;
        errmsg << "Cannot access object with index " << index << " in object" <<
          " group '" << abstractObjectGroup::get_idObjectGroup() << 
          "'. Index must be in range [0," << (objects.size()-1) << "].";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      return(static_cast<abstractObject*>(&objects[index]));
    }
};
// Initialization of static members
template <class T>
unsigned int templateObjectGroup<T>::n_instances= 0;

#endif

