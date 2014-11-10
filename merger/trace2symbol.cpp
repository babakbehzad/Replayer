/************************************************************/
/*Adjust the contents of the trace file for a unified format*/
/************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <string.h>

using namespace std;

ifstream fin;
ofstream fout;

/*Convert integer to string*/
string my_itos(int i)
{
        string r;
        char c[128];
        memset(c, 0, 128);
        sprintf(c, "%d", i);
        r += c;
        return r;
}

/*Get the name of the next file*/
string nextName(string name)
{
        ostringstream n;
        int posi;

        posi = name.find(".");
		int nextNumber = atoi(name.substr(posi+1, name.size()).c_str()) + 1;
        n << name.substr(0, posi).c_str() << "." << nextNumber;
		cout << n.str() << endl;
        return n.str();
}

/*Adjust the contents of the trace file for a unified format*/
/*
Example:
After applying work():
  1: MPI_Bcast= (void buffer= 1198516368, int count= 80, MPI_Datatype datatype= 1247288704, int root= 0,
                 MPI_Comm comm= 6604320, )S=[ 1367981215084679 ] E=[ 1367981215086238 ] D=[ 1559 ]
*/
void work()
{
	string s;
	int count = 0;

	double startTime, endTime, durTime;

	getline(fin ,s);
	while (fin)
	{
		count++;
		fout << count << ": ";

		char *str  = (char *) s.c_str();
		char *p = strtok(str, " ");

		startTime = atof(p);

		p = strtok(NULL, " ");
		string funcName(p);
		fout << funcName << "= ";

		p = strtok(NULL, " ");
		string funcArgs(p);
		fout << funcArgs ;

		fout.setf(ios::fixed);
		fout << "S=[ " << startTime << " ] ";

		p = strtok(NULL, " ");
		string ret_value(p);
		fout.setf(ios::fixed);
		fout << "R=[ " << ret_value << " ] ";

		p = strtok(NULL, " ");
		durTime = atof(p);
		endTime = startTime + durTime;


		fout << "E=[ " << endTime << " ] ";

		fout << "D=[ " << durTime << " ] ";

		fout << std::endl;

		getline(fin ,s);
	}
}

int main(int argc,char *argv[])
{
	string name,oname;
	int i;

	//char ts1[16] = "0.trace";
	//char ts2[12] = "0.symbol";
	//char ts3[10] = "1";


	if (argc != 4)
	{
		cout<<"Usage: trace2symbol inputfile outputfile $n\n"<<endl;
		return 0;
	}

	name.assign(argv[1]);
	//name.assign(ts1);
	oname.assign(argv[2]);
	//oname.assign(ts2);
	for (i=0;i<atoi(argv[3]);i++)
	//for (i=0;i<atoi(ts3);i++)
	{
		fin.open(name.c_str());

		if (fin.fail())
		{
			cout<<"Bad file name!\n"<<endl;
			return 0;
		}

		fout.open(oname.c_str());

		cout<<"Trace2Symbol(V1.0) "<<endl;

		cout << "Dealing with " << name << "...";

		work();

		fin.close();
		fout.close();
		fin.clear();
		fout.clear();

		cout << "Done." << endl;
		name=nextName(name);
		oname=nextName(oname);
	}
	return 0;
}
