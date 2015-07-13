
#include "table.h"

/*******************************************************************************
Constructor
*******************************************************************************/
table::table() {
  numcols= 0;
  numrows= 0;
}

/*******************************************************************************
Delete table data
*******************************************************************************/
void table::clear() {
  data.clear();
  colnames.clear();
  searchmap.clear();
  numrows= 0;
  numcols= 0;
  index_searchcolumn= 0;
}

/*******************************************************************************
Destructor
*******************************************************************************/
table::~table() {
  clear();
}

/*******************************************************************************
Query table dimensions
*******************************************************************************/
bool table::empty() const {
  if ((numrows * numcols) == 0) {
    return(true);
  } else {
    return(false);
  }
}
table::size_type table::nrow() const { return (numrows); }
table::size_type table::ncol() const { return (numcols); }

/*******************************************************************************
Determine column index from column name
*******************************************************************************/
table::size_type table::colindex (const string &colname) const {
  size_type index_col;
  if (table::empty()) {
    except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
    throw(e);
  }
  index_col= 0;
  for (size_type i=0; i<colnames.size(); i++) {
    if (colnames[i] == colname) {
  	  index_col= i+1;
      break;
    }
	}
  if (index_col==0) {
    stringstream errmsg;
	  errmsg << "Column with name '" << colname << "' does not exist in table.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);    
  }
  return(index_col);
}

/*******************************************************************************
Return a single table element (lower bound for indices is 1!)
*******************************************************************************/

// Using the element's col & row index
string table::get_element(const size_type index_row, const size_type index_col) const {
  if (table::empty()) {
    except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
    throw(e);
  }
  if ((index_row < 1) || (index_row > numrows)) {
    stringstream errmsg;
	  errmsg << "Row index (" << index_row << ") out of range." <<
	    " Index must be in range [1," << numrows << "] for this table.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  if ((index_col < 1) || (index_col > numcols))  {
    stringstream errmsg;
	  errmsg << "Column index (" << index_col << ") out of range." <<
	    " Index must be in range [1," << numcols << "] for this table.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  return( data[(index_row-1)*numcols + index_col - 1] );
}

/*******************************************************************************
Return a row/column of the table by index (lower bound for row index is 1!)
*******************************************************************************/
void table::get_row(const size_type index, vector<string> &strvect) const {
  if ((index < 1) || (index > numrows)) {
    if (table::empty()) {
		  except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
		  throw(e);
    } else {
      stringstream errmsg;
		  errmsg << "Row index (" << index << ") out of range." <<
		    " Index must be in range [1," << numrows << "] for this table.";
		  except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		  throw(e);
    }
  }
  strvect.resize(numcols);
  for (size_type i=0; i<numcols; i++) {
    strvect[i]= data[(index-1)*numcols + i];
  }
}

void table::get_col(const size_type index, vector<string> &strvect) const {
  if ((index < 1) || (index > numcols))  {
    if (table::empty()) {
		  except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
		  throw(e);
    } else {
      stringstream errmsg;
		  errmsg << "Column index (" << index << ") out of range." <<
		    " Index must be in range [1," << numcols << "] for this table.";
		  except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		  throw(e);
    }
  }
  strvect.resize(numrows);
  for (size_type i=0; i<numrows; i++) {
    strvect[i]= data[(index-1) + i*numcols];
  }
}

/*******************************************************************************
Select a column as the search column (puts row indices of elements in multimap).
*******************************************************************************/
void table::update_searchcolumn(const size_type index_col) {
  if ((index_col < 1) || (index_col > numcols))  {
    stringstream errmsg;
	  errmsg << "Cannot select column with index " << index_col <<
     " as search column." << " Index must be in range [1," << numcols <<
     "] for this table.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  index_searchcolumn= index_col;
  searchmap.clear();
  for (unsigned int i=0; i<numrows; i++) {
    searchmap.insert(T_searchmap_pair(data[(index_searchcolumn-1)+(i*numcols)],
      (i+1))); // Let row indices start at 1 !
  }
}

/*******************************************************************************
Find first or all matching row(s) based on search in a column.
*******************************************************************************/

// The function returns the index of the FIRST (!) row matching the search criterion.
// Indices start at 1. If there is no matching row, the returned value is 0.
// Note: Update of the seach column is not done inside this method to keep it
//       a const method.
table::size_type table::first_row(const size_type index_col_filter,
  const string &searchstring) const {
  if (table::empty()) {
    except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
    throw(e);
  }
  // Check index of search column (also ensures that index is not out of range)
  if (index_col_filter != index_searchcolumn) {
    stringstream errmsg;
    errmsg << "Selected filter column (index " << index_col_filter <<
      ") is not the currently active search column (index " <<
      index_searchcolumn << "). Update the latter before calling this method.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Find index of first matching row
  pair<T_searchmap::const_iterator, T_searchmap::const_iterator> pos=
    searchmap.equal_range(searchstring);
  if (distance(pos.first,pos.second) > 0) {  // One or more matches found
    return(pos.first->second);               // Return row index of first match
  } else {
    return(0);                               // Return 0 if no match was found
  }
}

// The function returns the index of all rows meeting the search criterion
// The array of row indices is passed by reference (indices start at 1 !).
// If no matches were found, the returned vector of indices is empty.
// Note: Update of the seach column is not done inside this method to keep it
//       a const method.
void table::which_rows(const size_type index_col_filter,
  const string &searchstring, vector<size_type> &indices) const
{
  if (table::empty()) {
    except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
    throw(e);
  }
  // Check index of search column (also ensures that index is not out of range)
  if (index_col_filter != index_searchcolumn) {
    stringstream errmsg;
    errmsg << "Selected filter column (index " << index_col_filter <<
      ") is not the currently active search column (index " <<
      index_searchcolumn << "). Update the latter before calling this method.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Find row index by search in the filter column
  indices.clear();
  pair<T_searchmap::const_iterator, T_searchmap::const_iterator> pos=
    searchmap.equal_range(searchstring);
  if (distance(pos.first,pos.second) > 0) {  // One or more matches found 
    for (T_searchmap::const_iterator iter=pos.first; iter!=pos.second; iter++) {
      indices.push_back(iter->second);
    }
  }
}

/*******************************************************************************
Subset a table by rows using an array of target row indices
*******************************************************************************/
void table::subset(const vector<size_type> &rowindices, table &tab) const {
  if (table::empty()) {
    except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
    throw(e);
	}
  if (rowindices.size() == 0) {
    except e(__PRETTY_FUNCTION__,"Vector of row indices is empty.",__FILE__,__LINE__);
    throw(e);
  }
  if (rowindices.size() > numrows) {
    stringstream errmsg;
    errmsg << "Length of input vector (" << rowindices.size() << ") must not" <<
      " exceed number of rows in table (" << numrows << ").";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  tab.clear();
  tab.numcols= numcols;
  tab.numrows= rowindices.size();
  tab.colnames.resize(numcols);
  tab.data.resize(numcols*rowindices.size());
  // Set colnames
  for (size_type i=0; i<numcols; i++)
    tab.colnames[i]= colnames[i];
  // Set data
  for (size_type i=0; i<rowindices.size(); i++) {
		if ((rowindices[i] < 1) || (rowindices[i] > numrows)) {
      stringstream errmsg;
		  errmsg << "Row index (" << rowindices[i] << ") out of range." <<
		    " Index must be in range [1," << numrows << "] for this table.";
		  except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		  throw(e);
		}
    for (size_type k=0; k<numcols; k++) {
      tab.data[i*numcols+k]= data[(rowindices[i]-1)*numcols+k];
    }
	}
  // Update search column in the new table (keep column index unchanged)
  try {
    tab.update_searchcolumn(index_searchcolumn);
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Update of search column failed.",__FILE__,__LINE__);
    throw(e);
  }
}

/*******************************************************************************
Insert a row
*******************************************************************************/
void table::insert_row(const vector<string> &values, const bool at_end) {
  if (table::empty()) {
    except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
    throw(e);
  }
  if (values.size() != numcols) {
      stringstream errmsg;
      errmsg << "Length of insert vector (" << values.size() << ") does not" <<
        " match number of columns (" << numcols << ").";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
  }
  // Insert the values
  if (at_end) {
    data.insert(data.end(), values.begin(), values.end());
  } else {
    data.insert(data.begin(), values.begin(), values.end());
  }
  // Update the table's dimension parameters
  numrows++;
  // Update search column (keep column index unchanged)
  try {
    update_searchcolumn(index_searchcolumn);
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Update of search column failed.",__FILE__,__LINE__);
    throw(e);
  }
}

/*******************************************************************************
Subset a table by columns using an array of target column indices
*******************************************************************************/
void table::subset_cols(const vector<size_type> &colindices, table &tab) const {
  if (table::empty()) {
    except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
    throw(e);
  }
  if (colindices.size() == 0) {
    except e(__PRETTY_FUNCTION__,"Vector of column indices is empty.",__FILE__,__LINE__);
    throw(e);
  }
  if (colindices.size() == numcols) {
    except e(__PRETTY_FUNCTION__,"Number of columns to drop must be less than the total number of columns.",__FILE__,__LINE__);
    throw(e);
  }
  tab.clear();
  tab.numcols= colindices.size();
  tab.numrows= numrows;
  tab.colnames.resize(colindices.size());
  tab.data.resize(colindices.size()*numrows);

  // Set colnames
  for (size_type i=0; i<colindices.size(); i++) {
    if ((colindices[i] < 1) || (colindices[i] > numcols)) {
      stringstream errmsg;
      errmsg << "Column index (" << colindices[i] << ") out of range." <<
       " Index must be in range [1," << numcols << "] for this table.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
    tab.colnames[i]= colnames[colindices[i]-1];
  }
  
  // Set data
  for (size_type i=0; i<numrows; i++) {
    for (size_type k=0; k<colindices.size(); k++) {
      // Range of colum index already checked when setting column names
      tab.data[i*colindices.size()+k]= data[i*numcols+(colindices[k]-1)];
    }
  }
  // Define column 1 as current search column
  try {
    tab.update_searchcolumn(1);
  } catch (except) {
    except e(__PRETTY_FUNCTION__,"Update of search column failed.",__FILE__,__LINE__);
    throw(e);
  }
}

/*******************************************************************************
Get one or all column names
*******************************************************************************/
string table::get_colname(const size_type index) const {
  if ((index < 1) || (index > numcols))  {
    if (table::empty()) {
      except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
      throw(e);
    } else {
		  stringstream errmsg;
		  errmsg << "Column index (" << index << ") out of range." <<
		    " Index must be in range [1," << numcols << "] for this table.";
		  except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
		  throw(e);
    }
  }
  return( colnames[index-1] );
}

void table::get_colnames(vector<string> &strvect) const {
  if (table::empty()) {
    except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
    throw(e);
  }
  strvect.resize(numcols);
  strvect= colnames;
}

/*******************************************************************************
Set a column name
*******************************************************************************/
void table::set_colname(const size_type index, const string &colname) {
  if ((index < 1) || (index > numcols))  {
    if (table::empty()) {
      except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
      throw(e);
    } else {
      stringstream errmsg;
      errmsg << "Column index (" << index << ") out of range." <<
      " Index must be in range [1," << numcols << "] for this table.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
  colnames[index-1]= colname;
}

/*******************************************************************************
Read table of strings from a text file
*******************************************************************************/
void table::read(const string &file, const bool &header, const string &chars_colsep,
  const string &chars_comment) {
  size_type n;
  size_type i;
  size_type i_first;
  string line;
  string::size_type strpos;
  vector<string> tokens;
  size_type ntokens;
  ifstream ifile;
  short has_header= static_cast<short>(header);
  // Open input file
  ifile.open(file.c_str());
  if (!ifile.is_open()) {
    stringstream errmsg;
    errmsg << "Unable to open file '" << file << "' for reading.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Determine number of rows
  n= 0;
  i= 0;
  while(!ifile.eof()) {
    i++;
    getline(ifile,line);
	  strpos= line.find_first_not_of(" \t\n");    // Find first non-blank position
	  if (strpos != string::npos) {               // Skip blank/whitespace lines
      if (line.substr(strpos,1).find_first_of(chars_comment) == string::npos) { // Skip comment lines
        n++;                                    // Count row
      }
	  }
  }
	ifile.close();
  // Save and check number of rows
  numrows= n - has_header;
  if (numrows <= 0) {
    stringstream errmsg;
    errmsg << "File '" << file << "' does not contain (usable) data.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  // Determine number of columns from first row and read matrix (checking ncols in each row)
  ifile.open(file.c_str());
  n= 0;
  i= 0;
  i_first= 0;
  while(!ifile.eof()) {
    i++;
    getline(ifile,line);
	  strpos= line.find_first_not_of(" \t\n");
	  if (strpos != string::npos) {
      if (line.substr(strpos,1).find_first_of(chars_comment) == string::npos) {
        n++;
        // Split line into tokens
        ntokens= split(line, tokens, chars_colsep);
        if (ntokens == 0) {
          stringstream errmsg;
			    errmsg << "Unable to determine number of tokens at line " <<
            i << " of file '" << file << "'.";
					except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
					throw(e);
        }
        if (n == 1) {
	        // Allocate matrix based on the number of tokens in the first row
          i_first= i;
          numcols= ntokens;
          colnames.resize(numcols);          
          data.resize(numrows*numcols);
          // Set column names from file header and check if unique
		      if (has_header) {
						for (size_type k=0; k<numcols; k++) {
				      colnames[k]= tokens[k];
							if (k > 0) {
    						for (size_type m=0; m<k; m++) {
                  if (colnames[k] == colnames[m]) {
										stringstream errmsg;
										errmsg << "Column name '" << colnames[k] <<
                      "' is not unique in file '" << file << ".";
										except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
										throw(e);
                  }
                }
              }
						}
          // Make column names if not provided
		      } else {
						for (size_type k=0; k<numcols; k++) {
				      colnames[k]= "x" + as_string(k);
						}
          }
				// Check that the number of elements in each row is consistent
        } else {
          if (ntokens != numcols) {
		        stringstream errmsg;
			      errmsg << "Number of tokens at line " << i << 
              " of file '" << file << "' is " << ntokens <<
              " but the header/top line (line " << i_first << ") has " << numcols <<
              " token(s). Assuming columns to be separated by '" << chars_colsep << "'.";
						except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
						throw(e);
          }
        }
        // Save the data of the current row in 1D array
        // Array is filled by row, i.e. the matrix
        //   "a" "b" "c"
        //   "d" "e" "f"
        // is stored as vector ("a" "b" "c" "d" "e" "f").
        // If header==TRUE, the first line is stored in the colnames array.
        if ((n > 1) || (!has_header)) {  // Ignore first row if there is a header
					for (size_type k=0; k<numcols; k++) {
		        data[(n-has_header-1)*numcols+k]= tokens[k];
					}
				}
        tokens.clear();
      }
		}
  }
	ifile.close();
  // Initialize search map (using first column)
  try {
    update_searchcolumn(1);
  } catch (except) {
		except e(__PRETTY_FUNCTION__,"Initialization of search column (1) failed.",__FILE__,__LINE__);
		throw(e);
  }
}

/*******************************************************************************
Private generic method for export to file --> used by all write methods
*******************************************************************************/
void table::export_to_file(const vector<size_type> &colindices, const vector<size_type> &rowindices,
  const string &file, const string &colsep, const bool &replace) const {

  vector<string> strvect;

  if (table::empty()) {
    except e(__PRETTY_FUNCTION__,"Table is empty.",__FILE__,__LINE__);
    throw(e);
  }
  // Check indices
  if (colindices.size() == 0) {
      stringstream errmsg;
      errmsg << "Vector of column indices is empty.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
  }
  for (size_type i=0; i<colindices.size(); i++) {
    if ((colindices[i] < 1) || (colindices[i] > numcols)) {
      stringstream errmsg;
      errmsg << "Column index (" << colindices[i] << ") out of range." <<
       " Index must be in range [1," << numcols << "] for this table.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
  if (rowindices.size() == 0) {
      stringstream errmsg;
      errmsg << "Vector of row indices is empty.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
  }
  for (size_type i=0; i<rowindices.size(); i++) {
    if ((rowindices[i] < 1) || (rowindices[i] > numrows)) {
      stringstream errmsg;
      errmsg << "Row index (" << rowindices[i] << ") out of range." <<
       " Index must be in range [1," << numrows << "] for this table.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
  }
  // Prepare file
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
  // Print header
  try {
    table::get_colnames(strvect);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Unable to return column names from source table while" <<
      " trying to create file '" << file << "'.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
  if (colindices.size() == 1) {
    ofile << strvect[colindices[0]-1] << endl;
  } else {
    for (size_type k=0; k<(colindices.size()-1); k++) {
      ofile << strvect[colindices[k]-1] << colsep;
    }    
    ofile << strvect[colindices[(colindices.size()-1)]-1] << endl;
  }
  // Print rows
  for (size_type i=0; i<rowindices.size(); i++) {
    try {
      table::get_row(rowindices[i], strvect);
    } catch (except) {
      stringstream errmsg;
      errmsg << "Unable to return row " << rowindices[i] << " from source table while" <<
        " trying to create file '" << file << "'.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }
    if (colindices.size() == 1) {
      ofile << strvect[colindices[0]-1] << endl;
    } else {
      for (size_type k=0; k<(colindices.size()-1); k++) {
	ofile << strvect[colindices[k]-1] << colsep;
      }    
      ofile << strvect[colindices[(colindices.size()-1)]-1] << endl;
    }
  }
  // Close output
  ofile.close();
}

/*******************************************************************************
Write table of strings to text file
*******************************************************************************/

// All cols and rows
void table::write(const string &file, const string &colsep, const bool &replace) const {
  vector<size_type> colindices(numcols);
  for (size_type i=0; i<numcols; i++) colindices[i]= i+1; // All column indices
  vector<size_type> rowindices(numrows);
  for (size_type i=0; i<numrows; i++) rowindices[i]= i+1; // All row indices
  try {
    export_to_file(colindices, rowindices, file, colsep, replace);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Unable to write table contents to file '" << file << "'." <<
      " Selected columns: " << "all" << " of " << numcols << "." <<
      " Selected rows: " << "all" << " of " << numrows << ".";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}
// Selected cols AND rows
void table::write(const string &file, const string &colsep, const bool &replace,
  const vector<size_type> colindices, const vector<size_type> rowindices) const {
  try {
    export_to_file(colindices, rowindices, file, colsep, replace);
  } catch (except) {
    stringstream errmsg;
    errmsg << "Unable to write table contents to file '" << file << "'." <<
      " Selected columns: " << colindices.size() << " of " << numcols << "." <<
      " Selected rows: " << rowindices.size() << " of " << numrows << ".";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}
// Selected cols and all rows OR selected rows and all cols
void table::write(const string &file, const string &colsep, const bool &replace,
  const vector<size_type> indices, const bool indices_are_columns) const {
  try {
    if (indices_are_columns) {
      vector<size_type> rowindices(numrows);
      for (size_type i=0; i<numrows; i++) rowindices[i]= i+1; // All row indices
      export_to_file(indices, rowindices, file, colsep, replace);
    } else {
      vector<size_type> colindices(numcols);
      for (size_type i=0; i<numcols; i++) colindices[i]= i+1; // All column indices
      export_to_file(colindices, indices, file, colsep, replace);
    }
  } catch (except) {
    stringstream errmsg;
    if (indices_are_columns) {    
      errmsg << "Unable to write table contents to file '" << file << "'." <<
	" Selected columns: " << indices.size() << " of " << numcols << "." <<
	" Selected rows: " << "all" << " of " << numrows << ".";
    } else {
      errmsg << "Unable to write table contents to file '" << file << "'." <<
	" Selected columns: " << "all" << " of " << numcols << "." <<
	" Selected rows: " << indices.size() << " of " << numrows << ".";
    }
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    throw(e);
  }
}




