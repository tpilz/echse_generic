
#include "stringmanip.h"

////////////////////////////////////////////////////////////////////////////////
// Splits a string into tokens (Source: "C++ von a bis z", page 716, modified)
// Returns the number of tokens (return value) and the tokens (as a vector)
////////////////////////////////////////////////////////////////////////////////

unsigned int split(const string& str, vector<string>& tokens, const string &sepchars) {
  string::size_type start, stop;
  unsigned int ntokens= 0;
  const string WHITESPACE= " \t";
  tokens.clear();
  if (!str.length()) return(0);                            // String is empty
  // Find start position (skip blank lines)
  start= str.find_first_not_of(WHITESPACE+"\n");
  if (start == string::npos) return(0);                    // String contains nothing useful
  for (unsigned int i=0; i<str.length(); i++) {
    stop= str.find_first_of(sepchars,start);
    if (stop != string::npos) {
      if (stop > start) {
        tokens.push_back(str.substr(start, stop-start));
        ntokens++;
			}
		} else if (start < str.length()) {
      tokens.push_back(str.substr(start, str.length()-start));
      ntokens++;
      break;
    } else { 
      break;
    }
    start= str.find_first_not_of(sepchars,stop);
  }
  return(ntokens);
}

// Converts case of letters in a string
string casefold (const string str, const bool to_lower) {
  const string lower= "abcdefghijklmnopqrstuvwxyz";
  const string upper= "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  string::size_type pos;
  string result;
  result= str;
  if (to_lower) {
    for (unsigned int i=0; i<result.length(); i++) {
      pos= upper.find_first_of(result.substr(i,1));
      if (pos != string::npos) result.replace(i,1,lower.substr(pos,1));
    }
  } else {
    for (unsigned int i=0; i<result.length(); i++) {
      pos= lower.find_first_of(result.substr(i,1));
      if (pos != string::npos) result.replace(i,1,upper.substr(pos,1));
    }
  }
  return(result);
}

