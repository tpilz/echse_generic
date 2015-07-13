
#include "settings.h"

// Constructor
settings::settings() {
  source= "";
  data.clear();
}

// Destructor
settings::~settings() {
  source= "";
  data.clear();
}

// Read settings from file
void settings::read_fileOnly(const string &file, const string &chars_colsep, const string &chars_comment) {
  string key, val;
  T_insertStatus stat;
  table tab;
  // Initial clear
  clear();
  source= "file '" + file + "'";
  try {
    tab.read(file, false, chars_colsep, chars_comment);
    if ((tab.ncol() != 2) || (tab.empty())) {
      stringstream errmsg;
      errmsg << "Table with settings (file '" << file <<
        "') must have 2 columns and at least 1 row.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    for (table::size_type i=1; i<=tab.nrow(); i++) {
      key= tab.get_element(i,1);
      val= tab.get_element(i,2);
      stat= data.insert(T_pair(key,val));
      if (!stat.second) {
        stringstream errmsg;
        errmsg << "Cannot add setting with key '" << key << "' to list of settings." <<
          " This is probably due to multiple instances of that key in file '" <<
          file << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
    tab.clear();
  } catch (except) {
    stringstream errmsg;
    errmsg << "Cannot load settings from " << source << ".";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

// Read settings from both file and command line
void settings::read_fileAndArgs(const string &file, const string &chars_colsep,
  const string &chars_comment, const cmdline &cmdl, const string &chars_keysep) {
  string key, val;
  T_insertStatus stat;
  table tab;
  // Initial clear
  clear();
  source= "command line & file '" + file + "'";
  // Read settings from file
  try {
    tab.read(file, false, chars_colsep, chars_comment);
    if ((tab.ncol() != 2) || (tab.empty())) {
      stringstream errmsg;
      errmsg << "Table with settings (file '" << file <<
        "') must have 2 columns and at least 1 row.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    for (table::size_type i=1; i<=tab.nrow(); i++) {
      key= tab.get_element(i,1);
      val= tab.get_element(i,2);
      stat= data.insert(T_pair(key,val));
      if (!stat.second) {
        stringstream errmsg;
        errmsg << "Cannot add setting with key '" << key << "' to list of settings." <<
          " This is probably due to multiple instances of that key in file '" <<
          file << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
    tab.clear();
  } catch (except) {
    stringstream errmsg;
    errmsg << "Cannot load settings from " << source << ".";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  // Add command line settings
  try {
    const cmdline::T_map args= cmdl.export_pairs(chars_keysep);
    for (cmdline::T_map::const_iterator iter=args.begin(); iter != args.end(); iter++) {
      key= iter->first;
      val= iter->second;
      stat= data.insert(T_pair(key,val));
      if (!stat.second) {
        stringstream errmsg;
        errmsg << "Multiple definitions of key '" << key << "' detected. This" <<
          " key is defined at the command line as well as in file '" << file << "'.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
  } catch (except) {
    stringstream errmsg;
    errmsg << "Cannot load settings from " << source << ".";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
}

// Manually clear settings
void settings::clear() {
  source= "";
  data.clear();
}

// Overloading of index operator for access
const string& settings::operator[](const string &key) const {
  T_map::const_iterator iter= data.find(key);
  if (iter == data.end()) {
    stringstream errmsg;
    errmsg << "Value corresponding to key '" << key << "' not available." <<
      " Key does not exist in " << source << ".";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  return(iter->second);
}

