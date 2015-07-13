
#include "arrays.h"

bool any (const vector<bool> &v) {
  for (unsigned int i=0; i<v.size(); i++) {
    if (v[i]) return(true);
  }
  return(false);
}
bool all (const vector<bool> &v) {
  for (unsigned int i=0; i<v.size(); i++) {
    if (!v[i]) return(false);
  }
  return(true);
}
bool any_not (const vector<bool> &v) {
  for (unsigned int i=0; i<v.size(); i++) {
    if (!v[i]) return(true);
  }
  return(false);
}
bool all_not (const vector<bool> &v) {
  for (unsigned int i=0; i<v.size(); i++) {
    if (v[i]) return(false);
  }
  return(true);
}

