#include <bits/stdc++.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include<fstream>
using namespace std;

typedef long long ll;
typedef vector<int> vi;

struct mips{
    map <string,int> getRegister;         // Get all the register indexes
    vector<unordered_map<string, int> > countOfInstructions,            // vector stores the count for all the files
                             labels;
    vector<vector<string> > Instruction;              // stores all the instructions
    int N,M,rowDelay=10,columnDelay=2;               // number of cpu cores, max cycles, row delay and column delay
    string tempi[10]={"add","sub","mul","addi","j","beq","bne","slt","lw","sw"}; // all the valid instructions
    vector<vector<int> > registers;     // the 32 registers
    int memory[1<<20] = {0};
    int MAX_INSTRUCTIONS_MEMORY = 1024*1024 - 1;
    vector <int> pc;



    void getregister() // Initialize the register indexes
    {
        string temp[]={"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$s8","$ra"};
        for(int i=0;i<32;i++)
        {
            getRegister[temp[i]]=i;
        }
        return;
    }
    void begin(int N){
        // Initialize my mips processor
        for(int j=0;j<N;j++){
            unordered_map<string, int> countInstructions;
            vector<int> reg(32,0);
            for(int i=0;i<10;i++ ){
                countInstructions[tempi[i]]=0;
            }
            countOfInstructions.push_back(countInstructions);
            registers.push_back(reg);
        }
    }
    string& leftTrim(string& str, string& chars)
    // Removing the spaces from a string
    {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }
    void readFile()
    // reading the inputs in the required format and storing the instructions in vectors.
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
            //int reg[32];
            //registers.push_back(reg);
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
                    else if(str[0]!='#'){
                        replace(str.begin(), str.end(), ',', ' ');
                        stringstream ss(str);
                        string token;
                        while (getline(ss,token, ' '))
                        {
                            if(token.compare("")==0){
                                continue;
                            }
                            else{
                                v.push_back(token);
                            }
                        }
                        
                    }
                }
            }
            in.close();
            Instruction.push_back(v);
        }
        /*for (auto it = Instruction[0].begin(); it != Instruction[0].end(); it++)
        cout << *it << " ";*/
    }
    bool isValidRegister(string str,bool modify = false){
        // Check whether the register is valid for the given task or not
        if (getRegister.count(str) > 0)
        {
            if(modify==true){
                if(str.compare("$zero")==0 || str.compare("$at")==0){
                    //cout<<"No permission to modify register"<<"\n";
                    return false;
                }
            }
            return true;
        }
        else
        {
            //std::cout << "Invalid Register: " << str << std::endl;
            return false;
        }
    }
    bool isValidInstruction(string str){
        // Chech whether the instruction is valid or not
        for(int i=0;i<10;i++){
            if(str.compare(tempi[i])==0){
                return true;
            }
        }
        //cout<<"Invalid Instruction"<<"\n";
        return false;
    }

    pair<string,int> offset(string s) {
        // parse the offset instructions
        string off = "", reg = "";
        int i=0, n=s.length();
        while (i<n && s[i] != '(') off += s[i], i++;
        i++;
        while (i<n && s[i] != ')') reg += s[i], i++;
        if (i!=n-1 || !isNumber(off)) return {"",0};
        return {reg, stoi(off)};
    }

    bool isNumber(string str) {
        // check if a string is valid integer
        int i = (str[0]=='-');
        while (i<str.length()) {
            if (str[i]<'0' || str[i]>'9') return false;
            i++;
        }
        return true;
    }

    int syntaxCheck() {
        // Check the syntax of the instructions and load these instructions to the memory
        int i = 0, PC = 0, memoryloc = 0;

        while (i < Instruction.size()) {
            int j = 0;
            pc.push_back(PC);
            unordered_map<string, int> lab;
            int n = Instruction[i].size();
            while(j < Instruction[i].size()) {
                PC += 4;
                if (PC > MAX_INSTRUCTIONS_MEMORY) {
                    return -2;
                }
                if (Instruction[i][j] == "add" || Instruction[i][j] == "sub" || Instruction[i][j] == "mul" || Instruction[i][j] == "slt") {
                    if (j+3 >= n) return -1;
                    if (!isValidRegister(Instruction[i][j+1], true) ||
                        !isValidRegister(Instruction[i][j+2]) ||
                        !isValidRegister(Instruction[i][j+3])) return -1;

                    memory[PC] = memoryloc + j;
                    j += 4;
                }
                else if (Instruction[i][j] == "beq" || Instruction[i][j] == "bne") {
                    if (j+3 >= n) return -1;
                    if (!isValidRegister(Instruction[i][j+1]) ||
                        !isValidRegister(Instruction[i][j+2]) ||
                        isValidRegister(Instruction[i][j+3]) ||
                        isValidInstruction(Instruction[i][j+3])) return -1;

                    memory[PC] = memoryloc + j;
                    j += 4;
                }
                else if (Instruction[i][j] == "j") {
                    if (j+1 >= n) return -1;
                    if (isValidRegister(Instruction[i][j+1]) ||
                        isValidInstruction(Instruction[i][j+1])) return -1;

                    memory[PC] = memoryloc + j;
                    j += 2;
                }
                else if (Instruction[i][j] == "lw" || Instruction[i][j] == "sw") {
                    if (j+2 >= n) return -1;
                    if (!isValidRegister(Instruction[i][j+1]) ||
                        !isValidRegister(offset(Instruction[i][j+2]).first)) return -1;

                    memory[PC] = memoryloc + j;
                    j += 3;
                }
                else if (Instruction[i][j] == "addi") {
                    if (j+3 >= n) return -1;
                    if (!isValidRegister(Instruction[i][j+1], true) ||
                        !isValidRegister(Instruction[i][j+2]) ||
                        !isNumber(Instruction[i][j+3])) return -1;

                    memory[PC] = memoryloc + j;
                    j += 4;
                }
                else {
                    PC -= 4;
                    // memory[PC-4] = i;
                    
                    string s = Instruction[i][j];
                    if (Instruction[i][j].back() == ':') static_cast<void>(s = s.substr(0, int(s.length())-1)), j += 1;
                    else {
                        if (j+1 >= n) return -1;
                        if (Instruction[i][j+1] != ":") return -1;
                        j += 2;
                    }
                    if (isNumber(s)) return -1;
                    if (lab.find(s) != lab.end()) return -1;
                    lab.insert({s, PC});
                }
            }
            labels.push_back(lab);
            memoryloc += Instruction[i++].size();
        }
        return 0;
    }
    vector<int> add(int fileindex,int ind){
        int reg1,reg2,reg3;
        string s1=Instruction[fileindex][ind+1],s2=Instruction[fileindex][ind+2],s3=Instruction[fileindex][ind+3];
        reg1=getRegister[s1];
        reg2=getRegister[s2];
        reg3=getRegister[s3];
        if(not_safe(reg1,true) || not_safe(reg2,false) || not_safe(reg3,false)){
            vector<int> out(4,-1);  //return {-1,-1,-1,-1} when not safe
            return out;
        }
        else{
            registers[fileindex][reg1]=registers[fileindex][reg2]+registers[fileindex][reg3];
            vector<int> out(4,-2);  //return {-2,-2,-2,-2} when safe
            return out;
        }
    }
    vector<int> sub(int fileindex,int ind){
        int reg1,reg2,reg3;
        string s1=Instruction[fileindex][ind+1],s2=Instruction[fileindex][ind+2],s3=Instruction[fileindex][ind+3];
        reg1=getRegister[s1];
        reg2=getRegister[s2];
        reg3=getRegister[s3];
        if(not_safe(reg1,true) || not_safe(reg2,false) || not_safe(reg3,false)){
            vector<int> out(4,-1);
            return out;
        }
        else{
            registers[fileindex][reg1]=registers[fileindex][reg2]-registers[fileindex][reg3];
            vector<int> out(4,-2);
            return out;
        }
    }
    vector<int> mul(int fileindex,int ind){
        int reg1,reg2,reg3;
        string s1=Instruction[fileindex][ind+1],s2=Instruction[fileindex][ind+2],s3=Instruction[fileindex][ind+3];
        reg1=getRegister[s1];
        reg2=getRegister[s2];
        reg3=getRegister[s3];
        if(not_safe(reg1,true) || not_safe(reg2,false) || not_safe(reg3,false)){
            vector<int> out(4,-1);
            return out;
        }
        else{
            registers[fileindex][reg1]=registers[fileindex][reg2]*registers[fileindex][reg3];
            vector<int> out(4,-2);
            return out;
        }
    }
    vector<int> addi(int fileindex,int ind){
        int reg1,reg2,reg3;
        string s1=Instruction[fileindex][ind+1],s2=Instruction[fileindex][ind+2],s3=Instruction[fileindex][ind+3];
        reg1=getRegister[s1];
        reg2=getRegister[s2];
        reg3=stoi(s3);
        if(not_safe(reg1,true) || not_safe(reg2,false)){
            vector<int> out(4,-1);
            return out;
        }
        else{
            registers[fileindex][reg1]=registers[fileindex][reg2]+reg3;
            vector<int> out(4,-2);
            return out;
        }
    }
    vector<int> bne(int fileindex,int ind){
        int reg1,reg2,reg3;
        string s1=Instruction[fileindex][ind+1],s2=Instruction[fileindex][ind+2],s3=Instruction[fileindex][ind+3];
        reg1=getRegister[s1];
        reg2=getRegister[s2];
        if(labels[fileindex].find(s3)==labels[fileindex].end()){
            cout<<"Label not present"<<endl;
        }
        else{
            reg3=labels[fileindex][s3];
        }
        
        if(not_safe(reg1,false) || not_safe(reg2,false) ){
            vector<int> out(4,-1);
            return out;
        }
        else{
            int a,b,ans;
            a=registers[fileindex][reg1];
            b=registers[fileindex][reg2];
            if(a!=b){
                ans=reg3;
            }
            else{
                ans=ind+4;
            }
            vector<int> out(4,-2);
            out[1]=ans;
            return out; // return vector as {-2,output_index,-2,-2}
        }
    }
    vector<int> beq(int fileindex,int ind){
        int reg1,reg2,reg3;
        string s1=Instruction[fileindex][ind+1],s2=Instruction[fileindex][ind+2],s3=Instruction[fileindex][ind+3];
        reg1=getRegister[s1];
        reg2=getRegister[s2];
        if(labels[fileindex].find(s3)==labels[fileindex].end()){
            cout<<"Label not present"<<endl;
        }
        else{
            reg3=labels[fileindex][s3];
        }
        if(not_safe(reg1,false) || not_safe(reg2,false) ){
            vector<int> out(4,-1);
            return out;
        }
        else{
            int a,b,ans;
            a=registers[fileindex][reg1];
            b=registers[fileindex][reg2];
            if(a==b){
                ans=reg3;
            }
            else{
                ans=ind+4;
            }
            vector<int> out(4,-2);
            out[1]=ans;
            return out; // return vector as {-2,output_index,-2,-2}
        }
    }
    vector<int> j(int fileindex,int ind){
        int ans;
        string s1=Instruction[fileindex][ind+1];
        if(labels[fileindex].find(s1)==labels[fileindex].end()){
            cout<<"Label not present"<<endl;
        }
        else{
            ans=labels[fileindex][s1];
        }
        vector<int> out(4,-2);
        out[1]=ans;
        return out; // return vector as {-2,output_index,-2,-2}
    }
    vector<int> slt(int fileindex,int ind){
        int reg1,reg2,reg3;
        string s1=Instruction[fileindex][ind+1],s2=Instruction[fileindex][ind+2],s3=Instruction[fileindex][ind+3];
        reg1=getRegister[s1];
        reg2=getRegister[s2];
        reg3=getRegister[s3];
        if(not_safe(reg1,true) || not_safe(reg2,false) || not_safe(reg3,false)){
            vector<int> out(4,-1);
            return out;
        }
        else{
            if(registers[fileindex][reg2]<registers[fileindex][reg3]){
                registers[fileindex][reg1]=1;
            }
            else{
                registers[fileindex][reg1]=0;   
            }
            vector<int> out(4,-2);
            return out;
        }
    }
    vector<int> lw(int fileindex,int ind){
        int reg1,addr;
        string s1=Instruction[fileindex][ind+1],s=Instruction[fileindex][ind+2],reg2,offset;
        reg1=getRegister[s1];
        int h = 0;
        while(s[h] != '(') {
            h++;
        }
        if(h == 0) {
            offset = "0";
        } else if (h == s.size()) {
            h = -1;
            offset = "0";
        } else {
            for(int i = 0; i < h; i++) {
                offset += s[i];
            }
        }
        for(int i = h+1; i < s.size()-1; i++) {
            reg2 += s[i];
        }
        if(h == -1) {
            reg2 += s[s.size()-1];
        }
        int b=getRegister[reg2];
        int addr = registers[fileindex][b]+stoi(offset);
        if(not_safe(b,false)){
            vector<int> out(4,-1);
            return out;
        }
        else{
            //need to add no of instructions
            vector<int> out{reg1,addr,1};   //return {register addr,memory addr,boolean 1}
            return out;
        }
    }
    vector<int> sw(int fileindex,int ind){
        int reg1,addr;
        string s1=Instruction[fileindex][ind+1],s=Instruction[fileindex][ind+2],reg2,offset;
        reg1=getRegister[s1];
        int h = 0;
        while(s[h] != '(') {
            h++;
        }
        if(h == 0) {
            offset = "0";
        } else if (h == s.size()) {
            h = -1;
            offset = "0";
        } else {
            for(int i = 0; i < h; i++) {
                offset += s[i];
            }
        }
        for(int i = h+1; i < s.size()-1; i++) {
            reg2 += s[i];
        }
        if(h == -1) {
            reg2 += s[s.size()-1];
        }
        int b=getRegister[reg2];
        int addr = registers[fileindex][b]+stoi(offset);
        if(not_safe(reg1,false)){
            vector<int> out(4,-1);
            return out;
        }
        else{
            //need to add no of instructions
            vector<int> out{reg1,addr,0};   //return {register addr,memory addr,boolean 1}
            return out;
        }
    }
    
};

int main(int argc, char** argv) {
    mips m1;
    m1.readFile();
    m1.syntaxCheck();
    //vector<int> v;
    //v=m1.addi(0,0);
    /*int x=m1.bne(0,2);
    cout<<x<<endl;*/
    for(int j = 0; j < m1.Instruction[0].size(); j++) {
        cout << m1.Instruction[0][j] << "|";
    }
}