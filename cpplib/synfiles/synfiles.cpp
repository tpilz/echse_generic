
#include "synfiles.h"

void read_syn (
  const string synfile,
  const bool syn_timeAtEOI,
  const bool location_lowercase,
  const bool location_removePrefix,
  string &location,
  vector<double> &values,
  fixedZoneTime &firstEOI,
  unsigned int &dt_seconds
){
  // Local data
  typedef map<string, string> maptype;
  ifstream ifile;
  string headline, line;
  string::size_type pos;
  vector<string> tokens;
  maptype headinfo;
  maptype::const_iterator iter;
  double number;
  unsigned int year, month, day, hour, minute;
  unsigned int ndata;
  // Open file
  ifile.open(synfile.c_str());
  if (!ifile.is_open()) {
    except e(__PRETTY_FUNCTION__,"Cannot open file.",__FILE__,__LINE__);
    throw(e);
  }
  // Read header line
  getline(ifile,headline);
  if (!ifile) {
    except e(__PRETTY_FUNCTION__,"Cannot read file header.",__FILE__,__LINE__);
    throw(e);
  }
  // Remove possible comments after "%"
  pos= headline.find_first_of("%");
  if (pos != string::npos) {
    if (pos < 1) {
      except e(__PRETTY_FUNCTION__,"Unexpected position of \'%\' in header.",__FILE__,__LINE__);
      throw(e);
    }
    headline= headline.substr(0,pos);
  }
  // Set location name and remove everything up to "*"
  pos= headline.find_first_of("*");
  if ((pos < 1) | (pos >= (headline.length()-1))) {
    except e(__PRETTY_FUNCTION__,"Unexpected/missing position of \'*\' in header.",__FILE__,__LINE__);
    throw(e);
  }
  location= headline.substr(0,pos);
  if (location.find_last_not_of(" ") != string::npos) {
    location= location.substr(0,(location.find_last_not_of(" ")+1));
  }
  if (location_lowercase) location= casefold(location,true);
  if (location_removePrefix) {  // Prefix is assumed to end with "_" 
    pos= headline.find_first_of("_");
    if (pos < (location.length()-1)) {
      location= location.substr((pos+1),string::npos);
    }
  }
  pos= headline.find_first_of("*");
  headline= headline.substr((pos+1),string::npos);
  // Add keys to all elements in header
  headline.insert(0,"DATE=");
  // Split header into tokens at " "
  split(headline, tokens, " ");
  // Save header info in map
  headinfo.clear();
  for (unsigned int i=0; i<tokens.size(); i++) {
    pos= tokens[i].find_first_of("=");
    if ((pos < 1) | (pos >= (tokens[i].length()-1))) {
      except e(__PRETTY_FUNCTION__,"Unexpected/missing position of \'=\' in header.",__FILE__,__LINE__);
      throw(e);
    }
    headinfo[tokens[i].substr(0,pos)]= tokens[i].substr((pos+1),string::npos);
  }
  // Get time step
  iter= headinfo.find("DT");
  if (iter == headinfo.end()) {
    except e(__PRETTY_FUNCTION__,"Failed to extract info on time step.", __FILE__, __LINE__);
    throw(e);
  }
  try {
    dt_seconds= floor(as_double(iter->second) * 3600.);
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Error in time step info.",__FILE__,__LINE__);
    throw(e);
  }
  // Get date
  iter= headinfo.find("DATE");
  if (iter == headinfo.end()) {
    except e(__PRETTY_FUNCTION__,"Failed to extract date info.", __FILE__, __LINE__);
    throw(e);
  }
  try {
    day=   as_unsigned_integer(iter->second.substr(0,2));
    month= as_unsigned_integer(iter->second.substr(3,2));
    year=  as_unsigned_integer(iter->second.substr(6,4));
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Error in date components.",__FILE__,__LINE__);
    throw(e);
  }
  // Get time  (given as H.M, where M is the true minute, not a fraction)
  iter= headinfo.find("T0");
  if (iter == headinfo.end()) {
    except e(__PRETTY_FUNCTION__,"Failed to extract time info.", __FILE__, __LINE__);
    throw(e);
  }
  try {
    number= as_double(iter->second);
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Error in time info.",__FILE__,__LINE__);
    throw(e);
  }
  hour= floor(number);
  minute= floor((number - floor(number)) * 100);
  // Convert to date time
  firstEOI= fixedZoneTime(year,month,day,hour,minute,0);
  if (!syn_timeAtEOI) firstEOI= firstEOI + dt_seconds;
  // Get number of values
  iter= headinfo.find("N");
  if (iter == headinfo.end()) {
    except e(__PRETTY_FUNCTION__,"Failed to extract info on number of values.", __FILE__, __LINE__);
    throw(e);
  }
  try {
    ndata= as_unsigned_integer(iter->second);
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Error in info on number of values.",__FILE__,__LINE__);
    throw(e);
  }
  if (ndata < 1) {
    except e(__PRETTY_FUNCTION__,"Number of values must not be zero.",__FILE__,__LINE__);
    throw(e);
  }
  // Read values
  values.clear();
  while(!ifile.eof()) {
    getline(ifile,line);
    pos= line.find_last_not_of("\n\r");
    if (pos != string::npos) {
      line= line.substr(0,pos);
      split(line, tokens, " \t");
      if (tokens.size() > 0) {
        try {
          for (unsigned int i=0; i<tokens.size(); i++) {
            number= as_double(tokens[i]);
            values.push_back(number);
          }
        } catch (except) {
          except e(__PRETTY_FUNCTION__,"Non-numeric data encountered.",__FILE__,__LINE__);
          throw(e);
        }
      }
    }
  }
  if (values.size() != ndata) {
    except e(__PRETTY_FUNCTION__,"Number of values in file does not match header info.",__FILE__,__LINE__);
    throw(e);
  }
  // Close file
	ifile.close();
}

