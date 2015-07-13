#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

#include <ctime>

#include "../../fixedZoneTime/fixedZoneTime.h"
#include "../spaceTimeData.h"

using namespace std;

template <class T>
string printVector(const vector<T> &v) {
  stringstream ss;
  for (unsigned int i=0; i<v.size(); i++) {
    ss << v[i] << " ";
  }
  return(ss.str());
}

int main() {


  try {

    const string file= "/home/dkneis/cpp/dklib/spaceTimeData/test/data.txt";
    const unsigned int locIndex=1;

    spaceTimeData dat;
    fixedZoneTime t_ini, t_end, t, t1, t2;
    vector<double> v;

    const double dt=3600;
    const bool EOI=true;
    const bool SUMS=false;
    const unsigned int bufSize= 1199;
    
    dat.init(file, "\t", "#",EOI,SUMS,bufSize);
    cout << "# Empty: " << dat.empty() << endl;
    cout << "# Locs: " << printVector(dat.get_locations()) << endl;
    cout << "# First: " << dat.get_time_firstInFile().get("-",":"," ") << endl;

    t_ini.set(2011,01,01,00,00,00);
    t_end.set(2011,01,03,00,00,00);
    t= t_ini;
    while (t < t_end) {
      // Update
      dat.update(t, t+dt);
      // Print
      dat.get_times_currentWindow(t1, t2);
      cout << t1.get("-",":"," ") << "\t" << t2.get("-",":"," ") << "\t" << dat.get_value(locIndex) << endl;
      // Update time
      t= t + dt;
    }  
    dat.clear();

	} catch (except) {
		except e ("main", "Failed.", __FILE__, __LINE__);
    e.print();
    return(1);
  }
  return(0);
}

