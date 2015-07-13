
#include "echse_coreFunct_instantiateObjects.h"

void instantiateObjects(
  const string file_objects, const string chars_colsep, const string chars_comment,
  vector<abstractObjectGroup*> &objectGroups,
  vector<abstractObject*> &objects
) {
  const string legal_chars= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-()0123456789";
  table tab;
  vector<string> vect_idObject, vect_idObjectGroup, vect_activity;
  bool ok;
  try {

    // Make sure that column separators or comment characters are not among the
    // characters used in legal object names
    if (legal_chars.find_first_of(chars_colsep) != string::npos) {
      stringstream errmsg;
      errmsg << "Cannot use one of characters '" << chars_colsep << "' as column separator(s)." <<
       "Please chose (an)other character(s) not contained in '" << legal_chars << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    if (legal_chars.find_first_of(chars_comment) != string::npos) {
      stringstream errmsg;
      errmsg << "Cannot use one of characters '" << chars_comment << "' as comment character." <<
        "Please chose (an)other character(s) not contained in '" << legal_chars << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    // Process table with object declaration
    try {
      tab.read(file_objects, true, chars_colsep, chars_comment);
    } catch (except) {
      stringstream errmsg;
      errmsg << "Cannot read object declaration table from file '" << file_objects << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    try {
      tab.get_col(tab.colindex(globalConst::colNames.objectID), vect_idObject);
      tab.get_col(tab.colindex(globalConst::colNames.objectGroupID), vect_idObjectGroup);
    } catch (except) {
      stringstream errmsg;
      errmsg << "Cannot retrieve data from object declaration table ('" <<
        file_objects << "'). Expecting columns '" << globalConst::colNames.objectID <<
        "' and '" << globalConst::colNames.objectGroupID << "'.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    // Make sure that object ids are unique
    if (has_duplicates(vect_idObject)) {
      stringstream errmsg;
      errmsg << "Duplicate object id(s) detected in file '" << file_objects << 
        "'. Id values must be unique.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    // Check object names
	  for (unsigned int i=0; i<vect_idObject.size(); i++) {
      // There must not be characters not allowed in file names
      if (vect_idObject[i].find_first_not_of(legal_chars) != string::npos) {
        stringstream errmsg;
        errmsg << "Invalid characters in object id '" << vect_idObject[i] <<
          "' detected in file '" << file_objects << "'. Allowed character set is '" <<
          legal_chars << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }

    }
    // Instantiate objects of all object groups
	  for (unsigned int i=0; i<vect_idObject.size(); i++) {
      ok= false;
      for (unsigned int k=0; k<objectGroups.size(); k++) {
        if (objectGroups[k]->get_idObjectGroup() == vect_idObjectGroup[i]) {
          objectGroups[k]->addObject(vect_idObject[i]);
          ok= true;
          break;
        }
      }
      if (!ok) {
        stringstream errmsg;
        errmsg << "Object group '" << vect_idObjectGroup[i] << 
          "' is unknown (not implemented). One or more object(s) declared in" <<
          " file '" << file_objects << "' cannot be instantiated.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
    tab.clear();
    // Create pointer array to handle all objects
    // Note: The order of objects in the array is NOT necessarily the order of
    //       processing. The processing order is determined ONLY by object linkage.
    objects.clear();
    for (unsigned int k=0; k<objectGroups.size(); k++) {
      for (unsigned int i=0; i<objectGroups[k]->numObjects(); i++) {
        objects.push_back(objectGroups[k]->get_objectAddress(i));
      }
    }
  } catch (except) {
    stringstream errmsg;
    errmsg << "Failed to instantiate objects using file '" << file_objects << "'.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}

