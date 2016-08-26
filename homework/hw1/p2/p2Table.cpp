

#include "p2Table.h"


using namespace std;

// Implement member functions of class Row and Table here

bool
Table::read(const string& csvFile)
{
	if(csvFile.length()-csvFile.find(".csv", 0) == 4)
  		return true;
  	else return false;
}


