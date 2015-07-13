#include<iostream>
#include<string>
#include<vector>
#include "../namedArray.h"
using namespace std;

int main() {

  vector<string> keys;
  keys.push_back("dresden");
  keys.push_back("poseritz");
  keys.push_back("chemnitz");
  keys.push_back("pirna");
  keys.push_back("hamburg");
  keys.push_back("potsdam");
  keys.push_back("posemuckel");
  keys.push_back("piesiecke");
  keys.push_back("plön");

  vector<int> vals;
  vals.push_back(0351);
  vals.push_back(03307);
  vals.push_back(0371);
  vals.push_back(03501);
  vals.push_back(040);
  vals.push_back(0331);
  vals.push_back(034531);
  vals.push_back(031231);
  vals.push_back(033021);

  try {

    namedArray<int> a;
    a.fill(keys, vals);

    cout << "Size: " << a.size() << endl;

    for (unsigned int i=0; i<a.size(); i++) {
      cout << i << ": " << a[i] << endl;
    }

    for (unsigned int i=0; i<keys.size(); i++) {
      cout << keys[i] << ": " << a[keys[i]] << endl;
    }

	} catch (except) {
		except e ("main", "Failed.", __FILE__, __LINE__);
    e.print();
    return(1);
  }
  return(0);
}

