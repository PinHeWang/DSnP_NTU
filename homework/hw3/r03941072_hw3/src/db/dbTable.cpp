/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include <unordered_set>
#include "dbTable.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
  for(int i=0; i<r.size(); i++) { 
    if (r[i] == 0) os << '.';
    else os << r[i];
    if(i != (r.size()-1)) os << ' ';
    else break;
  }
  
  return os;
  
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells should be left blank (printed as ' ').
  
  for(int i=0; i<t.nRows(); i++) {
    os << setw(6);
    for(int j=0; j<t.nCols(); j++) {
      if(t[i][j] == INT_MAX) os << '.';
      else os << t[i][j];
      if(j != t.nCols()-1) os << setw(6); // NOT setw(6) at the end of the ROW
    }
    if(i == t.nRows()-1) break;
    os << endl;

  }
  return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
  // TODO: to read in data from csv file and store them in a table
  // - You can assume all the data of the table are in a single line.
  
  string str, row;
  getline(ifs, str);
  while (str.size()) {
    size_t posRowEnd = str.find_first_of(char(13), 0);
    row = str.substr(0, posRowEnd);
    str = str.substr(posRowEnd + 1);
    if (str == "") return ifs;
    row.push_back(',');
    DBRow dbr;
    while (row.size()) {
      string data;
      int num;
      size_t begin = 0;
      size_t end = row.find_first_of(',', begin);
      data = row.substr(begin, end - begin);
      row = row.substr(end + 1);
      (myStr2Int(data, num))? dbr.addData(num):dbr.addData(INT_MAX);
    }
    t.addRow(dbr);
    if (str[0] == char(13)) return ifs;
  }
  return ifs;

  }



/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
  // TODO
  if(c > _data.size() || c < 0){
    cout << "Illegal index!" << endl;
  }else _data.erase(_data.begin() + c); // remove c+1 row
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
  for(int i=0; i<_sortOrder.size(); i++) {
    if(r1[_sortOrder[i]] == r2[_sortOrder[i]]) continue;
    else return false;
  }
  return true;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
  // TODO
  _table.clear();
}

void
DBTable::addCol(const vector<int>& d)
{
  // TODO: add a column to the right of the table. Data are in 'd'.
  if(d.size() != nRows()) {cout << "Illegal size!" << endl;}
  else {
    for(int i=0; i<nRows(); i++) 
      _table[i].addData(d[i]);
  }
}

void
DBTable::delRow(int c)
{
  // TODO: delete row #c. Note #0 is the first row.
  if(c > nRows() || c < 0){
    cout << "Illegal index!" << endl;
  }else _table.erase(_table.begin() + c); // remove c+1 row

}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
  if(c > nCols() || c < 0) {
    cout << "Illegal index!" << endl;
  }else{
    for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
  }
}


// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
  // TODO: get the max data in column #c
  if(c > nCols() || c < 0) {
    cout << "Illegal index!" << endl;
    return 0.0;
  }else {
    float max = _table[0][c];
    for(int i=1; i<nRows(); i++)
      if(_table[i][c] > max && _table[i][c] != INT_MAX) max = _table[i][c];
    return max;
  }
}

float
DBTable::getMin(size_t c) const
{
  // TODO: get the min data in column #c
  if(c > nCols() || c < 0) {
    cout << "Illegal index!" << endl;
    return 0.0;
  }else {
    float min = _table[0][c];
    for(int i=1; i<nRows(); i++)
      if(_table[i][c] < min && _table[i][c] != INT_MAX) min = _table[i][c];
    return min;
  }
}

float 
DBTable::getSum(size_t c) const
{
  // TODO: compute the sum of data in column #c
  if(c > nCols() || c < 0) {
    cout << "Illegal index!" << endl;
    return 0.0;
  }else {
    float sum = 0.0;
    for(int i=0; i<nRows(); i++) {
      if(_table[i][c] == INT_MAX){}
      else {sum += _table[i][c];}
    }
    return sum;
  }
}

int
DBTable::getCount(size_t c) const
{
  // TODO: compute the number of distinct data in column #c
  // - Ignore null cells
  if(c > nCols() || c < 0) {
    cout << "Illegal index!" << endl;
    return 0;
  }else {
    int myCol[nRows()];
    int count = 0;
    for(int i=0; i<nRows(); i++) {
      myCol[i] = getData(i,c);
      if(getData(i,c) == INT_MAX) count += 1;
    }
    
    const size_t len = sizeof(myCol) / sizeof(myCol[0]);
    unordered_set<int> s(myCol, myCol+len);
    return s.size() - count;
  }
}



float
DBTable::getAve(size_t c) const
{
  // TODO: compute the average of data in column #c
  if(c > nCols()){
    cout << "Illegal index!";
    return 0.0;
  }else {
    int count = 0;
    for(int i=0; i<nRows(); i++) {
      if(_table[i][c] == INT_MAX) {}
      else count += 1; 
    }
    float ave = getSum(c) / count;

    return ave;
  }
}

void
DBTable::sort(const struct DBSort& s)
{
  // TODO: sort the data according to the order of columns in 's'
  ::sort(_table.begin(), _table.end(), s);

}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
  if(c > nCols()){
    cout << "Illegal index!" << endl;
  }else{
    for(int i=0; i<nRows(); i++) {
      if(_table[i][c] == INT_MAX) cout << '.';
      else cout << _table[i][c];
    cout << ' ';
    }
  }
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

