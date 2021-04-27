#include <bits/stdc++.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include<fstream>
using namespace std;

typedef long long ll;
typedef vector<int> vi;

map <string,int> getRegister;

struct mips{
	int PC = 0;
	vector<map<string, int>> countOfInstructions;
	vector<vector<string>> Instruction;
	int N,M,rowDelay=10,columnDelay=2;
	void getregister()
	{
		string temp[]={"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$s8","$ra"}; 
		for(int i=0;i<32;i++)
		{
			getRegister[temp[i]]=i;
		}
		return;
	}
	void begin(int N){
		string tempi[10]={"add","sub","mul","addi","j","beq","bne","slt","lw","sw"}; 
		for(int j=0;j<N;j++){
			map<string, int> countInstructions;
			for(int i=0;i<10;i++ ){
				countInstructions[tempi[i]]=0;
			}
			countOfInstructions.push_back(countInstructions);
		}
	}
	string& leftTrim(string& str, string& chars)
	{
	    str.erase(0, str.find_first_not_of(chars));
	    return str;
	}
	void readFile()
	{
		
	    cout<<"Enter number of CPU cores"<<"\n";
	    cin>>N;
	    cout<<"Enter Halt time"<<"\n";
	    cin>>M;
	    cout<<"Enter MIPS files"<<"\n";
	    string files[N];
	    for(int i=0;i<N;i++){
	    	cin>>files[i];
	    }
	    cout<<"Enter the DRAM timing values in the format : ROW_ACCESS_DELAY  COL_ACCESS_DELAY"<<"\n";
	    cin>>rowDelay>>columnDelay;
	    if(N<=0 || M<0 || rowDelay<=0 || columnDelay<=0){
	    	cout<<"Incorrect Input Format"<<"\n";
	    	return;
	    }
	    getregister();
	    begin(N);
	    for(int i=0;i<N;i++){
	    	vector<string> v;
	    	std::ifstream in(files[i].c_str());
		    if(!in)
		    {
		        std::cerr << "Cannot open the File : "<<files[i]<<std::endl;
		        return;
		    }
		    std::string str;
		    string chars_to_trim = " ,\t";
		    while (std::getline(in, str))
		    {
		        if(str.size() > 0){
		        	leftTrim(str, chars_to_trim);
		        	if(str[0]=='#')
		        		continue;
		        	else if(str[0]!='#')
		            	v.push_back(str);
		        }
		    }
		    in.close();
		    Instruction.push_back(v);
		}
	}
	bool isValidRegister(string str,bool modify){
		if (getRegister.count(str) > 0)
	    {
	        if(modify==true){
	        	if(str.compare("$zero")==0 || str.compare("$at")==0){
	        		cout<<"No permission to modify register"<<"\n";
	        		return false;
	        	}
	        }
	        return true; 
	    }
	    else
	    {
	        std::cout << "Invalid Register" << std::endl;
	        return false;
	    }
	}
	bool isValidInstruction(string str){
		
	}
	
};

int main(int argc, char** argv) {
	/*mips m1;
	m1.getregister();
	m1.readFile();*/
 }