#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>



using namespace std;



string reconstitute(const string& read,const string& harry){
	for(uint i(0);i<harry.size();++i){
		if(harry[i]=='$'){
			return harry.substr(0,i)+read+harry.substr(i+1);
		}
	}
	return read;
}


string getLineFasta(ifstream* in){
	string line,result;
	getline(*in,line);
	char c=in->peek();
	while(c!='>' and c!=EOF){
		getline(*in,line);
		result+=line;
		c=in->peek();
	}
	return result;
}



int main(int argc, char ** argv){
	if(argc<2){
		cout<<"[Fasta file] [Recover File] [outfile]"<<endl;
		exit(0);
	}
	string inputReads(argv[1]);
	string inputRecover(argv[2]);

	if(argc<3){
		cout<<"Need two files"<<endl;
		exit(0);
	}
	srand (time(NULL));
	string header, sequence,line,harry;
	ifstream inReads(inputReads);
	ifstream inRecover(inputRecover);
	ofstream out(argv[3]);
	while(not inReads.eof() and not inRecover.eof()){
		getline(inReads,header);
		getline(inReads,line);
		getline(inRecover,harry);
		if(not harry.empty()){
		//~ cout<<line<<endl;
		//~ cout<<harry<<endl;
		//~ cout<<reconstitute(line,harry)<<endl;cin.get();
			out<<header<<"\n"<<reconstitute(line,harry)<<"\n";
		}
	}
}