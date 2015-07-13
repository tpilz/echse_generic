
#include <string>
#include <fstream>

using namespace std;

// Check existance of a file by trying to open it for reading
bool file_exists(const string filename) {
  ifstream ifile;
  ifile.open(filename.c_str());
  if (ifile.is_open()) {
    ifile.close();
    return(true);
  } else {
    return(false);
  }
}

