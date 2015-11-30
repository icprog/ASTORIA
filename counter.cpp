#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

int main(int argc, char **argv){

  string line;
  ifstream myfile ("parsed.txt");
  size_t found1, found2;
  int counter = 0;

  if (myfile.is_open())
  {
	while ( getline (myfile,line) )
	{
		found1=line.find("01140094");
		found2=line.find("00830155");
		if (found1!=string::npos) {
			counter++;
		}
		else {
			if (found2!=string::npos){
				counter++;
			}
		}
	}
    cout << counter << endl;
    myfile.close();
  }
  else cout << "Unable to open file";

  return 0;

}
