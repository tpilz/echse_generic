
/*
!Hinweise:
!- Der Endpunkt einer Zone stimmt immer mit dem Anfangspunkt der folgenden Zone ueberein (doppelte Haltung der Endpunkte).
!  Dann kann keine Zone weniger als 2 Punkte haben und es lassen sich (z.B. Fläche) leicht berechnen
!- Die Rauhigkeit einer Zone wird jeweils aus der Rauhigkeitsinfo für den ersten Punkt ermittelt
!  (die Rauheit des letzten Punkts wird ignoriert).
!- Zonen werden automatisch dort neu begonnnen, wo die Rauheit wechselt. Sind gar keine Rauheiten sondern ist nur ein
!  globaler Default gegeben, dann wird nur eine einzige Zone gebildet.
!- Daten können als 2d (offset, elev) oder 3d (x,y,elev) gelesen werden. Hintenan kann jeweils noch die Rauheit stehen.
*/

#ifndef XSECTIONS_H
#define XSECTIONS_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "../except/except.h"
#include "../typeconv/typeconv.h"
#include "../table/table.h"
#include "../system/system.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Single point of a xsection
////////////////////////////////////////////////////////////////////////////////
struct xsection_point {
  double x,y,elev;   // coordinates in 3D
  double offset;     // offset from reference bank (left bank)
};

////////////////////////////////////////////////////////////////////////////////
// A subsection (or flow zone) of a xsection
////////////////////////////////////////////////////////////////////////////////
class xsection; // Forward declaration to use this as a friend
class xsection_zone {
  private:
    vector<xsection_point> point;
    double rough;      // property of the zone (though it is read for points)
    // Hydraulics methods
    void return_stagefunctions(const double wselev, double &flowarea,
      double &topwidth, double &wetperim) const;
    double return_flow(const double wselev, const double slope) const;
  public:
    // Constructor/destructor
    xsection_zone();
    ~xsection_zone();
  // Friends
  friend class xsection;
};

////////////////////////////////////////////////////////////////////////////////
// A complete xsection
////////////////////////////////////////////////////////////////////////////////
class xsection {
  private:
    bool coordinates3d;
    string name_river;
    string name_reach;
    double station;
    vector<xsection_zone> zone;
  public:
    // Constructor/destructor
    xsection();
    ~xsection();
    // Delete xsection data
    void clear();
    // Read xsection from file
    void read(const string datafile, const string chars_comment,
      const string chars_colsep, const string colname_offset,
      const string colname_x, const string colname_y,
      const string colname_elev, const string colname_rough,
      const string rivername, const string reachname, const double riverstation,
      const bool read3d, const bool readrough, const double defaultrough,
      const double position_nodata);
    // Print xsection to file
    void print(const string file, const bool replace) const;
    // Return geometric features
    void return_cutline(vector<double> &xvect, vector<double> &yvect) const;
    double return_min_elevation() const;
    double return_max_elevation() const;
    double return_left_elevation() const;
    double return_right_elevation() const;
    // Return attributes
    string get_rivername() const;
    string get_reachname() const;
    double get_station() const;
    // Hydraulics methods
    void return_stagefunctions(const double wselev, double &flowarea,
      double &topwidth, double &wetperim) const;
    double return_flow(const double wselev, const double slope) const;
    double return_surface(const double flow, const double slope,
      const double absprecision, const unsigned int maxniter) const;
};

#endif

