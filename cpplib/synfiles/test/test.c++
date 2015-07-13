#include<iostream>
#include<sstream>
#include<string>

#include "../synfiles.h"
#include "../../except/except.h"
#include "../../fixedZoneTime/fixedZoneTime.h"

using namespace std;

int main() {

  string synfile="/home/dkneis/progress/hypso/neckar/daten_hvz/meteo/syn/66_4463.syn";
  const bool in_timeAtEOI=true;

  string location;
  vector<double> values;
  fixedZoneTime firstEOI;

  try {
    read_syn (
      synfile,
      in_timeAtEOI,
      location,
      values,
      firstEOI
    );
  } catch (except) {
    except e("Main","Error in function",__FILE__,__LINE__);
    e.print();
    return(1);
  }

  cout << "--- Function OK. Data in main: ---" << endl;
  cout << "LOCATION=  " << location << endl;
  cout << "FIRST EOI= " << firstEOI.get("-",":"," ") << endl;
  cout << "NDATA=     " << values.size() << endl;

  return(0);
}

