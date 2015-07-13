
#include "echse_coreFunct_saveState.h" 

void saveState(
  const string &outdir,
  const fixedZoneTime &now,
  const vector<fixedZoneTime> &times_stateOutput,
  const string chars_colsep, const string chars_comment,
  const vector<abstractObject*> &objects
) {
  unsigned int nrecs;
  // If output of states is requested for one/some time(s)
  if (times_stateOutput.size() > 0) {
    vector<fixedZoneTime>::const_iterator iter= find(times_stateOutput.begin(),
      times_stateOutput.end(), now);
    // If current time is in vector of desired output times
    if (iter != times_stateOutput.end()) {
      string file;
      ofstream ost;  
      // Scalar states
      file= outdir + "/" + "statesScal" + "_" + now.get("","","") +
        globalConst::fileExtensions.tabular;
      if (file_exists(file)) {
        stringstream errmsg;
        errmsg << "Cannot print values of scalar states to file '" <<
          file << "'. File already exists.";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      ost.open(file.c_str());
      if (!ost.is_open()) {
        stringstream errmsg;
        errmsg << "Cannot print values of scalar states to file '" <<
          file << "'. File cannot be opened.";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      ost << chars_comment << " Values of scalar state variables at: " <<
        now.get("-",":"," ") << endl;
      ost << "object" << chars_colsep << "variable" << chars_colsep << "value" << endl;
      nrecs= 0;
      for (unsigned int i=0; i<objects.size(); i++) {
        try {
          const vector<string>& names= objects[i]->get_objectGroupPointer()->get_namesStatesScal();
          struct T_index_stateScal index;
          for (vector<string>::size_type k=0; k<names.size(); k++) {
            index.index= k;
            ost << objects[i]->get_idObject() << chars_colsep << names[k] <<
              chars_colsep << objects[i]->stateScal(index) << endl;
            nrecs++;
          }
        } catch (except) {
          stringstream errmsg;
          errmsg << "Cannot print values of scalar states to file '" <<
            file << "'. Encountered problems with object '" <<
            objects[i]->get_idObject() << "'.";
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
      }
      if (nrecs == 0) {
        ost << "none" << chars_colsep << "none" << chars_colsep << "none" << endl;
      }
      ost.close();
      // Vector states
      file= outdir + "/" + "statesVect" + "_" + now.get("","","") + ".txt";
      if (file_exists(file)) {
        stringstream errmsg;
        errmsg << "Cannot print values of vector states to file '" <<
          file << "'. File already exists.";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      ost.open(file.c_str());
      if (!ost.is_open()) {
        stringstream errmsg;
        errmsg << "Cannot print values of vector states to file '" <<
          file << "'. File cannot be opened.";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      ost << chars_comment << " Values of vector state variables at: " <<
        now.get("-",":"," ") << endl;
      ost << "object" << chars_colsep << "variable" << chars_colsep << "index" <<
        chars_colsep << "value" << endl;
      nrecs= 0;
      for (unsigned int i=0; i<objects.size(); i++) {
        try {
          const vector<string>& names= objects[i]->get_objectGroupPointer()->get_namesStatesVect();
          struct T_index_stateVect index;
          for (vector<string>::size_type k=0; k<names.size(); k++) {
            index.index= k;
            const vector<double>& values= objects[i]->stateVect(index);
            for (unsigned int n=0; n<values.size(); n++) {
              ost << objects[i]->get_idObject() << chars_colsep << names[k] <<
                chars_colsep << n << chars_colsep << values[n] << endl;
              nrecs++;
            }
          }
        } catch (except) {
          stringstream errmsg;
          errmsg << "Cannot print values of vector states to file '" <<
            file << "'. Encountered problems with object '" <<
            objects[i]->get_idObject() << "'.";
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
      }
      if (nrecs == 0) {
        ost << "none" << chars_colsep << "none" << chars_colsep << "none" <<
          chars_colsep << "none" << endl;
      }
      ost.close();
    }
  }
}

