
#include "functions.h"

/*******************************************************************************
 Static members
*******************************************************************************/
const double tblFunction::FRACTOL_XSPACING= 1.0e-06;

/*******************************************************************************
 Constructors / destructors
*******************************************************************************/
tblFunction::tblFunction() {
  name= "";
  narg= 0;
  index_lastaccess= 0;
}
tblFunction::~tblFunction() {
  name= "";
  narg= 0;
  index_lastaccess= 0;
  x.resize(0);
  fx.resize(0);
}

/*******************************************************************************
 Method to initialize a function table using two arrays
*******************************************************************************/
void tblFunction::init(const string funcname, const vector<double> &args,
  const vector<double> &vals
) {
  const double ZERO= 0.0;
  double xspacing, xspacing_first;
  name= funcname;
  narg= args.size();
  // Check input dimensions
  if (narg < 2) {
    stringstream errmsg;
    errmsg << "Array of arguments for function '" << name << "' has length " <<
      narg << ". Need at least two pairs of (argument, value).";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  if (narg != vals.size()) {
    stringstream errmsg;
    errmsg << "Array of arguments and array of values for function '" <<
      name << "' differ in length.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Copy
  x.resize(narg);
  fx.resize(narg);
  x= args;
  fx= vals;
  // Initialize access index
  index_lastaccess= floor(narg/2.);
  // Check arguments for ascending order and find out whether x is equispaced  
  x_equispaced= true;
  xspacing_first= x[1] - x[0];
  for (size_type i=1; i<narg; i++) {
    xspacing= x[i] - x[i-1];
    if (xspacing <= ZERO) {
      stringstream errmsg;
      errmsg << "Arguments for function '" << name << "' not in ascending" <<
        " order. Value at" << " position " << i << " (" << x[i] <<
        ") must be greater than value" << " at position " << (i-1) << " (" <<
        x[i-1] << ").";  
		  except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		  throw(e);      
    }
    if (abs(xspacing-xspacing_first) > (FRACTOL_XSPACING * 
      max(xspacing,xspacing_first))) {
      x_equispaced= false;
    }
  }  
}

/*******************************************************************************
 Methods to initialize a function table from a tabular data file
*******************************************************************************/
void tblFunction::read(const string file, const bool header,
  const string chars_colsep, const string chars_comment, const string funcname,
  const string colname_x, const string colname_fx
) {
	table tab;
	vector<string> x_str, fx_str;
  vector<double> x_num, fx_num;
  name= funcname;
	try {
		tab.read(file, header, chars_colsep, chars_comment);
		tab.get_col(tab.colindex(colname_x), x_str);
		tab.get_col(tab.colindex(colname_fx), fx_str);
    convert_type(x_str, x_num);
    convert_type(fx_str, fx_num);
/* This alternative code for the two statements above has very similar performance
    x_num.resize(x_str.size());
    fx_num.resize(fx_str.size());
    for (unsigned int i=0; i<x_str.size(); i++) {
      x_num[i]= as_double(x_str[i]); 
      fx_num[i]= as_double(fx_str[i]); 
    }
*/
		init(funcname, x_num, fx_num);
	} catch (except) {
		stringstream errmsg;
		errmsg << "Could not initialize function '" << name <<
      "' (argument name = '" << colname_x<< "', result name = '" <<
      colname_fx << "') using data from file '" << file << "'."; 
		except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
	}
	tab.clear();
	x_str.resize(0);
	fx_str.resize(0);
	x_num.resize(0);
	fx_num.resize(0);
}

/*******************************************************************************
 Methods to return the argument range
*******************************************************************************/
double tblFunction::minarg() const {
  if (x.size() == 0) {
		except e(__PRETTY_FUNCTION__, "Function not initialized.", __FILE__, __LINE__);
		throw(e);
	}
  return(x[0]);
}
double tblFunction::maxarg() const {
  if (x.size() == 0) {
		except e(__PRETTY_FUNCTION__, "Function not initialized.", __FILE__, __LINE__);
		throw(e);
	}
  return(x[narg-1]);
}

/*******************************************************************************
 Method to evaluate the tabulated function
*******************************************************************************/
double tblFunction::eval(const double arg) {
  const double UNITY= 1.0;
  double weight;
  size_type i;
  if (x.size() == 0) {
		except e(__PRETTY_FUNCTION__, "Function not initialized.", __FILE__, __LINE__);
		throw(e);
	}
  // Check range
  if ((arg < x[0]) || (arg > x[narg-1])) {
    stringstream errmsg;
    errmsg << "Unable to evaluate function '" << name << "' for argument '" <<
      arg << "'. Expecting argument in range [" << x[0] << ", " << x[narg-1] <<
      "].";  
	  except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
	  throw(e);      
  }
  if (x_equispaced) {
    // Find solution by index
		i= floor( (static_cast<double>(narg)-UNITY) * (arg-x[0])/(x[narg-1]-x[0]) );
		if (i == (narg-1)) {
		  return( fx[narg-1] );
		} else {
      weight= (x[i+1]-arg) / (x[i+1]-x[i]);
		  return( fx[i] * weight + fx[i+1] * (UNITY-weight) );
		}
  } else {
    // Find solution by search and update access index
		if (arg >= x[index_lastaccess]) {
	    // Forward search
			for (size_type i=index_lastaccess; i<(narg-1); i++) {
				if ((x[i] <= arg) && (x[i+1] >= arg)) {
          index_lastaccess= i;
				  weight= (x[i+1]-arg) / (x[i+1]-x[i]);
					return( fx[i] * weight + fx[i+1] * (UNITY-weight) );
				}
			}
		} else {
	    // Backward search (loops counts down to 1, not zero, to avoid problems
      // with unsigned int counter)
			for (size_type i=index_lastaccess; i>=1; i--) {
				if ((x[i-1] <= arg) && (x[i] >= arg)) {
          index_lastaccess= i-1;
				  weight= (x[i]-arg) / (x[i]-x[i-1]);
					return( fx[i-1] * weight + fx[i] * (UNITY-weight) );
				}
			}
		}
    // We may only arrive here if
    // - the argument vector is unsorted (sorting is done by .init)
    // - the argument for which the function is evaluated is not a valid number
    if (!isnormal(arg)) {
      stringstream errmsg;
      errmsg << "Cannot return value of function '" << name << "'." <<
        " The argument is not a valid floating point number.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
    except e(__PRETTY_FUNCTION__,"Bug in source code.",__FILE__,__LINE__);
    throw(e);
  }
}

