#include<iostream>
#include<sstream>
#include<string>
#include<vector>

#include "../matrix.h"
#include "../../except/except.h"

using namespace std;

template <class T>
void m_show(const matrix<T> &m) {
  cout << "Matrix with " << m.nrows() << " rows x " << m.ncols() << " cols" << endl;
  for (unsigned int irow=1; irow <= m.nrows(); irow++) {
    for (unsigned int icol=1; icol <= m.ncols(); icol++) {
      cout << m.cell(irow,icol,true) << "\t";
    }
    cout << endl;
  }
}

template <class T>
void v_show(const vector<T> &v) {
  cout << "Vector with " << v.size() << " elements" << endl;
  for (unsigned int i=0; i<v.size(); i++) {
    cout << v[i] << "\t";
  }
    cout << endl;
}

int main() {

try {

  vector<string> chars;
  chars.push_back("a");
  chars.push_back("b");
  chars.push_back("c");
  chars.push_back("d");
  chars.push_back("e");
  chars.push_back("f");
  chars.push_back("g");
  chars.push_back("h");
  chars.push_back("i");
  chars.push_back("j");
  chars.push_back("k");
  chars.push_back("l");

  string cell;
  vector<string> vect;
  unsigned int r,c;



  cout << "Init with constant and modify data..." << endl;
  matrix<int> m_int(4,6,99);
  vector<int> int_vect;
  c=2;
  r=3;
  int_vect.resize(6);
  for (unsigned int i=0; i<int_vect.size(); i++) int_vect[i]= 33;
  m_int.set_row(r,true,int_vect);
  m_show(m_int);
  m_int.set_row(r,false,int_vect);
  m_show(m_int);
  int_vect.resize(4);
  for (unsigned int i=0; i<int_vect.size(); i++) int_vect[i]= 11;
  m_int.set_col(c,true,int_vect);
  m_show(m_int);
  m_int.set_col(c,false,int_vect);
  m_show(m_int);
  m_int.set_cell(r,c,true,77);
  m_int.set_cell(r,c,false,88);
  m_show(m_int);



  matrix<string> m(2,6,true,chars);
  m_show(m);

  m.set(4,3,false,chars);
  m_show(m);

  cout << "A cell..." << endl;
  r= 3;
  c= 2;
  cout << "r=" << r << " c=" << c << " base 1 --> " << m.cell(r,c,true) << endl;
  cout << "r=" << r << " c=" << c << " base 0 --> " << m.cell(r,c,false) << endl;

  cout << "Row copies..." << endl;
  m.row_copy(r,true,vect);
  cout << "r=" << r << " base 1 --> ";
  v_show(vect);
  m.row_copy(r,false,vect);
  cout << "r=" << r << " base 0 --> ";
  v_show(vect);

  cout << "Col copies..." << endl;
  m.col_copy(c,true,vect);
  cout << "c=" << c << " base 1 --> ";
  v_show(vect);
  m.col_copy(c,false,vect);
  cout << "c=" << c << " base 0 --> ";
  v_show(vect);

  m.clear();

} catch (except) {
  except e("main","Failed.",__FILE__,__LINE__);
  e.print();
}

return(0);
}

