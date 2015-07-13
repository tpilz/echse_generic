
#ifndef EXCEPT_H
#define EXCEPT_H

#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>

using namespace std;

//==============================================================================
// Class to represent a vector of exceptions (error trace)
//==============================================================================

class except {
  private:
    // Types
    typedef unsigned int size_type;
    // Nested (private) class to deal with a single exception
		class single_exception {
			private:
				string function;
				string message;
				string sourcefile;
				int sourceline;
			public:
				single_exception(const string func, const string mess, const string file, const int line) {
				  function= func;
				  message= mess;
				  sourcefile= file;
				  sourceline= line;
				}
				~single_exception() {
				  function= "";
				  message= "";
				  sourcefile= "";
				  sourceline= 0;
				}
				string get_func() const { return(function); }
				string get_mess() const { return(message); }
				string get_file() const { return(sourcefile); }
				int get_line() const { return(sourceline); }
		};
    // Data (static, to collect info on all exceptions in all program units)
		static vector<single_exception> exceptions;
  public:
    // Constructors (message may be a string or a stringstream)
    except (const string func, const string mess, const string file, const int line);
    except (const string func, const stringstream& mess, const string file, const int line);
    // Public methods
    int count() const;                   // Returns the total number of recorded exceptions
    void print() const;                  // Sends info on all recorded exceptions to stderr
    void print(const string file, const string fmt) const; // Sends info to file (fmt=xml|html|"")
    void clear();                        // Removes info on all recorded exceptions
};

#endif

