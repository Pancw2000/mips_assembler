#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <bitset>
using namespace std;


//pass1
//pass2


//不管注释
//已经读入一条指令
map<string,int> label_addr_table; //在第一遍遍历指令时，创建addr_table
map<string,string> R_func_map = {
    {"add","100000"},
    {"addu","100001"},
    {"sub","100010"},
    {"subu","100011"},
    {"slt","101010"},
    {"sltu","101011"},
    {"and","100100"},
    {"or","100101"},
    {"xor","100110"},
    {"sll","000000"},
    {"srl","000010"},
    {"sra","000011"},
    {"mult","011000"},
    {"multu","011001"},
    {"div","011010"},
    {"divu","011011"},
    {"jr","001000"},
    {"jalr","001001"}
};

map<string,string> opcode_map = {
    {"R","000000"},
    {"lw","100011"},
    {"addi","001000"},
    {"addiu","001001"},
    {"andi","001100"},
    {"ori","001101"},
    {"xori","001110"},
    {"lui","001111"},
    {"slti","001010"},
    {"sltiu","001011"},

    {"beq","000100"},
    {"bne","000101"},
    {"blez","000110"},
    {"bgtz","000111"},
    {"bltz","000001"},
    {"bgez","000001"},

    {"j","000010"},
    {"jal","000011"},
    {"jalr","001001"},
    {"jr","001000"}
};

map<string,string> reg_map = {
    {"$zero","00000"},
    {"$at","00001"},
    {"$v0","00010"},
    {"$v1","00011"},
    {"$a0","00100"},
    {"$a1","00101"},
    {"$a2","00110"},
    {"$a3","00111"}
    //todo
};

class addr_32bit{
    public:
        addr_32bit(){
            v1 =  vector<bool>(32,false);
        }
        string get(int begin,int end){
            string tmp_string;
            for(int i = begin;i>end;--i){
                tmp_string.append(v1[begin]?"1" : "0");
            }
        }

    private:
        vector<bool> v1;
};

enum opcode_string{
    Elw,
    Eadd,
    Eaddi
//todo
};

opcode_string hash_opcode(string const& opcode){
    if(opcode == "lw") return Elw;
    if(opcode == "add") return Eadd;
    if(opcode == "addi") return Eaddi;
//todo
}

//读入指令，写入out.txt
void assemble(const string& instruction, addr_32bit& cur_PC,ofstream& outfile){
   
    //parse the instruction
    string inst_delimiters = " ,\n\t\r";
    vector<string> tokens; //4 tokens utmost
    
    //multi delims
    int start = 0;
    int end = instruction.find_first_of(inst_delimiters,start);
    string new_token;
    while(end != string::npos){
            new_token = instruction.substr(start,end-start);
            tokens.push_back(new_token);
            start = end + 1;
            end = instruction.find_first_of(inst_delimiters,start);
    }
    new_token = instruction.substr(start);
    tokens.push_back(new_token);
    
    //up to 4 opcodes  1. opcode+rs+rt+rd 2. opcode+rs+rt+imm(addi) ... 3.opcode+rs+(rt+imm) (lw)
    //5($rs)这种类型还需要进一步parse 
    //需要使用label的跳转指令: beq j jal

    string out_bitstring;//out_bitstring stores binary form, later stored in outFile            
    out_bitstring.clear();
    string rs,rt,rd,imm16,shamt,func;
    switch (hash_opcode(tokens[0]))
    { 
    //load options    
    /*
        lw $s0,16($t1) tokens[2] = 16($t1)
        opcode rs,rt,imm16
    */
        case Elw:
        {
            rt = tokens[1];
            int paran_pos_0 = tokens[2].find('(',0);
            int paran_pos_1 = tokens[2].find(')',paran_pos_0+1);
            imm16 = tokens[2].substr(0,paran_pos_0);
            rs = tokens[2].substr(paran_pos_0+1,paran_pos_1-paran_pos_0-1);
            out_bitstring = opcode_map[tokens[0]] + reg_map[rs] + reg_map[rt] + (bitset<16>(stoi(imm16))).to_string();
            break;
        }

        case Eadd:
        {
            rt = tokens[1];
            rs = tokens[2];
            rd = tokens[3];
            out_bitstring = opcode_map["R"] + reg_map[rs] + reg_map[rd] + reg_map[rt] + "00000"+R_func_map[tokens[0]];
            break;
        }

        case Eaddi:
        {
            rd = tokens[1];
            rs = tokens[2];
            imm16 = tokens[3];
            out_bitstring = opcode_map[tokens[0]] + reg_map[rs] + reg_map[rd] + bitset<16>(stoi(imm16)).to_string();
            break;
        }
        /*
        case "lb":
            string offset = strtok(op3.")(");
            string rs = strtok(NULL,")(");
            tmp_string = "100000" + reg_map(rs) + reg_map(op2) + bitset<16>(stoi(offset)).to_string();
            break;
        case "lbu":
            string offset = strtok(op3.")(");
            string rs = strtok(NULL,")(");
            tmp_string = "100100" + reg_map(rs) + reg_map(op2) + bitset<16>(stoi(offset)).to_string();
            break;
        case "lh":
            string offset = strtok(op3.")(");
            string rs = strtok(NULL,")(");
            tmp_string = "100001" + reg_map(rs) + reg_map(op2) + bitset<16>(stoi(offset)).to_string();
            break;
        case "lhu":
            string offset = strtok(op3.")(");
            string rs = strtok(NULL,")(");
            tmp_string = "100101" + reg_map(rs) + reg_map(op2),bitset<16>(stoi(offset)).to_string();
            break;
    //save options
        case "sw":
            string offset = strtok(op3.")(");
            string rs = strtok(NULL,")(");
            tmp_string = "101011" + reg_map(rs) + reg_map(op2) + bitset<16>(stoi(offset)).to_string();
            break;
        case "sb":
            string offset = strtok(op3.")(");
            string rs = strtok(NULL,")(");
            tmp_string = "101000" + reg_map(rs) + reg_map(op2) + bitset<16>(stoi(offset)).to_string();
            break;
        case "sh":
            string offset = strtok(op3.")(");
            string rs = strtok(NULL,")(");
            tmp_string = "101001" + reg_map(rs) + reg_map(op2) + bitset<16>(stoi(offset)).to_string();
            break;
        //ALU-R, regular type
        case "add":
            ALU_Rtype_Write(op2,op3,op4,R_func_map["add"],tmp_string);
            break;
        case "addu":
            ALU_Rtype_Write(op2,op3,op4,R_func_map["addu"],tmp_string);
            break;
        case "sub":
            ALU_Rtype_Write(op2,op3,op4,R_func_map["sub"],tmp_string);
            break;
        case "subu":
            ALU_Rtype_Write(op2,op3,op4,R_func_map["subu"],tmp_string);
            break;
        case "slt":
            ALU_Rtype_Write(op2,op3,op4,R_func_map["slt"],tmp_string);
            break;
        case "sltu":
            ALU_Rtype_Write(op2,op3,op4,R_func_map["sltu"],tmp_string);
            break;
        case "and":
            ALU_Rtype_Write(op2,op3,op4,R_func_map["and"],tmp_string);
            break;
        case "or":
            ALU_Rtype_Write(op2,op3,op4,R_func_map["or"],tmp_string);
            break;
        case "xor":
            ALU_Rtype_Write(op2,op3,op4,R_func_map["xor"],tmp_string);
            break;
        case "nor":
            ALU_Rtype_Write(op2,op3,op4,R_func_map["nor"],tmp_string);
            break;
        //sll srl irregular
        case "sll": //rs unknown
            tmp_string = "000000" + "00000" + reg_map(op3) + reg_map(op2) + bitset<5>(stoi(op4)).to_string() + R_func_map("sll");
            break;
        case "srl":
            tmp_string = "000000" + "00000" + reg_map(op3) + reg_map(op2) + bitset<5>(stoi(op4)).to_string() + R_func_map("srl");
            break;
        case "sra":
            tmp_string = "000000" + "00000" + reg_map(op3) + reg_map(op2) + bitset<5>(stoi(op4)).to_string() + R_func_map("sra");
            break;

        //mult&div
        case "mult": //mult $rs,$rt
            tmp_string = "000000" + reg_map(op2) + reg_map(op3) + "00000" + "00000" + R_func_map("mult");
            break;

        case "multu": //multu $rs,$rt
            tmp_string = "000000" + reg_map(op2) + reg_map(op3) + "00000" + "00000" + R_func_map("multu");
            break;

        case "div": //div $rs,$rt
            tmp_string = "000000" + reg_map(op2) + reg_map(op3) + "00000" + "00000" + R_func_map("div");
            break;

        case "divu": //divu $rs,$rt
            tmp_string = "000000" + reg_map(op2) + reg_map(op3) + "00000" + "00000" + R_func_map("divu");
            break;        



        case "addi": //addi $rt,$rs,imm16
            ALU_Itype_Write(opcode_map["addi"],op2,op3,op4,tmp_string);
            break;
        case "addiu":
            ALU_Itype_Write(opcode_map["addiu"],op2,op3,op4,tmp_string);
            break;
        case "andi":
            ALU_Itype_Write(opcode_map["andi"],op2,op3,op4,tmp_string);
            break;
        case "ori":
            ALU_Itype_Write(opcode_map["ori"],op2,op3,op4,tmp_string);
            break;
        case "xori":
            ALU_Itype_Write(opcode_map["xori"],op2,op3,op4,tmp_string);
            break;
        case "lui":
            ALU_Itype_Write(opcode_map["lui"],op2,op3,op4,tmp_string);
            break;
        case "slti":
            ALU_Itype_Write(opcode_map["slti"],op2,op3,op4,tmp_string);
            break;
        case "sltiu":
            ALU_Itype_Write(opcode_map["sltiu"],op2,op3,op4,tmp_string);
            break;


        case "beq": //beq $rs,$rt,imm16 differ from normal ALU-I type!
            tmp_string = opcode_map("beq") + reg_map(op2) + reg_map(op3) + bitset<16>(stoi(op4)).to_string();
            break;
        case "bne": 
            tmp_string = opcode_map("bne") + reg_map(op2) + reg_map(op3) + bitset<16>(stoi(op4)).to_string();
            break;
        case "blez": 
            tmp_string = opcode_map("blez") + reg_map(op2) + reg_map(op3) + bitset<16>(stoi(op4)).to_string();
            break;  
        case "bgtz": 
            tmp_string = opcode_map("bgtz") + reg_map(op2) + reg_map(op3) + bitset<16>(stoi(op4)).to_string();
            break;
        case "bltz": 
            tmp_string = opcode_map("bltz") + reg_map(op2) + reg_map(op3) + bitset<16>(stoi(op4)).to_string();
            break;
        case "bgez": 
            tmp_string = opcode_map("bgez") + reg_map(op2) + reg_map(op3) + bitset<16>(stoi(op4)).to_string();
            break;

// able to jump into any address in 256MB MEM
        case "j":
            addr_32bit jump_addr = label_addr_table[op2];
            tmp_string = opcode_map("j") + jump_addr.get(27,2);
            break;
        case "jal":
            break;
        case "jalr":
            tmp_string = "000000" + reg_map(op2) + "00000" + "00000" + R_func_map("jalr");
            break;
        case "jr": //jr $s
            tmp_string = "000000" + reg_map(op2) + "00000" + "00000" + R_func_map("jr");
            break;

    default:
        break;
    }
    */
    }
    cout<<out_bitstring<<endl;
    outfile << out_bitstring <<endl;
}

/*需要增加: PC+4 add overload op'+'
    PC: 000000000001010000.... 
  位数: 31 30 29..
*/




void ALU_Rtype_Write(string rs,string rt, string rd, string func, string res){
    res = "000000" + reg_map[rs] + reg_map[rt] + reg_map[rd] + func;
}

//注意 imm的多种表示方法: 0x23(hex), 12 , 目前将imm视为纯数字10进制
// void ALU_Itype_Write(string opcode, string rt, string rs, string imm, string res){
//     res = opcode_map(opcode) + reg_map(rs) + reg_map(rt) + bitset<16>(stoi(imm)).to_string();
// }


// in.txt out.txt
//for testing no need for cmd compile + arg, use IDE instead
int main(){
    ifstream infile("in.txt");
    ofstream outfile("out.txt");
    addr_32bit cur_PC;
    string inst;

    int line_number = 0;
    if(!infile){
        cout<< "file input error";
        //error
    }
    else{
        while(getline(infile,inst)){
            cout<< line_number++;
            assemble(inst, cur_PC, outfile);
        }
    }
    infile.close();
    outfile.close();
}