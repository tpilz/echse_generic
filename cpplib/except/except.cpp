
#include "except.h"

//==============================================================================
// Definition of static members variables
//==============================================================================

vector<except::single_exception> except::exceptions;

//==============================================================================
// Constructors
//==============================================================================

// Create exception (message as string)
except::except (const string func, const string mess, const string file, const int line) {
  single_exception se(func, mess, file, line);
  exceptions.push_back(se);
}

// Create exception (message as stringstream)
except::except (const string func, const stringstream& mess, const string file, const int line) {
  single_exception se(func, mess.str(), file, line);
  exceptions.push_back(se);
}

//==============================================================================
// Definition of methods
//==============================================================================

// Count the number of recorded exceptions
int except::count () const {
  return(exceptions.size());
}
// Send info on all recorded exceptions to stderr
void except::print () const {
  const size_type n= exceptions.size();
  if (n > 0) {
    for (size_type i=0; i<n; i++) {
		  cerr << exceptions[i].get_func() << ": " << exceptions[i].get_mess() <<
		    " (Thrown at line " << exceptions[i].get_line() << " of '" << 
		    exceptions[i].get_file() << "').\n";
		}
  }
}
// Send info on all recorded exceptions to file
void except::print (const string file, const string fmt) const {
  const size_type n= exceptions.size();
  if (n > 0) {
    // Create time info
    time_t now_in_seconds;
    tm * ptm;
    time(&now_in_seconds);
    ptm= gmtime(&now_in_seconds);
    stringstream ISOdatetime;
    ISOdatetime << ptm->tm_year+1900 << "-" << setw(2) << setfill('0') << ptm->tm_mon+1 << "-" <<
      setw(2) << setfill('0') << ptm->tm_mday << " " << setw(2) << setfill('0') << ptm->tm_hour << ":" <<
      setw(2) << setfill('0') << ptm->tm_min << ":" << setw(2) << setfill('0') << ptm->tm_sec;
    // Try to send traceback to file
    fstream ofile;
    ofile.open(file.c_str(), ios::ate | ios::in | ios::out); // Try to open existing file in append mode
    if (!ofile.is_open()) ofile.open(file.c_str(), ios::out); // If that failed, try to open a new file
    if (ofile.is_open()) {
      if (fmt == "xml") {
        ofile << "<traceback datetime=\"" << ISOdatetime.str() << "\">\n";
        for (size_type i=0; i<n; i++) {
          ofile << "  <exception level=\"" << i+1 << "\">\n";
	  	    ofile << "    <method>" << exceptions[i].get_func() << "</method>\n";
	  	    ofile << "    <condition>" << exceptions[i].get_mess() << "</condition>\n";
          ofile << "    <sourcefile>" << exceptions[i].get_file() << "</sourcefile>\n";
          ofile << "    <sourceline>" << exceptions[i].get_line() << "</sourceline>\n";
          ofile << "  </exception>\n";
  		  }
        ofile << "</traceback>\n";
      } else if (fmt == "html") {
        ofile << "<p>Traceback created on \"" << ISOdatetime.str() << "\"\n";
        ofile << "<table border=TRUE>\n";
          ofile << "<tr>\n";
          ofile << "<td>Level</td><td>Condition</td><td>Method</td><td>Sourcefile</td><td>Sourceline</td>\n";
          ofile << "</tr>\n";
        for (size_type i=0; i<n; i++) {
          ofile << "<tr>\n";
          ofile << "<td>" << i+1 << "</td>";
	  	    ofile << "<td>" << exceptions[i].get_mess() << "</td>";
	  	    ofile << "<td>" << exceptions[i].get_func() << "</td>";
          ofile << "<td>" << exceptions[i].get_file() << "</td>";
          ofile << "<td>" << exceptions[i].get_line() << "</td>\n";
          ofile << "</tr>\n";
  		  }
        ofile << "</table>\n";
      } else {  // txt format
        ofile << "datetime\tlevel\tmethod\tcondition\tsourcefile\tsourceline\n";
        for (size_type i=0; i<n; i++) {
          ofile << ISOdatetime.str() << "\t" << i+1 << "\t" << exceptions[i].get_func() <<
            "\t" << exceptions[i].get_mess() << "\t" << exceptions[i].get_file() <<
            "\t" << exceptions[i].get_line() << "\n";
  		  }
      }
      ofile.close();
    // Send traceback to screen if output to file failed
    } else {
      print();
    }
  }
}
// Remove all recorded exceptions
void except::clear () {
  exceptions.clear();
}

