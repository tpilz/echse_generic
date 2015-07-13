#include<iostream>
#include<string>
#include<vector>
#include "../xsections.h"
using namespace std;

int main() {

  const string datafile="/home/dkneis/cpp/dklib/xsections/test/xs.txt";
  const bool read3d=true;
  const bool readrough=true;
  const double defaultrough= 30;
  vector<double> x, y; 
  
  const double slope= 0.01;

  double elev, flow;
  double a, w, p;

  try {

    xsection xs;

    // Read
    cout << "Reading..." << endl;
    xs.read(datafile, "#", "\t ", "offset", "x", "y", "elev", "rough", 
      "river_1", "reach_1", 1.0, read3d, readrough, defaultrough, -9999.0);

    // Write
    cout << "Writing..." << endl;
    xs.print((datafile+".out"), true);

    // Attribs
    cout << "River= " << xs.get_rivername() << endl;
    cout << "Reach= " << xs.get_reachname() << endl;
    cout << "Station= " << xs.get_station() << endl;

    // Geometry
    xs.return_cutline(x, y);
    cout << "cutline nx: " << x.size() << " 1st: " << x[0] << " nth: " << x[x.size()-1] << endl;
    cout << "cutline ny: " << y.size() << " 1st: " << y[0] << " nth: " << y[y.size()-1] << endl;
    cout << "Min elev= " << xs.return_min_elevation() << endl;
    cout << "Max elev= " << xs.return_max_elevation() << endl;

    elev= 0.5 * (xs.return_min_elevation() + xs.return_max_elevation());

    // Stage functions
    xs.return_stagefunctions(elev, a, w, p);
    cout << "At h=" << elev << " we have area=" << a << " width=" << w << " perim=" << p << endl;
    
    // Elev --> flow --> elev
    flow= xs.return_flow(elev, slope);
    cout << "q (h = " << elev << ")= " << flow << endl;
    elev= xs.return_surface(flow, slope, 0.001, 200);
    cout << "h (q = " << flow << ")= " << elev << endl;

    // Delete
    cout << "Deleting..." << endl;
    xs.clear();


	} catch (except) {
		except e ("main", "Failed.", __FILE__, __LINE__);
    e.print();
    return(1);
  }
  return(0);
}

