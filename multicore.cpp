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
        int readFile(string filename, int memory[])
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
            /*for (auto it = Instruction.begin(); it != Instruction.end(); it++)
            cout << *it << " ";*/
            return syntaxCheck(memory);
            
        }
        bool isValidRegister(string str,bool modify = false){
            // Check whether the register is valid for the given task or not
            if(str == "") {
                return true;
            }
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

        int syntaxCheck(int memory[]) {
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
                        !isValidRegister(offset(Instruction[j+2]).first)) {
                            return -1;}
                    totalInstructions++;
                    memory[PC] = j;
                    j += 3;
                }
                else if (Instruction[j] == "addi") {
                    cout << "yes" << endl;
                    if (j+3 >= n) return -1;
                    if (!isValidRegister(Instruction[j+1], true) ||
                        !isValidRegister(Instruction[j+2]) ||
                        !isNumber(Instruction[j+3])) {
                            return -1;}
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
            cout << "\t\tadd " << s1 << "," << s2 << "," << s3 << "---------->>>";
                registers[reg1]=registers[reg2]+registers[reg3];
                cout << s1 << " = " << registers[reg1];
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
            cout << "\t\tsub " << s1 << "," << s2 << "," << s3 << "---------->>>";
                registers[reg1]=registers[reg2]-registers[reg3];
                cout << s1 << " = " << registers[reg1];
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
            cout << "\t\tmul " << s1 << "," << s2 << "," << s3 << "---------->>>";
                registers[reg1]=registers[reg2]*registers[reg3];
                cout << s1 << " = " << registers[reg1];
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
            cout << "\t\taddi " << s1 << "," << s2 << "," << s3 << "---------->>>";
                registers[reg1]=registers[reg2]+reg3;
                cout << s1 << " = " << registers[reg1];
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
            cout << "\t\tbne " << s1 << "," << s2 << "," << s3;
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
            cout << "\t\tbeq " << s1 << "," << s2 << "," << s3;
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
            cout << "\t\tj " << s1;
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
            cout << "\t\tslt " << s1 << "," << s2 << "," << s3;
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
            /*if(not_safe(b,false)){
                vector<int> out(4,-1);
                return out;
            }
            else{*/
            cout << "\t\tlw " << s1 << "," << s;
                //need to add no of instructions
                involvedRegisters.insert(reg1);
                vector<int> out{reg1,addr,1};   //return {register addr,memory addr,boolean 1}
                return out;
            //}
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
            cout << "\t\tsw " << s1 << "," << s;
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
    int memory[1<<20] = {0};
    

    //Memory request manager
    struct MRM {
        vector<vector<int> > dramRequests;
        //vector<vector<int> >;
        vector<vector<int> > startInsertions;
        vector<int> dram{-1,-1,-1,-1,-1};
        vector<vector<int> > pendingRequests;

        int size = 12;
        int currentRequests = 0;

        int requestCounter = 0;
        int ramCounter = 0;
        bool flag = false;
        bool dramActive = false;
        bool shouldRightBack = false;

        // v is expected to be {register index,memory address,boolean,number of instructions,core index}
        int sendRequest(vector<int> v) {
            // this function will be called by the cpu whe the cpu wants to pass a request.
            // if returns -1, the cpu should stop the processing rightaway because the queue is full.
            // if returns -2, te vector inserted is of wrong length/ expected length should be 5.
            // 0 returned on successful insertion
            if(v.size() < 5) {
                return -2;
            }
            if(currentRequests >= size) {
                return -1;
            }
            //cout<<"request sent"<<"\n";
            pendingRequests.push_back(v); currentRequests++; return 0;
        }

        // Each clock cycle the MRM will pass the pending requests to the queue because only one instruction can be passed at a time to the vector.
        int requestIssued(){
            // This function is being called in every clock cycle b y the simulate function of mrm. It adds the next instruction to the queue.
            // returns 0 on successful insertion.
            //          ------------------------------------- THE IF CONDITIONS IN THIS FUNCTION REQUIRE VERIFICATION -----------------------------------------
            if(pendingRequests.size() == 0) {
                return 0;
            }
            dramActive = true;
            vi v = pendingRequests[0];
            pendingRequests.erase(pendingRequests.begin());
            
            int count = dramRequests.size();
            bool shouldRemove = true;
            int removalIndex = -1;
            bool insertionDone = false;
            int index = -1;

            if(dram[1] == v[1] && dramRequests[0][1] != dram[1]) {
                insertionDone = true; index = 0;
            }

            for(int i = 0; i < count; i++) {
                if(dramRequests[i][4] == v[4]) {
                    if(v[2] == 1 && dramRequests[i][2] == v[2] && dramRequests[i][0] == v[0]) {
                        removalIndex = i;
                    }
                }
                //cout << dramRequests[i].size() << endl;
                //cout << v.size() << endl;
                if(!insertionDone && dramRequests[i][1] == v[1] && dramRequests[i][3] < v[3]) {
                    index = i; insertionDone = true;
                } else if(!insertionDone && i >= 1 && dramRequests[i-1][1] == v[1] && dramRequests[i][1] != v[1]) {
                    index = i; insertionDone = true;
                } else if (!insertionDone && i >= 1 && dramRequests[i-1][1]/1024 == v[1]/1024 && dramRequests[i][1]/1024 != v[1]/1024) {
                    index = i; insertionDone = true;
                } else if(!insertionDone && i >= 1 && dramRequests[i-1][1]/1024 != dramRequests[i][1]/1024 && v[3] > dramRequests[i][3]) {
                    index = i; insertionDone = true;
                }

                if(dramRequests[i][2] == 0 && ((dramRequests[i][4] == v[4] && dramRequests[i][0] == v[0]) || dramRequests[i][1] == v[1]) ) {
                    insertionDone = false;
                    if(removalIndex != -1) {
                        shouldRemove = false;
                    }
                }
            }
            if(count > requestCounter) {
                requestCounter = count;
            }
            if(shouldRemove) {
                if(removalIndex != -1) {
                    dramRequests.erase(dramRequests.begin() + removalIndex);
                }
            }
            if(!insertionDone) {
                dramRequests.insert(dramRequests.end(), v);
            } else {
                dramRequests.insert(dramRequests.begin() + index, v);
            }
            if(shouldRemove) {
                if(dram[4] == v[4] && v[2] == 1 && dram[2] == 1 && dram[0] == v[0]) {
                    cout << "\tUpdating the dram with new request, redundant code found!" << endl;
                    currentRequests--;
                    vi next = dramRequests[0];
                    dramRequests.erase(dramRequests.begin());
                    if(dram[1] != -1 && next[1] == dram[1]) {
                        if(ramCounter <= 1) ramCounter = 1;
                    } else if (dram[1] != -1 && next[1]/1024 == dram[1]/1024) {
                        if(ramCounter < 2) {
                            ramCounter = 2;
                        }
                    } else {
                        shouldRightBack = true;
                        ramCounter = 12;
                    }
                    dram = next;
                }
            }
            return 0;
        }

        int simulate(vector<core>* cores, int memory[]) {
            // This is the function that is required to be called by the cpu every clock cycle. 
            // Loads any pending instructions on the queue. 
            // if -1 is returned, then it means that nothing is loaded on the dram yet. The cpu may or may not proceed forward.
            // if 0 is returned, it means that that the ram has completed its clock cycle successfully.
            // If a number > 0 was returned. it means that the something was written in the register of (number-1)th core and the write operations in that core should be halted for one cycle.
            int a = -1;
            if(!dramActive) {
                return 0;
            }
            if(ramCounter == 0) {
                editCore(cores, memory);
                if(dramRequests.size() == 0) {
                    dramActive = false;
                    return 0;
                }
                if(requestCounter <= 0) {
                    currentRequests--;
                    a = dram[4] + 1;
                    vi next = dramRequests[0];
                    //cout << "I am here with a: " << a << endl;
                    dramRequests.erase(dramRequests.begin());
                    if(dram[1] != -1 && next[1] == dram[1]) {
                        shouldRightBack = (dram[2] == 0 || shouldRightBack);
                        ramCounter = 1;
                    } else if (dram[1] != -1 && next[1]/1024 == dram[1]/1024) {
                        shouldRightBack = (dram[2] == 0 || shouldRightBack);
                        ramCounter = 2;
                    } else {
                        if(dram[2] == 0 || shouldRightBack) {
                            shouldRightBack = false;
                            ramCounter += 10;
                        }
                        ramCounter += 12;
                    }
                    dram = next;
                } else {
                    requestCounter--;
                    return a;
                }
            }
            if (a == -1) {
                a = 0;
            }
            ramCounter--;
            requestCounter--;
            return a;
        }

        void editCore(vector<core>* cores, int memory[]) {
            int coreIndex = dram[4];
            if(coreIndex == -1) {
                return;
            }

            if(dram[2]) {
                (*cores)[coreIndex].registers[dram[0]] = memory[dram[1]];

                (*cores)[coreIndex].involvedRegisters.erase(dram[0]);

                cout << "\tExecuted: regIndex " << dram[0] << " = " << memory[dram[1]] << endl;
            } else {
                memory[dram[1]] = dram[5];
                cout << "\tExecuted: value at memory Address " << dram[1] << " = " << dram[5];
            }
        }

    };
    //MRM is initialized in cpu by the name manager
    MRM manager;
    cpu() {
        cout<<"Enter number of CPU cores"<<"\n";
        cin >> numCores;
        cout<<"Enter Halt time"<<"\n";
        cin >> maxCycles;
        cout<<"Enter MIPS files"<<"\n";
        string files[numCores];
        
        for(int i = 0; i < numCores; i++) {
            core c;
            cin>>files[i];
            c.readFile(files[i], memory);
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
        cout << cores[0].totalInstructions << endl;
        while(cycles < maxCycles && (flag || manager.dramActive)) {
            cycles++;
            cout << "cycle: " << cycles << endl;
            int mrm_check=manager.simulate(&cores, memory);

            for(int i = 0; i < numCores; i++) {
                vi result;
                if(mrm_check<=0){
                    cout << "\tCore " << i + 1 << ": " << endl;
                    result = cores[i].simulate();
                    result.push_back(i);
                    if(result[2] == 0) {
                        result.push_back(cores[i].registers[result[0]]);
                    }
                    cout << endl;
                    //cout<<" "<<result[1]<<endl;
                }
                else{
                    cout << "\tCore " << i + 1 << ": Paused for one cycle for writing in register by DRAM" << endl;
                    continue;
                }
                if(determineResult(result) == -1) {
                    cout << "\t\t Not safe to execute instructions, waiting for DRAM to respond!" << endl;
                    continue;
                } else if (determineResult(result) == -2) {
                    instructionsExecuted++;
                    continue;
                } else if (determineResult(result) == -3) {
                    // Insert the vector to ram
                    int check,check1;
                    check=manager.sendRequest(result);
                    manager.requestIssued();
                    if(check==0){
                        cout<<"\t\tDRAM request issued"<<endl;
                    } else {
                        cout << "\t\t DRAM Full!, Cannot send the request to DRAM." << endl;
                    }
                    /*if(check==-1){
                        while(manager.sendRequest(result)!=0 && cycles<maxCycles){
                            cycles++;
                        }
                    }**/
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
