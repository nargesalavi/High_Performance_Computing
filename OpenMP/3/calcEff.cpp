#include <iostream>
#include <fstream>

using namespace std;

int main(){
	double NumberofIN,t,NThread,serialT,paralletT;
	ifstream infile;
	ofstream out1,out2,out3;
	out1.open("out1.txt");
	out2.open("out2.txt");
	out3.open("out3.txt");
	infile.open("res.txt");
	NumberofIN=0;
	while(infile >> NumberofIN){
		if (t!=NumberofIN){
			out1 <<"\n";
			out2 << "\n";
			out3 << "\n";
		}
		infile >> NThread;
		infile >> serialT;
		infile >> paralletT;
		out1 << NumberofIN << " " << NThread << " " << (serialT/paralletT)/NThread << "\n";
		out2 << (serialT/paralletT)/NThread << "\n";
		out3 << (serialT/paralletT)<< "\n";
		t=NumberofIN;
	}
	out1.close();
	out2.close();
	infile.close();
}