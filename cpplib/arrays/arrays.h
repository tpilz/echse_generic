
#ifndef ARRAYS_H
#define ARRAYS_H

#include <vector>

using namespace std;

// Methods for logical arrays
bool any (const vector<bool> &v);
bool all (const vector<bool> &v);
bool any_not (const vector<bool> &v);
bool all_not (const vector<bool> &v);


// Any and all methods

////////////////////////////////////////////////////////////////////////////////
/// \fn void unique_elements (const T &in, T &out)
/// \brief Extracts the unique elements of a vector or valarray.
///
/// \param [in] in
///   Vector or valarray of any type (see remarks).
/// \param [in] out
///   Array containing the unique elements of \p in (duplicate values removed).
///   If \p in is empty, the returned array \p out has zero size too.
/// 
/// \return
///   \c void
///
/// \remarks
/// \li The function is designed to handle input arrays of both types vector and
///     valarray.
/// \li The comparison operator \c == must be defined for the elements of \p x.
/// \li The values in the result array \out are not sorted.
///
/// \par Example:
/// \code
///   #include<vector>
///   #include<iostream>
///   using namespace std;
///   int main() {
///     vector<int> a, b;
///     for (int i=1; i<=3; i++) a.push_back(i);
///     for (int i=2; i<=4; i++) a.push_back(i);
///     cout << endl << "a:" << endl;
///     for (unsigned int i=0; i<a.size(); i++) cout << a[i] << " ";
///     unique_elements(a, b);
///     cout << endl << "b:" << endl;
///     for (unsigned int i=0; i<b.size(); i++) cout << b[i] << " ";
///     cout << endl;
///   }
/// \endcode
///
////////////////////////////////////////////////////////////////////////////////

template <class T>
void unique_elements (const T &in, T &out) {
  if (in.size() == 0) {
    out.resize(0);
  } else {
    T temp;
    unsigned int n;
    bool dupl;
		temp.resize(in.size());
    temp[0]= in[0];
    n= 1;
	  for (unsigned int i=0; i<in.size(); i++) {
		  dupl= false;
      for (unsigned int k=0; k<n; k++) {
        if (in[i] == temp[k]) {
					dupl= true;
          break;
				}
		  }
      if (!dupl) {
        n++;
        temp[n-1]= in[i];
			}
		}
    out.resize(n);
    for (unsigned int i=0; i<n; i++) out[i]= temp[i];
    temp.resize(0);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// \fn bool has_duplicates (const T &in)
/// \brief Tests for duplicate (non-unique) elements in a vector or valarray.
///
/// \param [in] in
///   Vector or valarray of any type (see remarks).
/// 
/// \return
///   \c true if duplicate values occur in the input.
///
/// \remarks
/// \li The function is designed to handle input arrays of both types vector and
///     valarray.
/// \li The comparison operator \c == must be defined for the elements of \p x.
///
////////////////////////////////////////////////////////////////////////////////

template <class T>
bool has_duplicates (const T &in) {
  if (in.size() <= 1) {
    return(false);
  } else {
	  for (unsigned int i=1; i<in.size(); i++) {
      for (unsigned int k=0; k<i; k++) {
        if (in[k] == in[i]) {
					return(true);
				}
		  }
		}
    return(false);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// \fn void merge (const T &in1, const T &in2, T &out)
/// \brief Merges two vectors or valarrays.
///
/// \param [in] in1
///   Vector or valarray of any type.
/// \param [in] in2
///   Vector or valarray of the same type as \p in1.
/// \param [in] out
///   Array with the values of \p in2 appended to \p in1.
///   If both \p in1 and \p in2 are empty, \p out has zero size too.
/// 
/// \return
///   \c void
///
/// \remarks
/// \li The function is designed to handle input arrays of both types vector and
/// \li valarray.
/// \li The values in the result array \out are not sorted.
///
/// \par Example:
/// \code
///    using namespace std;
///    int main() {
///      vector<int> a, b, c;
///      for (int i=1; i<=3; i++) a.push_back(i);
///      for (int i=2; i<=4; i++) b.push_back(i);
///      cout << endl << "a:" << endl;
///      for (unsigned int i=0; i<a.size(); i++) cout << a[i] << " ";
///      cout << endl << "b:" << endl;
///      for (unsigned int i=0; i<b.size(); i++) cout << b[i] << " ";
///      merge(a, b, c);
///      cout << endl << "c:" << endl;
///      for (unsigned int i=0; i<c.size(); i++) cout << c[i] << " ";
///      cout << endl;
///    }
/// \endcode
///
////////////////////////////////////////////////////////////////////////////////

template <class T>
void merge (const T &in1, const T &in2, T &out) {
  unsigned int n1= in1.size();
  unsigned int n2= in2.size();
  out.resize(n1 + n2);
  for (unsigned int i=0; i<n1; i++) out[i]= in1[i];  
  for (unsigned int i=0; i<n2; i++) out[i+n1]= in2[i];  
}

#endif

