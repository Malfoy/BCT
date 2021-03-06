#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "zstr.hpp"




using namespace std;



string main_nuc(const string& str){
	uint Acount(0),Tcount(0);
	for(uint i(0);i<str.size();++i){
		if(str[i]=='A'){Acount++;}
		if(str[i]=='T'){Tcount++;}
	}
	if(Acount>Tcount){
		string result(str.size(),'A');
		return result;
	}else{
		string result(str.size(),'T');
		return result;
	}
	cout<<"Problem"<<endl;
	return str;
}



uint count_upper_case(const string& str){
	int res(0);
	for(uint i(0); i< str.size(); ++i){
		switch(str[i]){
			//~ case 'a':break;
			case 'A':++res;break;
			//~ case 'c':break;
			case 'C':++res;break;
			//~ case 'g':break;
			case 'G':++res;break;
			//~ case 't':break;
			case 'T':++res;break;
			//~ default: return res;
		}
	}
	return res;
}



pair<string,string> protect_real_nuc(const string& str,const string& tail, bool polyAtail){
	uint minpolytail(5);
	uint current_tail(0);
	uint nucleotide_to_remove(0);
	for(uint i(0);i<tail.size();++i){
		if( (tail[tail.size()-i-1]=='A' and polyAtail) or (tail[tail.size()-i-1]=='T' and not polyAtail) ){
			current_tail++;
			if(current_tail>=minpolytail){
				break;
			}
		}else{
			current_tail=0;
			nucleotide_to_remove=i+1;
		}
	}
	//~ cout<<"PRN"<<endl;
	//~ cout<<tail.substr(tail.size()-nucleotide_to_remove)+str<<endl;
	//~ cout<<tail.substr(0,tail.size()-nucleotide_to_remove)<<endl;
	return{tail.substr(tail.size()-nucleotide_to_remove)+str,tail.substr(0,tail.size()-nucleotide_to_remove)};
}



pair<string,string> clean_prefix2(const string& str, uint min_length, uint max_missmatch){
	if(str.size()<min_length){
		return {str,""};
	}
	bool polyAtail(false);
	uint ca(0),cc(0),cg(0),ct(0);
	for(uint i(0);i<min_length;++i){
		switch(str[i]){
			case 'A':++ca;break;
			case 'C':++cc;break;
			case 'G':++cg;break;
			default:++ct;break;
		}
		if(cc+cg>max_missmatch){
			return {str,""};
		}
	}
	if(ca>ct){
		polyAtail=true;
		if (ca<min_length-max_missmatch){
			return {str,""};
		}
	}else{
		if (ct<min_length-max_missmatch){
			return {str,""};
		}
	}
	uint nuc_to_remove(0);
	for(uint i(0);i+min_length<str.size();++i){
		switch(str[i]){
			case 'A':--ca;break;
			case 'C':--cc;break;
			case 'G':--cg;break;
			default:--ct;break;
		}
		switch(str[i+min_length]){
			case 'A':++ca;break;
			case 'C':++cc;break;
			case 'G':++cg;break;
			default:++ct;break;
		}
		if(polyAtail){
			if (ca<min_length-max_missmatch){
				break;
			}
		}else{
			if (ct<min_length-max_missmatch){
				break;
			}
		}
		nuc_to_remove++;
	}
	if(nuc_to_remove+min_length==str.size()){
		nuc_to_remove--;
	}
	//~ cout<<str.substr(nuc_to_remove+min_length)<<endl;
	//~ cout<<str.substr(0,nuc_to_remove+min_length)<<endl;
	return protect_real_nuc(str.substr(nuc_to_remove+min_length),str.substr(0,nuc_to_remove+min_length),polyAtail);
	return{str.substr(nuc_to_remove+min_length),str.substr(0,nuc_to_remove+min_length)};
}



pair<string,string> clean_homo2(string& str, uint min_length, uint max_missmatch){
	auto pair=clean_prefix2(str,min_length,max_missmatch);
	reverse(str.begin(),str.end());
	auto pair2=clean_prefix2(str,min_length,max_missmatch);
	if(pair2.second.empty()){
		//~ cout<<"p1"<<endl;
		return {pair.first,main_nuc(pair.second)+"$"};
	}
	if(pair.second.empty()){
		reverse(pair2.first.begin(),pair2.first.end());
		return {pair2.first,"$"+main_nuc(pair2.second)};
	}
	if(pair.second.size()<pair2.second.size()){
		//~ cout<<"p2"<<endl;
		return {pair.first,main_nuc(pair.second)+"$"};
	}else{
		//~ cout<<"s1"<<endl;
		reverse(pair2.first.begin(),pair2.first.end());
		return {pair2.first,"$"+main_nuc(pair2.second)};
	}
	//~ cout<<"p3"<<endl;
	return {pair.first,main_nuc(pair.second)+"$"};
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



void clean(string& str){
	for(uint i(0); i< str.size(); ++i){
		switch(str[i]){
			case 'a':break;
			case 'A':break;
			case 'c':break;
			case 'C':break;
			case 'g':break;
			case 'G':break;
			case 't':break;
			case 'T':break;
			case 'N':break;
			default: str="";return;
		}
	}
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}


int main(int argc, char ** argv){
	if(argc<2){
		cout<<"[Fasta file] (out file) (homo size) (backup file) (fastq)"<<endl;
		exit(0);
	}
	string input(argv[1]);
	bool cleaning(true),fastq_mode(false);
	uint min_size(21);
	string out_file("clean_reads.fa.gz");
	string recover_file("Arecover");
	if(argc>2){
		out_file=((argv[2]));
	}
	if(argc>3){
		min_size=(stoi(argv[3]));
	}
	if(argc>4){
		recover_file=((argv[4]));
	}
	if(argc>5){
		fastq_mode=true;
	}

	srand (time(NULL));
	string header, sequence,line,useless;
	istream* in;
	in=new zstr::ifstream(input);
	//~ ifstream in(input);
	ostream* out_backup;
	ostream* out_clean;
	out_backup=new zstr::ofstream((recover_file).c_str());
	out_clean=new zstr::ofstream((out_file).c_str());
	//~ ofstream out_backup(recover_file);
	//~ ofstream out_clean(out_file);
	uint i(0);
	while(not in->eof()){
		if(fastq_mode){
			getline(*in,header);
			getline(*in,sequence);
			getline(*in,useless);
			getline(*in,useless);
		}else{
			getline(*in,header);
			char c=in->peek();
			while(c!='>' and c!=EOF){
				getline(*in,line);
				sequence+=line;
				c=in->peek();
			}
		}
		//WE CLEAN THE SEQ
		clean(sequence);
		if(sequence.size()>5){
			//~ cout<<sequence<<endl;
			auto pair=clean_homo2(sequence,min_size,3);
			*out_backup<<pair.second<<"\n";
			*out_clean<<'>'+header<<'\n'<<pair.first<<"\n";
			//~ cout<<pair.first<<endl;
		}
		sequence="";
	}
	out_backup->flush();
	out_clean->flush();
	delete(out_backup);
	delete(out_clean);
	delete(in);
}
