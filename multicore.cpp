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
	map<string, int> countOfInstructions;
	void begin(){
		string tempi[10]={"add","sub","mul","addi","j","beq","bne","slt","lw","sw"}; 
		for(int i=0;i<10;i++ ){
			countOfInstructions[tempi[i]]=0;
		}
	}
	vector<string> Instruction;
};

string& leftTrim(string& str, string& chars)
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

void readFile(std::string fileName, std::vector<std::string> & vecOfStrs)
{
    std::ifstream in(fileName.c_str());
    if(!in)
    {
        std::cerr << "Cannot open the File : "<<fileName<<std::endl;
        return;
    }
    std::string str;
    string chars_to_trim = " ,\t";
    while (std::getline(in, str))
    {
        if(str.size() > 0){
        	leftTrim(str, chars_to_trim);
            vecOfStrs.push_back(str);
        }
    }
    in.close();
    return;
}

void getregister()
{
	string temp[]={"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$s8","$ra"}; 
	for(int i=0;i<32;i++)
	{
		getRegister[temp[i]]=i;
	}
}

int main(int argc, char** argv) {
    int N,M,rowDelay=10,columnDelay=2;
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
    	return 0;
    }
    getregister();
    

 }