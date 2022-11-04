#include <iostream>

using namespace std;

void incr(int &i, int &j) {
  i++;
  j++;
}

void incr2(int i, int j) {
  i++;
  j++;
}

int main() {
  cout << "Workbook4: question 2" << endl;
  int i = 0;
  cout << "Before call: i: " << i << endl;
  incr(i, i);
  cout << "After call: i: " << i << endl;
  return 0;
}
