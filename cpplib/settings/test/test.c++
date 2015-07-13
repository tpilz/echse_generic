#include<iostream>
#include<string>
#include<vector>
#include "../settings.h"

#define _GNU_SOURCE
#include <cfenv>

using namespace std;



int main() {

  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);

  const string file="/home/dkneis/cpp/dklib/settings/test/settings.txt";
  const string chars_colsep="=";
  const string chars_comment="#";

  try {

    settings s(file, chars_colsep, chars_comment);
    cout << s["dresden"] << endl;
    cout << s["poseritz"] << endl;


	} catch (except) {
		except e ("main", "Failed.", __FILE__, __LINE__);
    e.print();
    return(1);
  }
  return(0);
}

