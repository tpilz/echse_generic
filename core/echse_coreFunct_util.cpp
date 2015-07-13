
#include "echse_coreFunct_util.h"

// Returns the index of the appropriate element of 'percent'
unsigned int progressIndex(
  const unsigned int iStep,
  const unsigned int nSteps,
  const vector<unsigned int> percent
){
  unsigned int result;
  double percDone_now, percDone_old;
  result= percent.size();
  percDone_now= (100.*(iStep-1))/nSteps;
  percDone_old= (100.*(iStep-2))/nSteps;
  for (unsigned int i=0; i<percent.size(); i++) {
    if ((percDone_now >= static_cast<double>(percent[i])) &
        (percDone_old < static_cast<double>(percent[i]))) {
      result= i;
      break;
    }
  }
  return(result);
}

