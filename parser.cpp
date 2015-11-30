#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

int main(int argc, char **argv){

  string line1, line2;
  ifstream myfile1 ("sizedebug.txt");
  ifstream myfile2 ("output_size");
  size_t found;

  float time = 0;

  if (myfile1.is_open())
  {
    if (myfile2.is_open())
    {
	while ( getline (myfile2,line2) )
	{
		getline (myfile1,line1);
	
		found=line2.find("-1");
		if (found!=string::npos)
		cout << line1 << endl;
	}

    myfile1.close();
    myfile2.close();
    }
    else cout << "Unable to open file"; 
  }
  else cout << "Unable to open file";

  return 0;

}
