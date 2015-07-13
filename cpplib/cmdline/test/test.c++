#include <iostream>

#include "../../except/except.h"
#include "../cmdline.h"

using namespace std;

int main (const int argc, const char* argv[]) {
	try  {
		cmdline cmdargs(argc, argv);
		cout << "arg1= " << cmdargs.get_value("arg1","=") << endl;
		cout << "arg2= " << cmdargs.get_value("arg2","=") << endl;

    const cmdline::T_map args= cmdargs.export_pairs("=");
    cout << args.size() << endl;
    for (cmdline::T_map::const_iterator iter=args.begin(); iter != args.end(); iter++) {
      cout << iter->first << " --> " << iter->second << endl;
    }
	} catch (except) {
		except e("main","Failed.",__FILE__,__LINE__);
    e.print();
		return(1);
	}
  return(0);
}


