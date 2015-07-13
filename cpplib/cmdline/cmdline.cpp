
#include "cmdline.h"

// Constructor
cmdline::cmdline(const int argc, const char* argv[]) {
  if (argc > 1) {
    for (int i=1; i<argc; i++) {
			cmdargs.push_back(argv[i]);
		}
	}
}

// Destructor
cmdline::~cmdline() {
  cmdargs.resize(0);
}

// Method to retrieve a single value
string cmdline::get_value(const string key, const string sep) const {
  bool found;
  const string keystr= key+sep;
  string result;
  if (cmdargs.size() == 0) {
    stringstream errmsg;
    errmsg << "Cannot retrieve value from command line using keyword '" <<
      key << "' and separator '" << sep << "'. No arguments supplied." <<
      " Expecting '" << key << sep << "<value>'.";
		except e (__PRETTY_FUNCTION__, errmsg, __FILE__,__LINE__);
    throw(e);
	}
  found=false;
	for (unsigned int i=0; i<cmdargs.size(); i++) {
    if (cmdargs[i].length() >= keystr.length()) {
		  if (cmdargs[i].compare(0, keystr.length(), keystr, 0, keystr.length()) == 0) {
		    if (found) {
					stringstream errmsg;
          errmsg << "Cannot retrieve value from command line using keyword '" <<
            key << "' and separator '" << sep << "'. Detected multiple" <<
            " occurrences of this key ('" << key << sep << "').";
					except e (__PRETTY_FUNCTION__, errmsg, __FILE__,__LINE__);
					throw(e);
				}
		    if (cmdargs[i].length() <= keystr.length()) {
					stringstream errmsg;
          errmsg << "Cannot retrieve value from command line using keyword '" <<
            key << "' and separator '" << sep << "'. The key exists but the" <<
            " value is blank. Expecting '" << key << sep << "<value>'.";
					except e (__PRETTY_FUNCTION__, errmsg, __FILE__,__LINE__);
					throw(e);
				}
		    result= cmdargs[i].substr(keystr.length(),(cmdargs[i].length()-keystr.length()));
		    found= true;
			}
		}
  }
  if (!found) {
		stringstream errmsg;
    errmsg << "Cannot retrieve value from command line using keyword '" <<
      key << "' and separator '" << sep << "'. Expecting '" << key << sep << 
        "<value>'.";
		except e (__PRETTY_FUNCTION__, errmsg, __FILE__,__LINE__);
		throw(e);
	}
	return(result);
}

// Method to export all key-value-pairs as a map
cmdline::T_map cmdline::export_pairs(const string keysep) const {
  // Locals
  T_map data;
  T_insertStatus stat;
  string::size_type strpos;
  string key, val;
  // Code
  data.clear();
	for (unsigned int i=0; i<cmdargs.size(); i++) {
    strpos= cmdargs[i].find_first_of(keysep);
    // If separator exists AND it is not the first or last character
    if ((strpos > 0) & (strpos < (cmdargs[i].length()-1))) {
      key= cmdargs[i].substr(0,strpos);
      val= cmdargs[i].substr((strpos+1),(cmdargs[i].length()-strpos-1));      
      stat= data.insert(T_pair(key,val));
      if (!stat.second) {
        stringstream errmsg;
        errmsg << "Cannot add command line argument with key '" << key <<
          "' to list. This is probably due to non-unique keys.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
    }
  }
  return(data);
}


