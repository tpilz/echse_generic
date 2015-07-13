
/*

This class implements named arrays in a way that combindes the advantages of
classical vectors and maps. The elements of a named array can be accessed in
two ways:

  (1) using the numeric index  (like for STL vectors or other arrays)
  (2) using a key string       (like for STL map)

Internally, the actual data are stored in a normal STL vector.
The relation between the key strings and the data (namely the indices of the
data vector) is stored in a STL map. By use of the map, data elements can be
found by its key very fast (faster than by linear search in a vector of names).

*/


#ifndef NAMEDARRAY_H
#define NAMEDARRAY_H

#include <string>
#include <map>
#include <sstream>
#include "../except/except.h"

using namespace std;

template <class T>
class namedArray {
	public:
    typedef unsigned int size_type;
  private:
    typedef map<string, size_type> maptype;
    vector<T> data;
    maptype key_index_map;
	public:
    // Ctor and dtor
    namedArray() {
      data.clear();
      key_index_map.clear();
    }
    ~namedArray() {
      data.clear();
      key_index_map.clear();
    }
    // Manual clear
    void clear() {
      data.clear();
      key_index_map.clear();
    }
    // Size function
    size_type size() {
      return(data.size());
    }
    // Initialization method
    void fill(const vector<string> &names, const vector<T> &values) {
      // Check input lengths
      if (names.size() == 0) {
        except e(__PRETTY_FUNCTION__, "Input vectors are empty.", __FILE__, __LINE__);
        throw(e);
      }
      if (values.size() != names.size()) {
        stringstream errmsg;
        errmsg << "Length of key vector (" << names.size() << ") does not match" <<
          " length of data vector ("<< values.size() << ").";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      // Set data with check for duplicate keys
      for (size_type i=0; i<names.size(); i++) {
        typename maptype::const_iterator iter= key_index_map.find(names[i]);
        if (iter != key_index_map.end()) {
          stringstream errmsg;
          errmsg << "Multiple instances of key '" << names[i] << "' detected. Keys must be unique.";
          except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
          throw(e);
        }
        // Save data in vector
        data.push_back(values[i]);        
        // Save key and vector index
        key_index_map[names[i]]= i;
      }
    }
    // Direct access to data using the numeric array index
    const T& operator[](const size_type index) const {
      if (index >= data.size()) {
        stringstream errmsg;
        errmsg << "Cannot access data for index " << index <<
          ". Index is out of range [0," << data.size() << "].";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      return(data[index]);
    }
    // Data access using the key string (translated to index via map)
    const T& operator[](const string &key) const {
      typename maptype::const_iterator iter= key_index_map.find(key);
      if (iter == key_index_map.end()) {
        stringstream errmsg;
        errmsg << "Cannot access data for key '" << key << "'. Key is unknown.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      return(data[iter->second]);
    }
};

#endif

