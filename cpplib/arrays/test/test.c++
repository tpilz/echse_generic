   #include<vector>
   #include<iostream>

template <class T>
void merge (const T &in1, const T &in2, T &out) {
  unsigned int n1= in1.size();
  unsigned int n2= in2.size();
  out.resize(n1 + n2);
  for (unsigned int i=0; i<n1; i++) out[i]= in1[i];  
  for (unsigned int i=0; i<n2; i++) out[i+n1]= in2[i];  
}

   using namespace std;
   int main() {
     vector<int> a, b, c;
     for (int i=1; i<=3; i++) a.push_back(i);
     for (int i=2; i<=4; i++) b.push_back(i);
     cout << endl << "a:" << endl;
     for (unsigned int i=0; i<a.size(); i++) cout << a[i] << " ";
     cout << endl << "b:" << endl;
     for (unsigned int i=0; i<b.size(); i++) cout << b[i] << " ";
     merge(a, b, c);
     cout << endl << "c:" << endl;
     for (unsigned int i=0; i<c.size(); i++) cout << c[i] << " ";
     cout << endl;
   }


