#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

float averageLength (float medium, float newvalue, float elements)
{
	float r, total;
	total = medium * (elements - 1);
	r = (total + newvalue) / elements;
	return r;
}

float averageInterTime (float averagetime, float lasttime, float newtime, float elements)
{
	float r, total, intertime;

	//cout << "Avg time: " << averagetime << " last time: " << lasttime << " new time: " << newtime << " elements: " << elements << endl;

	if (elements <= 2) {
		intertime = newtime - lasttime;
		//cout << "Avg Inter time = new - last = " << intertime << endl;
		return intertime;
	}
	else {
		total = averagetime * (elements - 2);
		//cout << "Total = avg time * elements -2 = " << total;
		intertime = newtime - lasttime;
		//cout << "Inter time = new - last = " << intertime << endl;
		r = (total + intertime) / (elements - 1);
		//cout << "Avg Inter time = (total + intertime)/elements-1 = " << r << endl;
		return r;
	}
}

float maximumLength (float maxlength, float newvalue)
{
	if (maxlength >= newvalue)
		return maxlength;
	else
		return newvalue;
}

float minimumLength (float minlength, float newvalue)
{
	if (minlength <= newvalue)
		return minlength;
	else
		return newvalue;
}

int main(int argc, char **argv){

  string line;
  ifstream myfile ("RTU_data.txt");
  map <string, vector<float> > table;
  map<string, vector<float> >::iterator it;
  float time = 0;

  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
	string tablekey = line.substr(0,8);
	it = table.find(tablekey);
	float length;
	float timestamp;
	istringstream (line.substr(8,8)) >> length;
	istringstream (line.substr(16)) >> timestamp;

	if (timestamp - time >= 10) {

		for(it = table.begin(); it != table.end(); ++it)
		{

			//cout << "Key: " << it->first << " Values: " << it->second[0] << " " << it->second[1] << " " << it->second[2] << " " << it->second[3] << " " << it->second[4] << " " << it->second[5] << "\n";

			cout << "1 " << " 1:" << it->second[0] << " 2:" << it->second[1] << " 3:" << it->second[2] << " 4:" << it->second[3] << " 5:" << it->second[4] << "\n";
		}
		//cout << endl << endl;

		table.clear();
		time = time + 10;
	}

	float sample_timestamp = timestamp - time;
	
	if(it != table.end()) {
		table[tablekey][0]++;
		table[tablekey][1] = averageLength(table[tablekey][1],length,table[tablekey][0]);
		table[tablekey][2] = minimumLength(table[tablekey][2],length);
		table[tablekey][3] = maximumLength(table[tablekey][3],length);
		table[tablekey][4] = averageInterTime (table[tablekey][4], (table[tablekey][5]), timestamp, table[tablekey][0]);
		table[tablekey][5] = timestamp;
	}
	else {
		table[tablekey].push_back(1); //packet count
		table[tablekey].push_back(length); // average packet length
		table[tablekey].push_back(length); // minimum packet length
		table[tablekey].push_back(length); // maximum packet length
		table[tablekey].push_back(10); // average packet inter arrival time
		table[tablekey].push_back(timestamp); // last packet timestamp

	}
	
     // cout << line << '\n';
    }
  for(it = table.begin(); it != table.end(); ++it)
  {

	//cout << "Key: " << it->first << " Values: " << it->second[0] << " " << it->second[1] << " " << it->second[2] << " " << it->second[3] << " " << it->second[4] << " " << it->second[5] << "\n";

	cout << "1 " << " 1:" << it->second[0] << " 2:" << it->second[1] << " 3:" << it->second[2] << " 4:" << it->second[3] << " 5:" << it->second[4] << "\n";
  }

    myfile.close();
  }

  else cout << "Unable to open file"; 

  return 0;

}
