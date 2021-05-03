#include <bits/stdc++.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include<fstream>
using namespace std;

typedef long long ll;
typedef vector<int> vi;

struct cpu {
    struct core {
        map <string,int> getRegister;         // Get all the register indexes
        unordered_map<string, int> countOfInstructions,            // vector stores the count for all the files
                                 labels;
        vector<string> Instruction;              // stores all the instructions
        int rowDelay=10,columnDelay=2;               // number of cpu cores, max cycles, row delay and column delay
        string tempi[10]={"add","sub","mul","addi","j","beq","bne","slt","lw","sw"}; // all the valid instructions
        int registers[32] = {0};     // the 32 registers
        int memory[1<<20] = {0};
        int MAX_INSTRUCTIONS_MEMORY = 1024*1024 - 1;
        int PC = 0;
        unordered_set<int> involvedRegisters;
        int totalInstructions = 0;
        int currentinstruction = 1;
        int i = 0;

        void getregister() // Initialize the register indexes
        {
            string temp[]={"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$s8","$ra"};
            for(int i=0;i<32;i++)
            {
                getRegister[temp[i]]=i;
            }
            return;
        }
        void begin(){
            // Initialize my mips processor
            for(int i=0;i<10;i++ ){
                countOfInstructions[tempi[i]]=0;
            }
        }
        string& leftTrim(string& str, string& chars)
        // Removing the spaces from a string
        {
            str.erase(0, str.find_first_not_of(chars));
            return str;
        }
        int readFile(string filename)
        // reading the inputs in the required format and storing the instructions in vectors.
        {
            getregister();
            begin();

            //int reg[32];
            //registers.push_back(reg);
            std::ifstream in(filename);
            if(!in)
            {
                return -3;
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
                                Instruction.push_back(token);
                            }
                        }
                        
                    }
                }
            }
            in.close();
            
            return syntaxCheck();
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
        
        bool not_safe(int ind, bool modifying = false) {
            if(modifying == true) {
                if (ind == 0 || ind == 1) {
                    return true;
                }
            }
            if(involvedRegisters.find(ind) != involvedRegisters.end()) {
                return true;
            }
            
            return false;
        }

        int syntaxCheck() {
            // Check the syntax of the instructions and load these instructions to the memory
            int j = 0;
            int n = Instruction.size();
            while(j < Instruction.size()) {
                PC += 4;
                if (PC > MAX_INSTRUCTIONS_MEMORY) {
                    return -2;
                }
                if (Instruction[j] == "add" || Instruction[j] == "sub" || Instruction[j] == "mul" || Instruction[j] == "slt") {
                    totalInstructions++;
                    if (j+3 >= n) return -1;
                    if (!isValidRegister(Instruction[j+1], true) ||
                        !isValidRegister(Instruction[j+2]) ||
                        !isValidRegister(Instruction[j+3])) return -1;

                    memory[PC] = j;
                    j += 4;
                }
                else if (Instruction[j] == "beq" || Instruction[j] == "bne") {
                    if (j+3 >= n) return -1;
                    if (!isValidRegister(Instruction[j+1]) ||
                        !isValidRegister(Instruction[j+2]) ||
                        isValidRegister(Instruction[j+3]) ||
                        isValidInstruction(Instruction[j+3])) return -1;
                    totalInstructions++;
                    memory[PC] = j;
                    j += 4;
                }
                else if (Instruction[j] == "j") {
                    if (j+1 >= n) return -1;
                    if (isValidRegister(Instruction[j+1]) ||
                        isValidInstruction(Instruction[j+1])) return -1;
                    totalInstructions++;
                    memory[PC] = j;
                    j += 2;
                }
                else if (Instruction[j] == "lw" || Instruction[j] == "sw") {
                    if (j+2 >= n) return -1;
                    if (!isValidRegister(Instruction[j+1]) ||
                        !isValidRegister(offset(Instruction[j+2]).first)) return -1;
                    totalInstructions++;
                    memory[PC] = j;
                    j += 3;
                }
                else if (Instruction[j] == "addi") {
                    if (j+3 >= n) return -1;
                    if (!isValidRegister(Instruction[j+1], true) ||
                        !isValidRegister(Instruction[j+2]) ||
                        !isNumber(Instruction[j+3])) return -1;
                    totalInstructions++;
                    memory[PC] = j;
                    j += 4;
                }
                else {
                    PC -= 4;
                    // memory[PC-4] = i;
                    
                    string s = Instruction[j];
                    if (Instruction[j].back() == ':') (s = s.substr(0, int(s.length())-1)), j += 1;
                    else {
                        if (j+1 >= n) return -1;
                        if (Instruction[j+1] != ":") return -1;
                        j += 2;
                    }
                    if (isNumber(s)) return -1;
                    if (labels.find(s) != labels.end()) return -1;
                    labels.insert({s, PC});
                }
            }
            return 0;
        }
        vector<int> add(int ind){
            int reg1,reg2,reg3;
            string s1=Instruction[ind+1],s2=Instruction[ind+2],s3=Instruction[ind+3];
            reg1=getRegister[s1];
            reg2=getRegister[s2];
            reg3=getRegister[s3];
            if(not_safe(reg1,true) || not_safe(reg2,false) || not_safe(reg3,false)){
                vector<int> out(4,-1);  //return {-1,-1,-1,-1} when not safe
                return out;
            }
            else{
                registers[reg1]=registers[reg2]+registers[reg3];
                vector<int> out(4,-2);  //return {-2,-2,-2,-2} when safe
                return out;
            }
        }
        vector<int> sub(int ind){
            int reg1,reg2,reg3;
            string s1=Instruction[ind+1],s2=Instruction[ind+2],s3=Instruction[ind+3];
            reg1=getRegister[s1];
            reg2=getRegister[s2];
            reg3=getRegister[s3];
            if(not_safe(reg1,true) || not_safe(reg2,false) || not_safe(reg3,false)){
                vector<int> out(4,-1);
                return out;
            }
            else{
                registers[reg1]=registers[reg2]-registers[reg3];
                vector<int> out(4,-2);
                return out;
            }
        }
        vector<int> mul(int ind){
            int reg1,reg2,reg3;
            string s1=Instruction[ind+1],s2=Instruction[ind+2],s3=Instruction[ind+3];
            reg1=getRegister[s1];
            reg2=getRegister[s2];
            reg3=getRegister[s3];
            if(not_safe(reg1,true) || not_safe(reg2,false) || not_safe(reg3,false)){
                vector<int> out(4,-1);
                return out;
            }
            else{
                registers[reg1]=registers[reg2]*registers[reg3];
                vector<int> out(4,-2);
                return out;
            }
        }
        vector<int> addi(int ind){
            int reg1,reg2,reg3;
            string s1=Instruction[ind+1],s2=Instruction[ind+2],s3=Instruction[ind+3];
            reg1=getRegister[s1];
            reg2=getRegister[s2];
            reg3=stoi(s3);
            if(not_safe(reg1,true) || not_safe(reg2,false)){
                vector<int> out(4,-1);
                return out;
            }
            else{
                registers[reg1]=registers[reg2]+reg3;
                vector<int> out(4,-2);
                return out;
            }
        }
        vector<int> bne(int ind){
            int reg1,reg2,reg3;
            string s1=Instruction[ind+1],s2=Instruction[ind+2],s3=Instruction[ind+3];
            reg1=getRegister[s1];
            reg2=getRegister[s2];
            if(labels.find(s3)==labels.end()){
                cout<<"Label not present"<<endl;
            }
            else{
                reg3=labels[s3];
            }
            
            if(not_safe(reg1,false) || not_safe(reg2,false) ){
                vector<int> out(4,-1);
                return out;
            }
            else{
                int a,b,ans;
                a=registers[reg1];
                b=registers[reg2];
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
        vector<int> beq(int ind){
            int reg1,reg2,reg3;
            string s1=Instruction[ind+1],s2=Instruction[ind+2],s3=Instruction[ind+3];
            reg1=getRegister[s1];
            reg2=getRegister[s2];
            if(labels.find(s3)==labels.end()){
                cout<<"Label not present"<<endl;
            }
            else{
                reg3=labels[s3];
            }
            if(not_safe(reg1,false) || not_safe(reg2,false) ){
                vector<int> out(4,-1);
                return out;
            }
            else{
                int a,b,ans;
                a=registers[reg1];
                b=registers[reg2];
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
        vector<int> j(int ind){
            int ans = -2;
            string s1=Instruction[ind+1];
            if(labels.find(s1)==labels.end()){
                cout<<"Label not present"<<endl;
            }
            else{
                ans=labels[s1];
            }
            vector<int> out(4,-2);
            out[1]=ans;
            return out; // return vector as {-2,output_index,-2,-2}
        }
        vector<int> slt(int ind){
            int reg1,reg2,reg3;
            string s1=Instruction[ind+1],s2=Instruction[ind+2],s3=Instruction[ind+3];
            reg1=getRegister[s1];
            reg2=getRegister[s2];
            reg3=getRegister[s3];
            if(not_safe(reg1,true) || not_safe(reg2,false) || not_safe(reg3,false)){
                vector<int> out(4,-1);
                return out;
            }
            else{
                if(registers[reg2]<registers[reg3]){
                    registers[reg1]=1;
                }
                else{
                    registers[reg1]=0;
                }
                vector<int> out(4,-2);
                return out;
            }
        }
        vector<int> lw(int ind){
            int reg1,addr;
            string s1=Instruction[ind+1],s=Instruction[ind+2],reg2,offset;
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
            addr = registers[b]+stoi(offset);
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
        vector<int> sw(int ind){
            int reg1,addr;
            string s1=Instruction[ind+1],s=Instruction[ind+2],reg2,offset;
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
            addr = registers[b]+stoi(offset);
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
        
        vi simulate() {
            int instr_sz;
            if(i >= Instruction.size()) {
                return vi{0};
            }
            vi v;
            if (Instruction[i]=="add") {
                v = add(i);
                if (v == vi(4,-1)) {
                    return v;
                }
                i += 4;
                instr_sz = 4;
                countOfInstructions["add"]++;
            }
            else if (Instruction[i]=="sub") {
                v = sub(i);
                if (v == vi(4,-1)) {
                    return v;
                }
                i += 4;
                instr_sz = 4;
                countOfInstructions["sub"]++;
            }
            else if (Instruction[i]=="mul") {
                v = mul(i);
                if (v == vi(4,-1)) {
                    return v;
                }
                i += 4;
                instr_sz = 4;
                countOfInstructions["mul"]++;
            }
            else if (Instruction[i]=="beq") {
                v = beq(i);
                if (v == vi(4,-1)) {
                    return v;
                }
                i = v[1];
                v[1] = -2;
                //changed_reg = -2;
                instr_sz = 4;
                countOfInstructions["beq"]++;
            }
            else if (Instruction[i]=="bne") {
                v = bne(i);
                if (v == vi(4,-1)) {
                    return v;
                }
                i = v[1];
                v[1] = -2;
                //changed_reg = -2;
                instr_sz = 4;
                countOfInstructions["bne"]++;
            }
            else if (Instruction[i]=="slt") {
                v = slt(i);
                if (v == vi(4,-1)) {
                    return v;
                }
                i += 4;
                instr_sz = 4;
                countOfInstructions["slt"]++;
            }
            else if (Instruction[i]=="j") {
                v = j(i);
                if (v == vi(4,-1)) {
                    return v;
                }
                countOfInstructions["j"]++;
                instr_sz = 2;
                //changed_reg = -2;
            }
            else if (Instruction[i]=="lw") {
                // while (!empty_queue()) reduce_queue();
                countOfInstructions["lw"]++;
                v = lw(i);
                if (v == vi(4,-1)) {
                    return v;
                }
                //cout << "Cycle " << cycle << ": DRAM request issued by executing ";
                //for (int j=0; j<3; j++) cout << instructions[i+j] << " ";
                //cout << endl;
                //if (!NON_BLOCKING_MODE) reduce_queue();
                v.push_back(totalInstructions - currentinstruction);
                i += 3;
            }
            else if (Instruction[i]=="sw") {
                // while (!empty_queue()) reduce_queue();
                countOfInstructions["sw"]++;
                v = sw(i);
                if (v == vi(4,-1)) {
                    return v;
                }
                //cout << "Cycle " << cycle << ": DRAM request issued by executing ";
                //for (int j=0; j<3; j++) cout << instructions[i+j] << " ";
                //cout << endl;
                //if (!NON_BLOCKING_MODE) reduce_queue();
                v.push_back(totalInstructions-currentinstruction);
                i += 3;
                
            }
            else if (Instruction[i]=="addi") {
                v = addi(i);
                if (v == vi(4,-1)) {
                    return v;
                }
                i += 4;
                instr_sz = 4;
                countOfInstructions["addi"]++;
            }
            else i += 1;
            currentinstruction++;
            return v;
        }
    };
    int instructionsExecuted = 0;
    vector<core> cores;
    int numCores;
    int maxCycles;
    int cycles = 0;
    
    cpu() {
        cin >> numCores;
        cin >> maxCycles;
        
        for(int i = 0; i < numCores; i++) {
            core c;
            c.readFile("t" + to_string(i+1) + ".txt");
            cores.push_back(c);
        }
    }
    
    int determineResult(vi a) {
        if(a.size() < 4) {
            return -10;
        }
        if(a[2] == -1) return -1;
        if(a[2] == -2) return a[1];
        else return -3;
    }
    
    void simulate() {
        bool flag = true;
        while(cycles < maxCycles && flag) {
            cycles++;
            cout << "cycle: " << cycles << endl;
            for(int i = 0; i < numCores; i++) {
                vi result = cores[i].simulate();
                
                if(determineResult(result) == -1) {
                    continue;
                } else if (determineResult(result) == -2) {
                    instructionsExecuted++;
                    continue;
                } else if (determineResult(result) == -3) {
                    // Insert the vector to ram
                    instructionsExecuted++;
                    continue;
                } else {
                    flag = false;
                    break;
                }
            }
        }
        cout << "done" << endl;
    }
};

struct MRM {
    vector<vector<int> > dramRequests;
    //vector<vector<int> >;
    vector<vector<int> > startInsertions;
    vector<int> dram;
    vector<vector<int> > pendingRequests;

    int size = 12;
    int currentRequests = 0;

    int requestCounter = 0;
    int detectCounter = 0;
    bool flag = false;


    int requestIssued() {
        if(pendingRequests.size() > 0) {
            vector<int> v = pendingRequests[0];
            pendingRequests.erase(pendingRequests.begin());
            int count = dramRequests.size();

            if(count > requestCounter) {
                requestCounter = count;
            }
            return 0;
        }
        return -1;
    }

    // v is expected to be {register index,memory address,boolean,number of instructions,core index}
    // flag boolean is to mark that the requests that come in the same clock cycle cannot be given together to the request manager. 
    // Hence added to a queue which will be executed later on.
    int requestIssued(vector<int> v){
        if(v.size() != 5) {
            return -2;
        }

        if (flag || pendingRequests.size() > 0)
        {
            if(currentRequests >= size) {
                return -1; // request is unsuccessful as the size is full
            }
            else {
                pendingRequests.push_back(v);
                currentRequests++;
                return 0;
            }
        }
        
        int count = dramRequests.size();
        bool shouldRemove = true;
        int removalIndex = -1;
        bool insertionDone = false;
        int index = -1;

        for(int i = 0; i < count; i++) {
            if(dramRequests[i][4] == v[4]) {
                if(v[2] == 1 && dramRequests[i][2] == v[2] && dramRequests[i][0] == v[0]) {
                    dramRequests.erase(dramRequests.begin() + i--);
                }
            }
            if(!insertionDone && dramRequests[i][1] == v[1] && dramRequests[i][3] > v[3]) {
                index = i; insertionDone = true;
            } else if(!insertionDone && i >= 1 && dramRequests[i-1][1] == v[1] && dramRequests[i][1] != v[1]) {
                index = i; insertionDone = true;
            } else if (!insertionDone && dramRequests[i-1][1]/1024 == v[1]/1024 && dramRequests[i][1]/1024 != v[1]/1024) {
                index = i; insertionDone = true;
            } else if(!insertionDone && dramRequests[i-1][1]/1024 != dramRequests[i][1]/1024 && v[3] > dramRequests[i][3]) {
                index = i; insertionDone = true;
            }

            if(dramRequests)
        }
    }

    void detectAddress(int a) {
        for(int i = 0; i < dramRequests.size(); i++) {

        }
    }


    void simulate() {

    }

};

int main(int argc, char** argv) {
    /*mips m1;
    m1.readFile();
    m1.syntaxCheck();
    for(int j = 0; j < m1.Instruction[0].size(); j++) {
        cout << m1.Instruction[0][j] << "|";
    }*/
    cpu a;
    a.simulate();
}
