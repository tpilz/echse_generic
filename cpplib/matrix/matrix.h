
#ifndef MATRIX_H
#define MATRIX_H

#include <sstream>
#include <vector>

#include "../except/except.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Matrix class
////////////////////////////////////////////////////////////////////////////////

// Conventions:
//
// (1) The data are stored in a vector by row (see example of a 3 x 4 matrix).
//
// +------------+
// | 0  1  2  3 |
// | 4  5  6  7 |
// | 8  9 10 11 |
// +------------+
//
// (2) When accessing a cell: Row index 1st, column index 2nd


template <class T>
class matrix {
  private:
    vector<T> data;
    unsigned int NC, NR;
  public:
    // Constructors
    matrix(const unsigned int nrows, const unsigned int ncols, const T constant) {
      try {
        set(nrows,ncols,constant);
      } catch (except) {
        except e(__PRETTY_FUNCTION__,"Error in matrix constructor.",__FILE__,__LINE__);
        throw(e);
      }
    }
    matrix(const unsigned int nrows, const unsigned int ncols,
      const bool by_row, const vector<T> values) {
      try {
        set(nrows,ncols,by_row,values);
      } catch (except) {
        except e(__PRETTY_FUNCTION__,"Error in matrix constructor.",__FILE__,__LINE__);
        throw(e);
      }
    }
    // Destructor
    ~matrix() {
       clear();
    };
    // Clear method
    void clear() {
      data.clear();
      NC=0;
      NR=0;
    }
    // Set methods
    void set(const unsigned int nrows, const unsigned int ncols, const T constant) {
      NC= ncols;
      NR= nrows;
      data.resize(NC * NR);
      if (data.size() == 0) {
        stringstream errmsg;
        errmsg << "Bad matrix dimensions (" << NR << " rows x " << NC << " columns).";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      for (unsigned int i=0; i<data.size(); i++) data[i]= constant;
    }
    void set(const unsigned int nrows, const unsigned int ncols,
      const bool by_row, const vector<T> values) {
      NC= ncols;
      NR= nrows;
      data.resize(NC * NR);
      if (data.size() == 0) {
        stringstream errmsg;
        errmsg << "Bad matrix dimensions (" << NR << " rows x " << NC << " columns).";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      if (data.size() != values.size()) {
        stringstream errmsg;
        errmsg << "Matrix dimensions (" << NR << " rows x " << NC <<
          " columns) do not match with number of values (" << values.size() << ").";
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      if (by_row) {
        for (unsigned int i=0; i<data.size(); i++) {
          data[i]= values[i];
        }
      } else {
        for (unsigned int c=0; c<NC; c++) {
          for (unsigned int r=0; r<NR; r++) {
            data[r*NC+c]= values[c*NR+r];
          }
        }
      }
    }
    // Querying of dimensions
    unsigned int ncols() const {return(NC);}
    unsigned int nrows() const {return(NR);}
    // Return (a copy of) a single element
    T cell(const unsigned int row, const unsigned int col, const bool base_1) const {
      if (base_1) {
        if ((row < 1) | (row > NR) | (col < 1) | (col > NC)) {
          stringstream errmsg;
          errmsg << "Matrix indices (row " << row << " col " << col <<
            ") out of range. Row index must be in [1:" << NR <<
            "] and column index must be in [1," << NC << "] for this matrix." <<
            " if 'base_1' == true."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        return(data[(row-1)*NC+col-1]);
      } else {
        if ((row > (NR-1)) | (col > (NC-1))) {
          stringstream errmsg;
          errmsg << "Matrix indices (row " << row << " col " << col <<
            ") out of range. Row index must be in [0:" << (NR-1) <<
            "] and column index must be in [0," << (NC-1) << "] for this matrix." <<
            " if 'base_1' == false."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        return(data[row*NC+col]);
      }
    }
    // Export copy of a row
    void row_copy(const unsigned int row, const bool base_1, vector<T> &values) const {
      if (base_1) {
        if ((row < 1) | (row > NR)) {
          stringstream errmsg;
          errmsg << "Row index " << row << " out of range. Must be in [1:" <<
            NR << "] for this matrix if 'base_1' == true."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        values.resize(NC);
        for (unsigned int i=0; i<NC; i++) values[i]= data[(row-1)*NC+i];
      } else {
        if (row > (NR-1)) {
          stringstream errmsg;
          errmsg << "Row index " << row << " out of range. Must be in [0:" <<
            (NR-1) << "] for this matrix if 'base_1' == false."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        values.resize(NC);
        for (unsigned int i=0; i<NC; i++) values[i]= data[row*NC+i];
      }
    }
    // Export copy of a column
    void col_copy(const unsigned int col, const bool base_1, vector<T> &values) const {
      if (base_1) {
        if ((col < 1) | (col > NC)) {
          stringstream errmsg;
          errmsg << "Column index " << col << " out of range. Must be in [1:" <<
            NC << "] for this matrix if 'base_1' == true."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        values.resize(NR);
        for (unsigned int i=0; i<NR; i++) values[i]= data[(col-1)+i*NC];
      } else {
        if (col > (NC-1)) {
          stringstream errmsg;
          errmsg << "Column index " << col << " out of range. Must be in [0:" <<
            (NC-1) << "] for this matrix if 'base_1' == false."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        values.resize(NR);
        for (unsigned int i=0; i<NR; i++) values[i]= data[col+i*NC];
      }
    }
    // Set a single element
    void set_cell(const unsigned int row, const unsigned int col, const bool base_1, const T value) {
      if (base_1) {
        if ((row < 1) | (row > NR) | (col < 1) | (col > NC)) {
          stringstream errmsg;
          errmsg << "Matrix indices (row " << row << " col " << col <<
            ") out of range. Row index must be in [1:" << NR <<
            "] and column index must be in [1," << NC << "] for this matrix." <<
            " if 'base_1' == true."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        data[(row-1)*NC+col-1]= value;
      } else {
        if ((row > (NR-1)) | (col > (NC-1))) {
          stringstream errmsg;
          errmsg << "Matrix indices (row " << row << " col " << col <<
            ") out of range. Row index must be in [0:" << (NR-1) <<
            "] and column index must be in [0," << (NC-1) << "] for this matrix." <<
            " if 'base_1' == false."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        data[row*NC+col]= value;
      }
    }
    // Set a whole row
    void set_row(const unsigned int row, const bool base_1, const vector<T> values) {
      if (values.size() != NC) {
        stringstream errmsg;
        errmsg << "Need " << NC << " values to fill a row but vector has " <<
          values.size() << " elements."; 
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      if (base_1) {
        if ((row < 1) | (row > NR)) {
          stringstream errmsg;
          errmsg << "Row index " << row << " out of range. Must be in [1:" <<
            NR << "] for this matrix if 'base_1' == true."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        for (unsigned int i=0; i<NC; i++) data[(row-1)*NC+i]= values[i];
      } else {
        if (row > (NR-1)) {
          stringstream errmsg;
          errmsg << "Row index " << row << " out of range. Must be in [0:" <<
            (NR-1) << "] for this matrix if 'base_1' == false."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        for (unsigned int i=0; i<NC; i++) data[row*NC+i]= values[i];
      }
    }
    // Set a whole column
    void set_col(const unsigned int col, const bool base_1, const vector<T> values) {
      if (values.size() != NR) {
        stringstream errmsg;
        errmsg << "Need " << NR << " values to fill a column but vector has " <<
          values.size() << " elements."; 
        except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
        throw(e);
      }
      if (base_1) {
        if ((col < 1) | (col > NC)) {
          stringstream errmsg;
          errmsg << "Column index " << col << " out of range. Must be in [1:" <<
            NC << "] for this matrix if 'base_1' == true."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        for (unsigned int i=0; i<NR; i++) data[(col-1)+i*NC]= values[i];
      } else {
        if (col > (NC-1)) {
          stringstream errmsg;
          errmsg << "Column index " << col << " out of range. Must be in [0:" <<
            (NC-1) << "] for this matrix if 'base_1' == false."; 
          except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
          throw(e);
        }
        for (unsigned int i=0; i<NR; i++) data[col+i*NC]= values[i];
      }
    }
};

#endif

