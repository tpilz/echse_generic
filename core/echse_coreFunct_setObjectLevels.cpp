
#include "echse_coreFunct_setObjectLevels.h"

void setObjectLevels(
  const vector<abstractObject*> &objects
) {
  // Set the level of all objects without forward inputs to 1 (independend objects)
  // Initialize the level of all other objects with 0 (dependend objects)
  for (unsigned int i=0; i<objects.size(); i++) {
    const vector<abstractObject*>& p= objects[i]->get_forwardInputObjectPointers();
    if (p.size() == 0) {
      objects[i]->set_objectLevel(1);
    } else {
      objects[i]->set_objectLevel(0);
    }
  }
  // Set level of remaining objects
  bool inputLevelsComplete;
  unsigned int maxInputLevel;
  bool progress;
  bool done= false;
  while (!done) {
    done= true;
    progress= false;
    for (unsigned int i=0; i<objects.size(); i++) {
      // If this object's level is still unknown
      if (objects[i]->get_objectLevel() == 0) {
        // Is level of all source objects already known?
        const vector<abstractObject*>& p= objects[i]->get_forwardInputObjectPointers();
        inputLevelsComplete= true;
        maxInputLevel= 0;
        for (unsigned int k=0; k<p.size(); k++) {
          if (p[k]->get_objectLevel() == 0) {
            inputLevelsComplete= false;
            break;
          }
          if (p[k]->get_objectLevel() > maxInputLevel) {
            maxInputLevel= p[k]->get_objectLevel();
          }
        }
        if (inputLevelsComplete) {
          objects[i]->set_objectLevel(maxInputLevel + 1);
          progress= true;
        } else {
          done= false;
        }
      }    
    }
    if (!progress) {
      stringstream errmsg;
      errmsg << "Failed to assign object levels. This is likely to result from" <<
        " improper object linkage, i.e. badly defined input-output relations." <<
        " In particular, a object 'A' cannot (directly or indirectly) get 'forward'" <<
        " input from object 'B', if 'B' (directly or indirectly) gets 'forward'"<<
        " input from 'A'. In such a case, one of the two relations has to be"<<
        " declared as 'backward'.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
  // Check that the level has been set for all objects
  for (unsigned int i=0; i<objects.size(); i++) {
    if (objects[i]->get_objectLevel() == 0) {
      stringstream errmsg;
      errmsg << "Error in assignment of object levels (Bug in source code).";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
  // Debug code to check the assigned levels (no actual need to run this)
  for (unsigned int i=0; i<objects.size(); i++) {
    const vector<abstractObject*>& p= objects[i]->get_forwardInputObjectPointers();
    for (unsigned int k=0; k<p.size(); k++) {
      if (p[k]->get_objectLevel() >= objects[i]->get_objectLevel()) {
        stringstream errmsg;
        errmsg << "Error in assignment of object levels (Bug in source code).";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
    }
  }
}

