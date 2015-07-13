
#include "spaceTimeData.h"

////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////
spaceTimeData::spaceTimeData() {
  clear();
}

////////////////////////////////////////////////////////////////////////////////
// Clear object
////////////////////////////////////////////////////////////////////////////////

void spaceTimeData::clear() {
  if (ifs.is_open()) ifs.close();
  filepath= "";
  charsColSep= "";
  charsComment= "";
  // Data and counters
  locations.resize(0);
  data_recordStart.resize(0);;
  data_recordEnd.resize(0);;
  data_currentWindow.resize(0);;
  lineCurrent= 0;
  recNumber= 0;
  // Buffer
  indexBuffer= 0;
  nRecsInBuffer= 0;
  buffer.clear();
  // Times
  time_recordStart.set(0);
  time_recordEnd.set(0);
  time_currentWindowStart.set(0);
  time_currentWindowEnd.set(0);
  time_firstInFile.set(0);
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////
spaceTimeData::~spaceTimeData() {
  clear();
}

////////////////////////////////////////////////////////////////////////////////
// Check if initialized
////////////////////////////////////////////////////////////////////////////////
bool spaceTimeData::empty() const {
  if (locations.size() == 0) {
    return(true);
  } else {
    return(false);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Try to read the loaction IDs
////////////////////////////////////////////////////////////////////////////////

void spaceTimeData::read_locations() {
  unsigned int ntokens;
  string line;
  string::size_type strpos;
  vector<string> tokens;
  bool found= false;
  while (!ifs.eof()) {
    getline(ifs,line);
    // Update line counter
    lineCurrent++;
    strpos= line.find_first_not_of(" \t\n");
    if (strpos != string::npos) {                                               // Skip blank lines
      if (line.substr(strpos,1).find_first_of(charsComment) == string::npos) { // Skip comments
        ntokens= split(line, tokens, charsColSep);
        // Check number of items
        if (ntokens < 2) {
          stringstream errmsg;
			    errmsg << "Expecting at least two columns in file '" << filepath << "' (line " << lineCurrent << ").";
					except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
					throw(e);
        }
        locations.resize(ntokens-1);
        for (unsigned int k=1; k<ntokens; k++) {
          locations[k-1]= tokens[k];
        }
        found= true;
        break;
      }
    }
  }
  if (!found) {
    stringstream errmsg;
    errmsg << "Cannot read location IDs from file '" << filepath << "'." <<
      " Reached the end of the file.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Try to read a data record
////////////////////////////////////////////////////////////////////////////////

void spaceTimeData::read_data() {
  unsigned int ntokens;
  string line;
  string iso8601_string;
  string::size_type strpos;
  vector<string> tokens;

  // Load new data into buffer if neccessary
  if (indexBuffer == nRecsInBuffer) {
    nRecsInBuffer= 0;
    for (unsigned int i=0; i<buffer.size(); i++) {
      if (!ifs.eof()) {
        getline(ifs,buffer[i]);
        nRecsInBuffer++;
      } else {
        break;
      }
    }
    indexBuffer= 0;
  }

  // Throw an exception if buffer is empty (happens if the end of file is
  // encountered
  if (nRecsInBuffer == 0) {
    stringstream errmsg;
    errmsg << "Cannot read data from file '" << filepath << "' into buffer." <<
      " Reached the end of the file.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }

  // Get next record from buffer and advance counter
  line= buffer[indexBuffer];
  indexBuffer++;

  // Update line counter
  lineCurrent++;

  // Process this record
  strpos= line.find_first_not_of(" \t\n");
  if (strpos != string::npos) {                                               // Skip blank lines
    if (line.substr(strpos,1).find_first_of(charsComment) == string::npos) { // Skip comments
      // Update record number
      recNumber++;
      if (line.length() <= 20) {
        stringstream errmsg;
        errmsg << "Line " << lineCurrent << " of file '" << filepath <<
          "' is incomplete. Expecting time information followed by data for " <<
          locations.size() << " locations.";
			  except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
			  throw(e);
      }
      iso8601_string= line.substr(0,19);
      if (line.substr(19,1).find_first_of(charsColSep) == string::npos) {
        stringstream errmsg;
        errmsg << "Missing column separator between time and data at line " <<
          lineCurrent << " of file '" << filepath << "'.";
			  except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
			  throw(e);
      }
      ntokens= split(line.substr(20,(line.length()-20)), tokens, charsColSep);
      // Check number of items
      if (ntokens != locations.size()) {
        stringstream errmsg;
        errmsg << "Bad number of tokens at line " << lineCurrent << " of file '" <<
          filepath << "'. Expecting time information followed by data for " <<
          locations.size() << " locations.";
			  except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
			  throw(e);
      }
      // Save time and data of previous record. In case of the first record,
      // the values are set later.
      if (recNumber > 1) {
        time_recordStart= time_recordEnd;
        data_recordStart= data_recordEnd;
      }
      // Update time
      try {
        time_recordEnd.set(iso8601_string);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Cannot process time information at line " << lineCurrent <<
          " of file '" << filepath << "'.";
			  except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
			  throw(e);
      }
      // Make sure that time increases. Do not remove this check as the
      // update method relies on this check being done.
      if (recNumber > 1) {
        if (time_recordEnd <= time_recordStart) {
          stringstream errmsg;
          errmsg << "Bad record detected at line " << lineCurrent <<
            " of file '" << filepath << "'. Time must increase from one" <<
            " record to the next (oldest record first).";
		      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		      throw(e);
        }
      }
      // Set data (array must have been allocated already)
      for (unsigned int k=0; k<ntokens; k++) {
        try {
          data_recordEnd[k]= as_double(tokens[k]);
        } catch (except) {
          stringstream errmsg;
          errmsg << "Non-numeric value detected for location '" << locations[k] <<
            "' at line " << lineCurrent << " of file '" << filepath << "'.";
			    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
			    throw(e);
        }
      }
      // Initialize time and data of previous access if this was the first access.
      // Note: Since the init-method reads two data records, the values set here
      //       are actually never used.
      if (recNumber == 1) {
        time_recordStart= time_recordEnd - 1; // Adds 1 sec before the actual data
        data_recordStart= data_recordEnd;     // Assume persistence over the 1 sec
      }
    }
  }


}

////////////////////////////////////////////////////////////////////////////////
// Initialize data (read header and first record of data file)
////////////////////////////////////////////////////////////////////////////////

void spaceTimeData::init(const string &file, const string &chars_colsep,
  const string &chars_comment, const bool valuesAssignedToEndOfInterval,
  const bool reduceByTimeFraction, const unsigned int bufferSize) {
  // Reset all (does all initializations)
  clear();
  // Copy data
  filepath= file;
  charsColSep= chars_colsep;
  charsComment= chars_comment;
  redByTimeFract= reduceByTimeFraction;
  valsAssignedToEOInt= valuesAssignedToEndOfInterval;
  // Allocate buffer
  if (bufferSize >= 1) {
    buffer.resize(bufferSize);
  } else {
    stringstream errmsg;
    errmsg << "Buffer to hold records from file '" << filepath << "' must not have zero size.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Open input file
  ifs.open(filepath.c_str());
  if (!ifs.is_open()) {
    stringstream errmsg;
    errmsg << "Unable to open file '" << filepath << "' for reading.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Try to read locations from first record (1st token is ignored, location IDs follow)
  try {
    read_locations();
  } catch (except) {
    stringstream errmsg;
    errmsg << "Cannot determine location IDs from file '" << filepath << "'.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Allocate data vectors
  data_recordStart.resize(locations.size());
  data_recordEnd.resize(locations.size());
  data_currentWindow.resize(locations.size());
  // Try to read the first TWO data records
  try {
    // Read first and second data record
    read_data();
    read_data();
  } catch (except) {
    stringstream errmsg;
    errmsg << "Cannot read data for " << locations.size() << " locations from file '" << filepath << "'.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Save time stamp of first record
  time_firstInFile= time_recordStart;
  // Initialize time and data of current window (query window)
  time_currentWindowStart= time_recordStart;
  time_currentWindowEnd= time_recordEnd;
  if (valsAssignedToEOInt) {
    data_currentWindow= data_recordEnd;
  } else {
    data_currentWindow= data_recordStart;
  }
}


////////////////////////////////////////////////////////////////////////////////
// Get vector of location IDs
////////////////////////////////////////////////////////////////////////////////

const vector<string>& spaceTimeData::get_locations() const {
  if (empty()) {
		except e(__PRETTY_FUNCTION__,"Data object is empty.",__FILE__,__LINE__);
		throw(e);
  }
  return(locations);
}

////////////////////////////////////////////////////////////////////////////////
// Return current value for a certain location
////////////////////////////////////////////////////////////////////////////////

// Note: Smallest location index is 0
double spaceTimeData::get_value(const unsigned int indexLocation) const {
  if (empty()) {
		except e(__PRETTY_FUNCTION__,"Data object is empty.",__FILE__,__LINE__);
		throw(e);
  }
  if (indexLocation >= locations.size()) {
    stringstream errmsg;
    errmsg << "Cannot return data for location with index " << indexLocation <<
    " from file '" << filepath << "'. Index not in range [0," << (locations.size()-1) << "].";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return(data_currentWindow[indexLocation]);
}

////////////////////////////////////////////////////////////////////////////////
// Return the address of the value for a specified location
////////////////////////////////////////////////////////////////////////////////

// Note: Smallest location index is 0
const double* spaceTimeData::get_address(const unsigned int indexLocation) const {
  if (empty()) {
		except e(__PRETTY_FUNCTION__,"Data object is empty.",__FILE__,__LINE__);
		throw(e);
  }
  if (indexLocation >= locations.size()) {
    stringstream errmsg;
    errmsg << "Cannot return address of data for location with index " << indexLocation <<
    ". Index not in range [0," << (locations.size()-1) << "].";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return(&data_currentWindow[indexLocation]);
}

////////////////////////////////////////////////////////////////////////////////
// Update values (read data for specified time)
////////////////////////////////////////////////////////////////////////////////

void spaceTimeData::update(const fixedZoneTime &timeStart,
  const fixedZoneTime &timeEnd) {
  if (empty()) {
		except e(__PRETTY_FUNCTION__,"Data object is empty.",__FILE__,__LINE__);
		throw(e);
  }
  // Check target time window
  if (timeStart >= timeEnd) {
    stringstream errmsg;
    errmsg << "Cannot find data for target time window starting at '" <<
      time_currentWindowStart.get("-",":"," ") << "' and ending at '" <<
      time_currentWindowEnd.get("-",":"," ") << "' in file '" << filepath <<
      "'. The target time window must not be of zero or negative length.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Set current time window to target window (assume that the update succeedes)
  time_currentWindowStart= timeStart;
  time_currentWindowEnd=   timeEnd;
  // If the target window is too early
  if (time_currentWindowStart < time_recordStart) {
    if (time_currentWindowStart < time_firstInFile) {
      stringstream errmsg;
      errmsg << "Cannot find data for target time window starting at '" <<
        time_currentWindowStart.get("-",":"," ") << "' and ending at '" <<
        time_currentWindowEnd.get("-",":"," ") << "' in file '" << filepath <<
        "'. The data in the file do not (fully) cover the target time window.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    } else {
      stringstream errmsg;
      errmsg << "Cannot find data for target time window starting at '" <<
        time_currentWindowStart.get("-",":"," ") << "' and ending at '" <<
        time_currentWindowEnd.get("-",":"," ") << "' in file '" << filepath <<
        "'. The information is contained in this file but sequential" <<
        " access does not allow retrieval of data for times before '" <<
        time_recordStart.get("-",":"," ") << "' (start of active record)." <<
        " When making multiple calls of this method you must always move" <<
        " forward in time.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
  // Read new record(s) if the query window ends after the current record's ending time
  while (time_currentWindowEnd > time_recordEnd) {
    try {
      // read_data() updates 'time_recordEnd'
      // read_data() throws an exception if the end of file is reached
      read_data();
    } catch (except) {
      stringstream errmsg;
      errmsg << "Cannot find data for target time window starting at '" <<
        time_currentWindowStart.get("-",":"," ") << "' and ending at '" <<
        time_currentWindowEnd.get("-",":"," ") << "' in file '" << filepath <<
        "'. This may be due to one of the following reasons:" <<
        " (1) The data in the file do not (fully) cover the target time window." <<
        " (2) The file contains corrupt and/or incomplete record(s).";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
  // If we arrive here, we know that (time_currentWindowEnd <= time_recordEnd)
  // as this is the loop's exit condition and -- if it does not become true --
  // read_data() will throw an exception when the end of file is encountered.
  // Now check that (time_currentWindowStart >= time_recordStart)
  if (time_currentWindowStart < time_recordStart) {
    stringstream errmsg;
    errmsg << "Cannot find data for target time window starting at '" <<
      time_currentWindowStart.get("-",":"," ") << "' and ending at '" <<
      time_currentWindowEnd.get("-",":"," ") << "' in file '" << filepath <<
      "'. This may be due to one of the following reasons:" << 
      " (1) The data in the file do not (fully) cover the target time window." <<
      " (2) The target time window touches multiple time intervals defined in" <<
      " the file. This may happen, for example, if the time resolution in the" <<
      " file is higher than the resolution of the target window or if the target" <<
      " window is shifted against the time intervals given in the file." <<
      " (3) The times in the file do not increase with the record number.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Assign values to current window
  if (valsAssignedToEOInt) {
    data_currentWindow= data_recordEnd;
  } else {
    data_currentWindow= data_recordStart;
  }
  // If requested, reduce value by time fraction
  // --> Needs to be done for sum values (precipitation, energie sums, sunshine hours)
  // --> Must not be done for average values (temperature, flow rates, ...)
  if (redByTimeFract) {
    double fac;
    fac= (time_currentWindowEnd.get()-time_currentWindowStart.get()) / 
      (time_recordEnd.get()-time_recordStart.get());
    for (unsigned int i=0; i<data_currentWindow.size(); i++) {
      data_currentWindow[i]= data_currentWindow[i] * fac;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Query times
////////////////////////////////////////////////////////////////////////////////

fixedZoneTime spaceTimeData::get_time_firstInFile() const {
  if (empty()) {
		except e(__PRETTY_FUNCTION__,"Data object is empty.",__FILE__,__LINE__);
		throw(e);
  }
  return(time_firstInFile);
}

void spaceTimeData::get_times_currentWindow(fixedZoneTime &timeStart, fixedZoneTime &timeEnd) const {
  if (empty()) {
		except e(__PRETTY_FUNCTION__,"Data object is empty.",__FILE__,__LINE__);
		throw(e);
  }
  timeStart= time_currentWindowStart;
  timeEnd=   time_currentWindowEnd;
}

