
#include "xsections.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// METHODS OF CLASS 'XSECTION_ZONE'
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Constructor & destructor
////////////////////////////////////////////////////////////////////////////////
xsection_zone::xsection_zone() { }
xsection_zone::~xsection_zone() {
  rough= 0.0;
  point.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Returns stage dependent functions for a single zone
////////////////////////////////////////////////////////////////////////////////
void xsection_zone::return_stagefunctions(const double wselev,
  double &flowarea, double &topwidth, double &wetperim) const
{
  const double ZERO=0.0, TWO=2.0, HALF=0.5;
  double flowarea_sum, topwidth_sum, wetperim_sum;
  double minelev, maxelev, offsetdiff;
  // Code
  flowarea_sum= ZERO;
  topwidth_sum= ZERO;
  wetperim_sum= ZERO;
  for (unsigned int i=0; i<(point.size()-1); i++) {
    minelev= min(point[i].elev, point[i+1].elev);
    maxelev= max(point[i].elev, point[i+1].elev);
    offsetdiff= abs(point[i].offset - point[i+1].offset);
    // If stage is completely below the bottom
    if (wselev <= minelev) {
      topwidth= ZERO;
      flowarea= ZERO;
      wetperim= ZERO;
    // If bottom between adjacent points is completely submerged
    } else if (wselev >= maxelev) {
      topwidth= offsetdiff;
      flowarea= ((wselev - point[i].elev) + (wselev - point[i+1].elev)) /
        TWO * topwidth;
      wetperim= pow((pow((point[i].elev - point[i+1].elev),TWO)
        + pow(topwidth,TWO)), HALF);
    // If bottom is only partially submerged
    } else {
      topwidth= offsetdiff * ((wselev - minelev) / (maxelev - minelev)); 
      flowarea= topwidth * (wselev - minelev) / TWO;
      wetperim= pow( (pow(topwidth,TWO) + pow((wselev - minelev),TWO)), HALF);          
    }
    flowarea_sum= flowarea_sum + flowarea;
    topwidth_sum= topwidth_sum + topwidth;
    wetperim_sum= wetperim_sum + wetperim;
  }
  flowarea= flowarea_sum;
  topwidth= topwidth_sum;
  wetperim= wetperim_sum;
}

////////////////////////////////////////////////////////////////////////////////
// Returns the flow through a single zone according to mannings equation
////////////////////////////////////////////////////////////////////////////////
double xsection_zone::return_flow(const double wselev, const double slope) const
{
  const double EXPO=2.0/3.0, ZERO=0.0, HALF=0.5, NEGATIVE=-1.0;
  double flow, flowarea, topwidth, wetperim, hydradius;
  // Code
  return_stagefunctions(wselev, flowarea, topwidth, wetperim);
  if (wetperim > ZERO) {
    hydradius= flowarea / wetperim;
    if (slope > ZERO) {
      flow= rough * pow(slope, HALF) * pow(hydradius, EXPO) * flowarea;
    } else if (slope < ZERO) {
      flow= NEGATIVE * rough * pow(abs(slope), HALF) *
        pow(hydradius, EXPO) * flowarea;
    } else {
      flow= ZERO;
    }
  } else {
    flow= ZERO;
  }
  return(flow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// METHODS OF CLASS 'XSECTION'
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Constructor & destructor
////////////////////////////////////////////////////////////////////////////////
xsection::xsection() {}
xsection::~xsection() {
  coordinates3d= false;
  name_river= "";
  name_reach= "";
  station= 0.0;
  zone.clear();
}
void xsection::clear() {
  coordinates3d= false;
  name_river= "";
  name_reach= "";
  station= 0.0;
  zone.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Read xsection (different formats supported)
////////////////////////////////////////////////////////////////////////////////
void xsection::read(
  const string datafile, const string chars_comment, const string chars_colsep,
  const string colname_offset, const string colname_x, const string colname_y,
  const string colname_elev, const string colname_rough,
  const string rivername, const string reachname, const double riverstation,
  const bool read3d, const bool readrough, const double defaultrough,
  const double position_nodata
) {
  const double ZERO=0.0, TWO=2.0, HALF=0.5;
  table tab;
  vector<string> strings;
  vector<double> x, y, offset, elev, rough;
  unsigned int noffsets, nzones, npoints;
  unsigned int zoneindex;
  // Set descrption data
  name_river= rivername;
  name_reach= reachname;
  station= riverstation;
  coordinates3d= read3d;
  // Read input table

  try {
    tab.read(datafile, true, chars_colsep, chars_comment);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Could not read xsection data from file '" << datafile << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  noffsets= tab.nrow();
  if (noffsets < 2) {
    stringstream errmsg;
    errmsg << "Xsection defined in file '" << datafile << "' has too few offsets.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  // Fill required data vectors
  try {
    // 3d-format, given roughness
    if (read3d && readrough) {
      tab.get_col(tab.colindex(colname_x), strings);
        convert_type(strings, x);
      tab.get_col(tab.colindex(colname_y), strings);
        convert_type(strings, y);
      tab.get_col(tab.colindex(colname_elev), strings);
        convert_type(strings, elev);
      tab.get_col(tab.colindex(colname_rough), strings);
        convert_type(strings, rough);
      offset.resize(x.size());
      for (unsigned int i=0; i<offset.size(); i++) {
        if (i == 0) {
          offset[i]= ZERO;
        } else {
          offset[i]= offset[i-1] +
            pow(pow((x[i]-x[i-1]),TWO) + pow((y[i]-y[i-1]),TWO), HALF);
        }
      }
    // 3d-format, default roughness
    } else if (read3d && (!readrough)) {
      tab.get_col(tab.colindex(colname_x), strings);
        convert_type(strings, x);
      tab.get_col(tab.colindex(colname_y), strings);
        convert_type(strings, y);
      tab.get_col(tab.colindex(colname_elev), strings);
        convert_type(strings, elev);
      rough.resize(x.size());
      for (unsigned int i=0; i<rough.size(); i++) { rough[i]= defaultrough; }
      offset.resize(x.size());
      for (unsigned int i=0; i<offset.size(); i++) {
        if (i == 0) {
          offset[i]= ZERO;
        } else {
          offset[i]= offset[i-1] +
            pow(pow((x[i]-x[i-1]),TWO) + pow((y[i]-y[i-1]),TWO), HALF);
        }
      }
    // offset-format, given roughness
    } else if ((!read3d) && readrough) {
      tab.get_col(tab.colindex(colname_offset), strings);
        convert_type(strings, offset);
      tab.get_col(tab.colindex(colname_elev), strings);
        convert_type(strings, elev);
      tab.get_col(tab.colindex(colname_rough), strings);
        convert_type(strings, rough);
      x.resize(offset.size());
      for (unsigned int i=0; i<x.size(); i++) { x[i]= position_nodata; }
      y.resize(offset.size());
      for (unsigned int i=0; i<y.size(); i++) { y[i]= position_nodata; }
    // offset-format, default roughness
    } else if ((!read3d) && (!readrough)) {
      tab.get_col(tab.colindex(colname_offset), strings);
        convert_type(strings, offset);
      tab.get_col(tab.colindex(colname_elev), strings);
        convert_type(strings, elev);
      rough.resize(offset.size());
      for (unsigned int i=0; i<rough.size(); i++) { rough[i]= defaultrough; }
      x.resize(offset.size());
      for (unsigned int i=0; i<x.size(); i++) { x[i]= position_nodata; }
      y.resize(offset.size());
      for (unsigned int i=0; i<y.size(); i++) { y[i]= position_nodata; }
    }
  } catch (except) {
    stringstream errmsg;
    errmsg << "Could not extract xsection data from column(s) of file '" <<
      datafile << "'.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  tab.clear();
  // Check offsets
  for (unsigned int i=0; i<(noffsets-1); i++) {
    if (offset[i+1] < offset[i]) {
      stringstream errmsg;
      errmsg << "Offsets read/created from data in file '" <<
        datafile << "' must be in weakly ascending order. First bad" <<
        " value is " << offset[i+1] << " (offset number " << i+1+1 << ").";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
  }
  // Allocate the correct number of zones (new zone starts where roughness
  // changes, last point is ignored)
  nzones= 1;
  for (unsigned int i=0; i<(noffsets-1); i++) {
    if (i > 0) {
      if (rough[i] != rough[i-1]) nzones++;
    }
  }
  zone.resize(nzones);
  // Allocate the correct number of points for each zone and assign the data
  // Note: Points at interface of zones are counted twice (for both adjacent zones)
  npoints= 0;
  zoneindex= 0;
  for (unsigned int i=0; i<noffsets; i++) {
    if (i == 0) {
      // Initialize first zone
      zoneindex= 0;
      npoints= 1;
    } else if ((i > 0) && (i < (noffsets-1))) {
      if (rough[i] != rough[i-1]) {
        // Previous zone completed - save data
        npoints++;
        zone[zoneindex].point.resize(npoints);
        for (unsigned int k=0; k<npoints; k++) {
          zone[zoneindex].point[k].x = x[i-(npoints-1)+k];
          zone[zoneindex].point[k].y = y[i-(npoints-1)+k];
          zone[zoneindex].point[k].offset = offset[i-(npoints-1)+k];
          zone[zoneindex].point[k].elev = elev[i-(npoints-1)+k];
        }
        zone[zoneindex].rough = rough[i-1];
        // Initialize new zone
        zoneindex++;
        npoints= 1;
      } else {
        npoints++;
      }
    } else if (i == (noffsets-1)) {
      // Last zone completed - save data
      npoints++;
      zone[zoneindex].point.resize(npoints);
      for (unsigned int k=0; k<npoints; k++) {
        zone[zoneindex].point[k].x = x[i-(npoints-1)+k];
        zone[zoneindex].point[k].y = y[i-(npoints-1)+k];
        zone[zoneindex].point[k].offset = offset[i-(npoints-1)+k];
        zone[zoneindex].point[k].elev = elev[i-(npoints-1)+k];
      }
      zone[zoneindex].rough = rough[i-1];
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Return the cutline of a xsection (xsection must be in 3d format)
////////////////////////////////////////////////////////////////////////////////
void xsection::return_cutline(vector<double> &xvect, vector<double> &yvect) const
{
  // Check data
  if (!coordinates3d) {
    stringstream errmsg;
    errmsg << "Can't return cutline for xsection at river '" <<
      name_river << "' at reach '" << name_reach << "' at station " <<
      station << ". XY-coordinates not available.";
    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
    throw(e);
  }
  // Note: zones 2..n share a common point
  xvect.resize(0);
  yvect.resize(0);
  for (unsigned int i=0; i<zone.size(); i++) {
    if (i == 0) {
      for (unsigned int k=0; k<zone[i].point.size(); k++) {
        xvect.push_back(zone[i].point[k].x);
        yvect.push_back(zone[i].point[k].y);
      }
    } else {
      for (unsigned int k=1; k<zone[i].point.size(); k++) {
        xvect.push_back(zone[i].point[k].x);
        yvect.push_back(zone[i].point[k].y);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Return xsection attributes
////////////////////////////////////////////////////////////////////////////////
string xsection::get_rivername() const { return (name_river); }
string xsection::get_reachname() const { return (name_reach); }
double xsection::get_station() const { return (station); }

////////////////////////////////////////////////////////////////////////////////
// Print xsection to textfile
////////////////////////////////////////////////////////////////////////////////
void xsection::print(const string file, const bool replace) const {
  // Open output file
  if (file_exists(file) && (!replace)) {
    stringstream errmsg;
    errmsg << "File '" << file << "' already exists. Replacement not permitted.";
		except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		throw(e);
  }
  ofstream ofile;
  ofile.open(file.c_str());
  if (!ofile.is_open()) {
    stringstream errmsg;
    errmsg << "Unable to open file '" << file << "' for writing.";
		except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		throw(e);
  }
  // Header
  ofile << "# River:           " << name_river << endl;
  ofile << "# Reach:           " << name_reach << endl;
  ofile << "# Station:         " << station << endl;
  ofile << "# 3D-data?:        " << coordinates3d << endl;
  ofile << "# Number of zones: " << zone.size() << endl;
  ofile << "xcoord\tycoord\toffset\televation\troughness\tzone_index" << endl;
  for (unsigned int i=0; i<zone.size(); i++) {
    for (unsigned int k=0; k<zone[i].point.size(); k++) {
      ofile << zone[i].point[k].x << "\t" << zone[i].point[k].y << "\t" <<
        zone[i].point[k].offset << "\t" << zone[i].point[k].elev << "\t" <<
        zone[i].rough << "\t" << i << endl;
    }
  }
  ofile.close();
}

////////////////////////////////////////////////////////////////////////////////
// Returns stage dependent functions for a complete cross-section
////////////////////////////////////////////////////////////////////////////////
void xsection::return_stagefunctions(const double wselev,
  double &flowarea, double &topwidth, double &wetperim) const {
  const double ZERO= 0.0;
  double flowarea_zone, topwidth_zone, wetperim_zone;
  // Code
  flowarea= ZERO;
  topwidth= ZERO;
  wetperim= ZERO;
  for (unsigned int i=0; i<zone.size(); i++) {
    zone[i].return_stagefunctions(wselev, flowarea_zone, topwidth_zone,
      wetperim_zone);
    flowarea= flowarea + flowarea_zone;
    topwidth= topwidth + topwidth_zone;
    wetperim= wetperim + wetperim_zone;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Returns the flow through a complete xsection according to mannings equation
////////////////////////////////////////////////////////////////////////////////
double xsection::return_flow(const double wselev, const double slope) const
{
  const double ZERO=0.0;
  double flow;
  // Code
  flow= ZERO;
  for (unsigned int i=0; i<zone.size(); i++) {
    flow= flow + zone[i].return_flow(wselev, slope);
  }
  return(flow);
}

////////////////////////////////////////////////////////////////////////////////
// Returns the lowest and highest elevation in a xsection
////////////////////////////////////////////////////////////////////////////////
double xsection::return_min_elevation() const {
  double elev;
  elev= zone[0].point[0].elev;
  for (unsigned int i=0; i<zone.size(); i++) {
    for (unsigned int k=0; k<zone[i].point.size(); k++) {
      if (zone[i].point[k].elev < elev) {
        elev= zone[i].point[k].elev;
      }
    }
  }
  return(elev);
}
double xsection::return_max_elevation() const {
  double elev;
  elev= zone[0].point[0].elev;
  for (unsigned int i=0; i<zone.size(); i++) {
    for (unsigned int k=0; k<zone[i].point.size(); k++) {
      if (zone[i].point[k].elev > elev) {
        elev= zone[i].point[k].elev;
      }
    }
  }
  return(elev);
}


////////////////////////////////////////////////////////////////////////////////
// Elevation of left and right border
////////////////////////////////////////////////////////////////////////////////
double xsection::return_left_elevation() const {
  return( zone[0].point[0].elev );
}
double xsection::return_right_elevation() const {
  return( zone[zone.size()-1].point[zone[zone.size()-1].point.size()-1].elev );
}

////////////////////////////////////////////////////////////////////////////////
// Returns the water surface elevation in a cross section for a given flow and
// bottom slope using manning sequation (normal depth computation).
////////////////////////////////////////////////////////////////////////////////
double xsection::return_surface(const double flow, const double slope,
  const double absprecision, const unsigned int maxniter) const
{
  const double TWO=2.0;
  double wse_lower, wse_upper, wse_middle;
  double flow_lower, flow_upper, flow_middle;
  double normalwse;
  unsigned int niter;
  // Set initial estimates of water surface elevation for iterative solution
  wse_lower= return_min_elevation();
  wse_upper= return_min_elevation() +  // Above x-section capacity
    (return_max_elevation() - return_min_elevation()) * 2.0;
  // Perform iteration until the normal depth is calculated
  niter= 1;
  while (true) {
    // Calculate the flow for the estimated water levels
    flow_lower= return_flow(wse_lower, slope);
    flow_upper= return_flow(wse_upper, slope);
    // Exit iteration if one of the estimates is a solution
    if (abs(flow_lower - flow) <= absprecision) {
      normalwse= wse_lower;
      break;
    } else if (abs(flow_upper - flow) <= absprecision) {
      normalwse= wse_upper;
      break;
    }   
    // Check whether the solution is enclosed by the estimates.
    // The solution may be outside the search interval if the solution elevation
    // is higher than the initial upper estimate.
    if ((flow < flow_lower) || (flow > flow_upper)) {
      stringstream errmsg;
      errmsg << "Could not compute water surface elevation for steady" <<
        " uniform flow with a flow rate of " << flow << " using a slope of " <<
        slope << " for cross-section at river '" << name_river << "', reach '" <<
        name_reach << "', river station " << station << ". The solution is" <<
        " outside the expected range which is possibly due to a flow rate" <<
        " exceeding the cross-section's capacity by far.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }    
    // Bisect the interval between the estimates and calculate the flow
    wse_middle= wse_lower + (wse_upper-wse_lower) / TWO;
    flow_middle= return_flow(wse_middle, slope);
    // Exit if new estimate is a solution or adjust search interval otherwise
    if (abs(flow_middle - flow) <= absprecision) {
      normalwse= wse_middle;
      break;
    }
    // Adjust the search interval
    if (flow_middle > flow) {
      wse_upper= wse_middle;
    } else if (flow_middle < flow) {
      wse_lower= wse_middle;
    }    
    // Check for too many iterations and set counter
    if (niter == maxniter) {
      stringstream errmsg;
      errmsg << "Could not compute water surface elevation for steady" <<
        " uniform flow with a flow rate of " << flow << " using a slope of " <<
        slope << " for cross-section at river '" << name_river << "', reach '" <<
        name_reach << "', river station " << station << ". A solution with" <<
        " the required absolute precision of " << absprecision << " could" <<
        " not be found within " << niter << " iterations.";
      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
      throw(e);
    }
    niter++;
  }
  return(normalwse);
}

