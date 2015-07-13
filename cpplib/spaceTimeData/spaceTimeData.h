
////////////////////////////////////////////////////////////////////////////////
/*
A class for space-time data at multiple locations.

NOTES:
- Objects of this class hold data for a single time intervall only (but all
  locations). Thus, objects do NOT hold data for all time intervals.
- Data are read, time step by time step, from a text file and stored in the
  object. Because the file is read sequentially, data may be updated only for
  times not earlier than the time of the current record (no backward update).
- There is a get method that retrieves the currently stored data (it does not
  search for data for a certain time interval). Data search is done with a
  separate update method.
  This technique is useful because:
  --> The currently stored data may be queried multiple times without new data
      search. For example, we may want to query values for a climate station
      at multiple locations for a certain time.
  --> We may use pointers, pointing to the currently stored data (with possibly
      many pointers pointing to a single data item (== location)). A single call
      of the update method will then update the data for all locations (this way
      updating the pointers' targets).
- Records read from the file are stored in an (intermediate) buffer and data are
  taken from this buffer until its contents is 'used up'. The size of the buffer
  is controlled by the 'bufferSize' argument of the 'init' method. If the value
  of bufferSize is 1, the file is accessed each time new data are needed. Using
  a larger buffer size may lead to faster execution as more data are read in a
  single disk operation without (or with less) repositioning of the head. Of
  courcse, using a large buffer increases memory consumption. 
- There is one basic restriction on the resolution of the query time window and
  the time interval of the data stored in the file: Basically, the query time
  window must never touch multiple time intervals present in the file.
  The consequences are:
  --> One CANNOT retrieve daily data from a file with hourly data.
      (Thus, data aggregation is not supported).
  --> It is OK to query hourly data from a file with a resolution of 1 day.
      (Thus, data disaggregation is OK. However, disaggregation just means that
      the 1h value is the same (for average data) or a fraction (for sum data)
      of the 1d value. I.e. there are no sophisticated disaggregation schemes.)
  --> One CANNOT retrieve data, even if the resolution of the query time window
      matches with the intervals in the file, if there is a bad time shift.
      (Example: The file contains hourly data starting some day at 00:00:00 UTC.
      Then, one cannot query data for a time window starting at the half hour, 
      such as 00:30:00.)
  --> The time intervals in the file can be regular but they do NOT need to as
      long as the basic restriction is considered. For example, the file may
      contain a mix of hourly and daily data if the query time window has a
      size of one hour or less.

  The restriction in a sketch:

  Time intervals in file   ---------+-----------+--------+---------+----> time
  Query time window                 |-----------|  OK!

  Time intervals in file   ---------+-----------+--------+---------+----> time
  Query time window                   |-----|  OK!

  Time intervals in file   ---------+-----------+--------+---------+----> time
  Query time window              |-----|  NOT OK!

  Time intervals in file   ---------+-----------+--------+---------+----> time
  Query time window              |-----------------|  NOT OK!


- When initializing an object of the class, one must specify whether the values
  in the file are assgined to the end of the interval (usual convention) or to
  the beginning of the interval (Argument 'valuesAssignedToEndOfInterval').
  See sketch below:

  Time intervals in file   -----+-----------+--------+-----> time
  Values in file               14           7       88    
  Query time window               |---|
  Value for query time window       7    if valuesAssignedToEndOfInterval==TRUE
  Value for query time window      14    if valuesAssignedToEndOfInterval==FALSE

- When initializing an object of the class, one must specify whether on querying
  the values need to be reduced if the query time window is shorted than the
  corresponding interval in the data file based in the ratio of the lengths of
  the intervals (Argument 'reduceByTimeFraction').
  --> For sum values (e.g. precipitation) use reduceByTimeFraction=TRUE
  --> For average values (e.g. temperature) use reduceByTimeFraction=FALSE
  See sketch below (which assumes valuesAssignedToEndOfInterval==TRUE):
 
  Time intervals in file   -----+-----------+--------+-----> time
  Values in file               14           7       88    
  Query time window                   |-----|
  Value for query time window            7    if reduceByTimeFraction==FALSE
  Value for query time window           3.5   if reduceByTimeFraction==TRUE

TECHNICAL NOTES:
The class has an ifstream member, which is basically a link to an open file.
Therefore, objects of this class must not be copied. To restrict copying, 
the "=" operator and the copy constructor are private and unimplemented.
Due to that, this class cannot be used with the STL containers, i.e. one
cannot use vector<spaceTimeData> and the like.
To make an array of spaceTimeData objects, one must use the basic C++ method
of creating objects on the heap with 'new' and using pointers to manage the
objects. The array needs to be removed using 'delete'.

Example:

const unsigned int N=5;
vector<spaceTimeData*> ps;
ps.resize(N);
for (unsigned int i=0; i<N; i++) {
  ps[i]= new spaceTimeData();
  ps[i]->init(FILES[i],"\t","#",true);  
}
for (unsigned int i=0; i<ps.size(); i++) {
  delete ps[i];
}
ps.clear();

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef SPACETIMEDATA_H
#define SPACETIMEDATA_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "../except/except.h"
#include "../fixedZoneTime/fixedZoneTime.h"
#include "../stringmanip/stringmanip.h"
#include "../typeconv/typeconv.h"
#include "../system/system.h"

using namespace std;

class spaceTimeData {
  private:
    ifstream ifs;
    string filepath;
    string charsColSep;
    string charsComment;
    // Data
    vector<string> locations;
    vector<double> data_recordStart;
    vector<double> data_recordEnd;
    vector<double> data_currentWindow;
    // Counters
    unsigned int lineCurrent;
    unsigned int recNumber;
    // Record buffer
    unsigned int indexBuffer;
    unsigned int nRecsInBuffer;
    vector<string> buffer;
    // Times
    fixedZoneTime time_recordStart;
    fixedZoneTime time_recordEnd;
    fixedZoneTime time_currentWindowStart;
    fixedZoneTime time_currentWindowEnd;
    fixedZoneTime time_firstInFile;
    bool valsAssignedToEOInt;
    bool redByTimeFract;
    // Private methods
    void read_locations();
    void read_data();
    // Don't allow assignment and use of the copy ctor because of the ifstream
    // member by making these methods private and omitting the definition
    spaceTimeData(const spaceTimeData &x);
    spaceTimeData& operator=(const spaceTimeData &x);
  public:
    // Constructors, destructors
    spaceTimeData();
    ~spaceTimeData();
    // Diagnostic methods
    bool empty() const;
    // Initialization and clean up
    void init(const string &file, const string &chars_colsep,
      const string &chars_comment, const bool valuesAssignedToEndOfInterval,
      const bool reduceByTimeFraction, const unsigned int bufferSize);
    void clear();
    // Get vector of location IDs
    const vector<string>& get_locations() const;
    // Return current value for a specified location
    double get_value(const unsigned int indexLocation) const;
    // Return the address of the value for a specified location
    const double* get_address(const unsigned int indexLocation) const;
    // Update values (read data for specified time)
    void update(const fixedZoneTime &timeStart, const fixedZoneTime &timeEnd);
    // Query times
    fixedZoneTime get_time_firstInFile() const;
    void get_times_currentWindow(fixedZoneTime &timeStart, fixedZoneTime &timeEnd) const;
};

#endif

