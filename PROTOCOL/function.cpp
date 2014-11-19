#include <iostream>
//#include <string>
#include <list>

using namespace std;

void hello(string &name) {
  cout << "Hello " << name << endl;
}

void counter(int &count) {
  cout << "Current val: " << count++ << endl;
}

struct mantel {
  void (*f)(string&);
  string param;
};

int main() {
  list<mantel> functions;

  string fritz = "Fritz";
  string hans  = "Hans";

  int count = 1;

  mantel m1;
  m1.f = &hello;
  m1.param = fritz;

  functions.push_front(m1);

  mantel m2;
  m2.f = &hello;
  m2.param = hans;
  
  functions.push_front(m2);


  for ( list<mantel>::iterator it = functions.begin(); it != functions.end(); ++it )
    (*(*it).f)((*it).param);


  counter(count);
  counter(count);

  return 0;
}
