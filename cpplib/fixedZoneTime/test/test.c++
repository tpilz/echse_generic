#include<iostream>
#include<sstream>
#include<string>

#include "../fixedZoneTime.h"

using namespace std;

int main() {

  try {

    const double now= 1288895100.;
    const double y100= 3162240000.;
    const double incr= 900.;

    double u, u2;
    int Y, M, D, h, m, s;
    fixedZoneTime t;

    u= now-y100;
    while (u <= now+y100) {
      u= u + incr;
      t.set(u);
      t.get(Y,M,D,h,m,s);
      t.set(Y,M,D,h,m,s);
      u2= t.get();
      if (u != u2) {
        stringstream errmsg;
        errmsg << "At " << u << " ( " << Y << "-" << M << "-" << D << "T" << h << ":" << m << ":" << s << ")." << endl;
        except e("main",errmsg,__FILE__,__LINE__);
        throw(e);
      }
      cout << "OK:" << Y << "-" << M << "-" << D << "T" << h << ":" << m << ":" << s << ")." << endl;
    }

	} catch (except) {
		except e ("main", "Failed.", __FILE__, __LINE__);
    e.print();
    return(1);
  }
  return(0);
}

