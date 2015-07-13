
#include "logfile.h"

// Close method
void logfile::close() {
  filename="";
  if (ofs.is_open()) ofs.close();
}

// Destructor
logfile::~logfile() {
  close();
}

// Constructor
logfile::logfile() {
  filename="";
}

// Init method
void logfile::init(const string &file, const bool replace) {
  filename= file;
  if (file_exists(filename) & (!replace)) {
    stringstream errmsg;
    errmsg << "Cannot create log file '" << filename << "'. File already exists.";
		except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		throw(e);
  }
  ofs.open(filename.c_str());
  if (!ofs.is_open()) {
    stringstream errmsg;
    errmsg << "Unable to open log file '" << filename << "' for writing.";
		except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		throw(e);
  }
  ofs << "UTC" << "\t" << "event" << endl;
}

// Method to add data
void logfile::add(const bool silent, const string &message) {
  stringstream ss;
  time_t now_in_seconds;
  tm * ptm;
  time(&now_in_seconds);
  ptm= gmtime(&now_in_seconds);
  ss <<
    ptm->tm_year+1900 << "-" <<
    setw(2) << setfill('0') << ptm->tm_mon+1 << "-" <<
    setw(2) << setfill('0') << ptm->tm_mday << " " <<
    setw(2) << setfill('0') << ptm->tm_hour << ":" <<
    setw(2) << setfill('0') << ptm->tm_min << ":" <<
    setw(2) << setfill('0') << ptm->tm_sec << "\t" <<
    message << endl;
  if (ofs.is_open()) ofs << ss.str() << flush;         // Send to logfile
  if ((!ofs.is_open()) | (!silent)) cout << ss.str();  // Send to stdout
}


