/*
Andrew J Wood
CDA3101 Project 3 - MIPS Simulator
September 24, 2017

This program simulates a MIPS simulator by reading an object file output by the MIPS assembler.
The simulation then outputs any valid information the standard out.
 
In addition, a log.txt file is generated contianing the parsing of each line along with the state
of the registers at the point of execution of the corresponding line.

Note that the code is self-documenting.
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>

const size_t MAXPROGRAM = 32768;

//typedef support for table mappings
typedef std::pair<const unsigned int,const std::string> codePair;
typedef std::pair<const unsigned int, const unsigned int> opcodeFunctPair;
typedef std::pair<const opcodeFunctPair, const std::string> codePairFunctTable;

void printRegisterState(std::vector<int> &, std::ofstream &);
void printDataMemory(unsigned int *, size_t, std::ofstream &);

int main(int argc, char * argv[])
{
    //determine if file name was specified; otherwise, exit.
    if (argc < 2)
    {
        std::cout << argc << "\n";
        std::cerr << " ** File name not specified. **\n";
        exit(EXIT_FAILURE);
    }
    
    //open the passed object file for reading
    std::ifstream inFile(argv[1], std::ios::in);
    inFile.seekg(std::ios::beg);
    
    if (!inFile)
    {
        std::cerr << "File could not be opened.\n";
        exit(EXIT_FAILURE);
    }
    
    size_t numInst;
    inFile >> numInst; //read number of instructions
    size_t numWords;
    inFile >> numWords; //read number of words
    
    unsigned int progInstructions[MAXPROGRAM] = {0};
    unsigned int dataArray[MAXPROGRAM] = {0};
    
    //read in program instructions
    for (size_t i = 0; i < numInst; ++i)
    {
        inFile >> std::hex >> progInstructions[i];
    }
    
    //read dataArray
    for (size_t i = 0; i < numWords; ++i)
    {
        inFile >> std::hex >> dataArray[i];
    }
    
    //Done with file, close
    inFile.close();
    
    //create struct to decode instructions
    struct {
        union   {
            struct {
                unsigned int funct:6;
                unsigned int shamt:5;
                unsigned int rd:5;
                unsigned int rt:5;
                unsigned int rs:5;
                unsigned int opcode:6;
            } rFormat;
            struct  {
                unsigned int imm:16;
                unsigned int rt:5;
                unsigned int rs:5;
                unsigned int opcode:6;
            } iFormat;
            struct  {
                unsigned int address:26;
                unsigned int opcode:6;
            } jFormat;
            struct  {
                unsigned int address:26;
                unsigned int opcode:6;
            } opcodeCheck;
            unsigned int encoding;
        } u;
    } instructions[MAXPROGRAM] = {0}; //accessed by instructions.u.rFormat, etc.
    
    //put program instructions into struct to allow parsing of bit field values
    for (size_t i = 0; i < numInst; ++i)
    {
        instructions[i].u.encoding = progInstructions[i];
    }
    
    //create table of argument values mapped to corresponding string
    std::map <const unsigned int, const std::string> argTable;
    argTable.insert(codePair(0,"zero")); argTable.insert(codePair(1,"at")); argTable.insert(codePair(2,"v0"));
    argTable.insert(codePair(3,"v1")); argTable.insert(codePair(4,"a0")); argTable.insert(codePair(5,"a1"));
    argTable.insert(codePair(6,"a2")); argTable.insert(codePair(7,"a3")); argTable.insert(codePair(8,"t0"));
    argTable.insert(codePair(9,"t1")); argTable.insert(codePair(10,"t2")); argTable.insert(codePair(11,"t3"));
    argTable.insert(codePair(12,"t4")); argTable.insert(codePair(13,"t5")); argTable.insert(codePair(14,"t6"));
    argTable.insert(codePair(15,"t7")); argTable.insert(codePair(16,"s0")); argTable.insert(codePair(17,"s1"));
    argTable.insert(codePair(18,"s2")); argTable.insert(codePair(19,"s3")); argTable.insert(codePair(20,"s4"));
    argTable.insert(codePair(21,"s5")); argTable.insert(codePair(22,"s6")); argTable.insert(codePair(23,"s7"));
    argTable.insert(codePair(24,"t8")); argTable.insert(codePair(25,"t9")); argTable.insert(codePair(26,"k0"));
    argTable.insert(codePair(27,"k1")); argTable.insert(codePair(28,"gp")); argTable.insert(codePair(29,"sp"));
    argTable.insert(codePair(30,"fp")); argTable.insert(codePair(31,"ra"));
    
    //create table of opcode values mapped to corresponding string.
    std::map <const opcodeFunctPair, const std::string> opcodeFunctTable;
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(9,0),"addiu"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(0,33),"addu"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(0,36),"and"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(4,0),"beq"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(5,0),"bne"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(0,26),"div"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(2,0),"j"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(35,0),"lw"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(0,16),"mfhi"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(0,18),"mflo"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(0,24),"mult"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(0,37),"or"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(0,42),"slt"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(0,35),"subu"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(43,0),"sw"));
    opcodeFunctTable.insert(codePairFunctTable(opcodeFunctPair(0,12),"syscall"));
    
    /*
    //Test harness only
    std::cout << "Test of opcodeFunctTable:\n";
    std::cout << "Finding (0,0) in table: " << opcodeFunctTable.count(opcodeFunctPair(0,0)) << "\n";
    std::cout << "Finding (0,1) in table: " << opcodeFunctTable.count(opcodeFunctPair(0,1)) << "\n";
    std::cout << "Finding (1,2) in table: " << opcodeFunctTable.count(opcodeFunctPair(1,2)) << "\n";
    std::cout << "Value of (0,0) is: " << opcodeFunctTable[opcodeFunctPair(0,0)];
    */
    
    /* Part 1 - Instruction reading and parsing */
    
    //prepare text output file
    std::ofstream outFile("log_test.txt",std::ios::out);
    outFile.seekp(std::ios::beg);
    
    //prepare instruction storage vector
    std::vector<std::string> instStorage (numInst,"");
    
    outFile << "insts:\n";
    
    //print instructions
    for (size_t i = 0; i < numInst; ++i)
    {
        //determine opcodeFunct pair
        unsigned int opcode = instructions[i].u.opcodeCheck.opcode;
        unsigned int funct = 0;  //funct will be 0 unless opcode is 0
        
        //determine if the instsruction is an "R" instruction, then determine if the funct pair is valid
        //provided it is an "R" instruction.
        if (opcode == 0)
        {
            funct = instructions[i].u.rFormat.funct;
            size_t validOpcodeFunct = opcodeFunctTable.count(opcodeFunctPair(0,funct));
            if (!validOpcodeFunct)
            {
                std::cerr << "could not find inst with opcode " << opcode << " and funct " << funct;
                outFile.close();
                exit(EXIT_FAILURE);
            }
        }
        
        outFile << std::setw(4) << std::right;
        outFile << i << ": ";
    
        
        std::string instString; //string to store instructions
        instString += opcodeFunctTable[opcodeFunctPair(opcode,funct)];
        
        //determine sequence to ouput
        //separate by opcode using switch statement
        switch(opcode)
        {
            case 0:
                switch(funct)
                {
                    case 33:
                    case 36:
                    case 37:
                    case 42:
                    case 35:
                        instString += "\t";
                        instString += "$";
                        instString += argTable[instructions[i].u.rFormat.rd];
                        instString += ",$";
                        instString += argTable[instructions[i].u.rFormat.rs];
                        instString += ",$";
                        instString += argTable[instructions[i].u.rFormat.rt];
                        //add this string to the vector
                        instStorage[i] += instString;
                        break;
                    case 26:
                    case 24:
                        instString += "\t";
                        instString += "$";
                        instString += argTable[instructions[i].u.rFormat.rs];
                        instString += ",$";
                        instString += argTable[instructions[i].u.rFormat.rt];
                        //add this string to the vector
                        instStorage[i] += instString;
                        break;
                    case 12: //syscall case
                        instStorage[i] += instString;
                        break;
                    case 16:
                    case 18:
                        instString += "\t";
                        instString += "$";
                        instString += argTable[instructions[i].u.rFormat.rd];
                        instStorage[i] += instString;
                        break;
                    default:
                        break;
                }
                break; //opcode 0
            case 9:
                instString += "\t";
                instString += "$";
                instString += argTable[instructions[i].u.iFormat.rt];
                instString += ",$";
                instString += argTable[instructions[i].u.iFormat.rs];
                instString += ",";
                instString += std::to_string(instructions[i].u.iFormat.imm);
                instStorage[i] += instString;
                break;
            case 4:
            case 5:
                instString += "\t";
                instString += "$";
                instString += argTable[instructions[i].u.iFormat.rs];
                instString += ",$";
                instString += argTable[instructions[i].u.iFormat.rt];
                instString += ",";
                instString += std::to_string(static_cast<short>((instructions[i].u.iFormat.imm)));
                instStorage[i] += instString;
                break;
            case 2:
                instString += " ";
                instString += std::to_string(instructions[i].u.jFormat.address);
                instStorage[i] += instString;
                break;
            case 35:
            case 43:
                instString += "\t";
                instString += "$";
                instString += argTable[instructions[i].u.iFormat.rt];
                instString += ",";
                instString += std::to_string(instructions[i].u.iFormat.imm);
                instString += "($";
                instString += argTable[instructions[i].u.iFormat.rs];
                instString += ")";
                instStorage[i] += instString;
                break;
            default:
                std::cerr << "Invalid opcode / funct cominbation";
                exit(EXIT_FAILURE);
                break;
        }
        outFile << instStorage[i];
        outFile << "\n";
    } //end of instruction storage section
    
    //print data
    outFile << "\ndata:\n";
    
    for (size_t i = 0; i < numWords; ++i)
    {
        outFile << std::setw(4) << std::right;
        outFile << (i+numInst) << ": " << dataArray[i] << "\n";
    } //end of data display section
    
    outFile << "\n";
    
    
    /* Part 2 - MIPS Simulator with logged output */
    
    //prepare program counter
    int progCounter = 0;

    //create vector to store 32 register values plus "lo" and "hi" (34 total) - intialize all to zero.
    std::vector<int> registerStore(34,0);
    
    //initialize $gp to beginning of data area
    registerStore[28] = static_cast<int>(numInst);
    
    bool exitCondition = (progCounter >= numInst);
    
    while (!exitCondition)
    {
        outFile << "PC: " << progCounter << "\n";
        
        unsigned int opcode = instructions[progCounter].u.opcodeCheck.opcode;
        unsigned int funct = 0;
        unsigned int rd;
        unsigned int rs;
        unsigned int rt;
        unsigned short imm;
        unsigned int address;
        short immSigned; //used to determine validity of branch
        int addressSigned; //used to determine validity of jump (protect against negative)
        int addrLoadStore; //used to faciliate loading and storing of words
        
        struct {
            union {
                struct {
                    long lower:32;
                    long upper:32;
                } divLong;
                struct {
                    long full;
                } fullLong;
            } u;
        } longAssist;
    
        if (opcode == 0)
            funct = instructions[progCounter].u.rFormat.funct;
        
        switch(opcode)
        {
            case 0:
                switch(funct)
                {
                    case 33: //addu instruction
                        outFile << "inst: " << instStorage[progCounter] << "\n";
                        rd = instructions[progCounter].u.rFormat.rd;
                        if (rd != 0) //prevent the $zero register from being written to
                        {
                            rs = instructions[progCounter].u.rFormat.rs;
                            rt = instructions[progCounter].u.rFormat.rt;
                            registerStore[rd] = registerStore[rs] + registerStore[rt];
                        }
                        ++progCounter;
                        break;
                    
                    case 36: //and instruction
                        outFile << "inst: " << instStorage[progCounter] << "\n";
                        rd = instructions[progCounter].u.rFormat.rd;
                        if (rd != 0) //prevent the $zero register from being written to
                        {
                            rs = instructions[progCounter].u.rFormat.rs;
                            rt = instructions[progCounter].u.rFormat.rt;
                            registerStore[rd] = registerStore[rs] & registerStore[rt];
                        }
                        ++progCounter;
                        break;

                    case 37: //or instruction
                        outFile << "inst: " << instStorage[progCounter] << "\n";
                        rd = instructions[progCounter].u.rFormat.rd;
                        if (rd != 0) //prevent the $zero register from being written to
                        {
                            rs = instructions[progCounter].u.rFormat.rs;
                            rt = instructions[progCounter].u.rFormat.rt;
                            registerStore[rd] = registerStore[rs] | registerStore[rt];
                        }
                        ++progCounter;
                        break;
                        
                    case 42: //slt instruction
                        outFile << "inst: " << instStorage[progCounter] << "\n";
                        rd = instructions[progCounter].u.rFormat.rd;
                        if (rd != 0) //prevent the $zero register from being written to
                        {
                            rs = instructions[progCounter].u.rFormat.rs;
                            rt = instructions[progCounter].u.rFormat.rt;
                            registerStore[rd] = (registerStore[rs] < registerStore[rt]);
                        }
                        ++progCounter;
                        break;
                        
                    case 35: //subu instruction
                        outFile << "inst: " << instStorage[progCounter] << "\n";
                        rd = instructions[progCounter].u.rFormat.rd;
                        if (rd != 0) //prevent the $zero register from being written to
                        {
                            rs = instructions[progCounter].u.rFormat.rs;
                            rt = instructions[progCounter].u.rFormat.rt;
                            registerStore[rd] = registerStore[rs] - registerStore[rt];
                        }
                        ++progCounter;
                        break;
                        
                    case 26: //div instruction
                        outFile << "inst: " << instStorage[progCounter] << "\n";
                        rt = instructions[progCounter].u.rFormat.rt;
                        if (registerStore[rt] == 0) //prevent divide by zero
                        {
                            std::cerr << "divide by zero for instruction at " << progCounter;
                            exit(EXIT_FAILURE);
                        }
                        rs = instructions[progCounter].u.rFormat.rs;
                        registerStore[32] = registerStore[rs] / registerStore[rt]; //put quotient in lo
                        registerStore[33] = registerStore[rs] % registerStore[rt]; //put remainder in hi
                        ++progCounter;
                        break;
                        
                    case 24: // mult instruction
                        outFile << "inst: " << instStorage[progCounter] << "\n";
                        rt = instructions[progCounter].u.rFormat.rt;
                        rs = instructions[progCounter].u.rFormat.rs;
                        longAssist.u.fullLong.full = registerStore[rs] * registerStore[rt];
                        registerStore[32] = longAssist.u.divLong.lower; //put low order word in lo
                        registerStore[33] = longAssist.u.divLong.upper; //put high order word in hi
                        ++progCounter;
                        break;
                        
                    case 12: //syscall case - v0 is either 1, 5; otherwise ignore and exit condition is tested later
                        outFile << "inst: " << instStorage[progCounter] << "\n";
                        if (registerStore[2] == 1)
                        {
                            std::cout << registerStore[4] << "\n";
                        }
                        if (registerStore[2] == 5)
                        {
                            std::cin >> registerStore[2];
                        }
                        ++progCounter;
                        break;
                        
                    case 16: //mfhi instruction - move hi register to rd
                        outFile << "inst: " << instStorage[progCounter] << "\n";
                        rd = instructions[progCounter].u.rFormat.rd;
                        if (rd != 0) //prevent the zero register from being written to
                        {
                            registerStore[rd] = registerStore[33];
                        }
                        ++progCounter;
                        break;
                
                    case 18: //mflo instuction - move lo register to rd
                        outFile << "inst: " << instStorage[progCounter] << "\n";
                        rd = instructions[progCounter].u.rFormat.rd;
                        if (rd != 0) //prevent the zero register from being written to
                        {
                            registerStore[rd] = registerStore[32];
                        }
                        ++progCounter;
                        break;
                        
                    default: //the program should never reach this point
                        std::cerr << "Something was wrong.";
                        outFile.close();
                        exit(EXIT_FAILURE);
                        break;
                }
                break; //case if opcode 0
                
            case 9: //addiu instruction
                outFile << "inst: " << instStorage[progCounter] << "\n";
                rt = instructions[progCounter].u.iFormat.rt;
                if (rt != 0) //prevent the zero register from being written to
                {
                    rs = instructions[progCounter].u.iFormat.rs;
                    imm = instructions[progCounter].u.iFormat.imm;
                    registerStore[rt] = registerStore[rs] + imm;
                }
                ++progCounter;
                break;
                
            case 4: //beq instruction
                rs = instructions[progCounter].u.iFormat.rs;
                rt = instructions[progCounter].u.iFormat.rt;
                imm = instructions[progCounter].u.iFormat.imm;
                immSigned = static_cast<short>(imm);
                if (registerStore[rs] == registerStore[rt])
                {
                    //check if the branch would result in an invalid PC target
                    if ( ((progCounter + immSigned) > (numInst - 1)) || ((progCounter + immSigned) < 0)  )
                    {
                        //check if PC is accessing data memory
                        if ((progCounter + immSigned) <= (numInst + numWords - 1))
                        {
                            std::cerr << "PC is accessing data memory at address " << (progCounter + immSigned);
                        }
                        else
                        {
                            std::cerr << "PC is accessing illegal memory address " << (progCounter + immSigned);
                        }
                        outFile.close();
                        exit(EXIT_FAILURE);
                    }
                    outFile << "inst: " << instStorage[progCounter] << "\n";
                    progCounter += immSigned;
                }
                else
                {
                    outFile << "inst: " << instStorage[progCounter] << "\n";
                    ++progCounter;
                }
                break;
                
            case 5: //bne instruction
                rs = instructions[progCounter].u.iFormat.rs;
                rt = instructions[progCounter].u.iFormat.rt;
                imm = instructions[progCounter].u.iFormat.imm;
                immSigned = static_cast<short>(imm);
                if (registerStore[rs] != registerStore[rt])
                {
                    //check if the branch would result in an invalid PC target
                    if ( ((progCounter + immSigned) > (numInst - 1)) || ((progCounter + immSigned) < 0)  )
                    {
                        //check if PC is accessing data memory
                        if ((progCounter + immSigned) <= (numInst + numWords - 1))
                        {
                            std::cerr << "PC is accessing data memory at address " << (progCounter + immSigned);
                        }
                        else
                        {
                            std::cerr << "PC is accessing illegal memory address " << (progCounter + immSigned);
                        }
                        outFile.close();
                        exit(EXIT_FAILURE);
                    }
                    outFile << "inst: " << instStorage[progCounter] << "\n";
                    progCounter += immSigned;
                }
                else
                {
                    outFile << "inst: " << instStorage[progCounter] << "\n";
                    ++progCounter;
                }
                break;
                
            case 2: //j instruction
                address = instructions[progCounter].u.jFormat.address;
                addressSigned = static_cast<int>(address);
                //check if jump would result in an invalid PC target
                if ( (addressSigned > (numInst - 1)) || (addressSigned < 0) )
                {
                    //check if PC is accessing data memory
                    if ( address < (numInst + numWords) )
                    {
                        std::cerr << "PC is accessing data memory at address " << addressSigned;
                    }
                    else
                    {
                        std::cerr << "PC is accessing illegal memory address " << addressSigned;
                    }
                    outFile.close();
                    exit(EXIT_FAILURE);
                }
                outFile << "inst: " << instStorage[progCounter] << "\n";
                progCounter = addressSigned;
                break;
    
            case 35: //lw instruction
                outFile << "inst: " << instStorage[progCounter] << "\n";
                imm = instructions[progCounter].u.iFormat.imm;
                immSigned = static_cast<short>(imm);
                rt = instructions[progCounter].u.iFormat.rt; //destination
                rs = instructions[progCounter].u.iFormat.rs; //the base register
                
                addrLoadStore = registerStore[rs] + immSigned;
                //first check to ensure load address is valid
                if ((addrLoadStore < (numInst)) || (addrLoadStore >= (numInst + numWords)))
                {
                    std::cerr << "Load outside of data memory at address " << addrLoadStore;
                    outFile.close();
                    exit(EXIT_FAILURE);
                }
                if (rt != 0) //prevent zero register from being written to
                {
                    registerStore[rt] = dataArray[addrLoadStore - numInst];
                }
                ++progCounter;
                break;
                
            case 43: //sw instruction  sw $s0,0($gp)
                outFile << "inst: " << instStorage[progCounter] << "\n";
                imm = instructions[progCounter].u.iFormat.imm;
                immSigned = static_cast<short>(imm);
                rt = instructions[progCounter].u.iFormat.rt; //where word is to be taken from
                rs = instructions[progCounter].u.iFormat.rs; //the base register
                
                addrLoadStore = registerStore[rs] + immSigned;
                //first check to ensure load address is valid
                if ((addrLoadStore < (numInst)) || (addrLoadStore >= (numInst + numWords)))
                {
                    std::cerr << "Store outside of data memory at address " << addrLoadStore;
                    outFile.close();
                    exit(EXIT_FAILURE);
                }
                dataArray[addrLoadStore - numInst] = registerStore[rt];
                ++progCounter;
                break;

            default: //the program should never reach this point
                std::cerr << "Something went wrong.";
                outFile.close();
                exit(EXIT_FAILURE);
                break;
        } //end switch (opcode)
        
        //exit the program when one of the two conditions is true:
        //(1) a syscall with value 10 in the v0 register is encountered
        //(2) the program counter goes past the last valid instruction
        int vzero;
        vzero = registerStore[2];
        exitCondition = ((opcode == 0 && funct == 12 && vzero == 10) || (progCounter >= numInst));
        
        if (!exitCondition)
        {
            printRegisterState(registerStore,outFile);
            outFile << "\n\n";
            printDataMemory(dataArray,numWords,outFile);
            outFile << "\n\n";
        }

    } //end while (!exitCondition)
    
    outFile << "exiting simulator";
    outFile.close();
}

void printRegisterState(std::vector<int> & registerStore, std::ofstream & outFile)
{
    outFile << std::left << "\nregs:\n";
    outFile << std::right;
    outFile << std::setw(10) << "$zero =" << std::setw(6) << registerStore[0];
    outFile << std::setw(10) << "$at =" << std::setw(6) << registerStore[1];
    outFile << std::setw(10) << "$v0 =" << std::setw(6) << registerStore[2];
    outFile << std::setw(10) << "$v1 =" << std::setw(6) << registerStore[3] << "\n";
    outFile << std::setw(10) << "$a0 =" << std::setw(6) << registerStore[4];
    outFile << std::setw(10) << "$a1 =" << std::setw(6) << registerStore[5];
    outFile << std::setw(10) << "$a2 =" << std::setw(6) << registerStore[6];
    outFile << std::setw(10) << "$a3 =" << std::setw(6) << registerStore[7] << "\n";
    outFile << std::setw(10) << "$t0 =" << std::setw(6) << registerStore[8];
    outFile << std::setw(10) << "$t1 =" << std::setw(6) << registerStore[9];
    outFile << std::setw(10) << "$t2 =" << std::setw(6) << registerStore[10];
    outFile << std::setw(10) << "$t3 =" << std::setw(6) << registerStore[11] << "\n";
    outFile << std::setw(10) << "$t4 =" << std::setw(6) << registerStore[12];
    outFile << std::setw(10) << "$t5 =" << std::setw(6) << registerStore[13];
    outFile << std::setw(10) << "$t6 =" << std::setw(6) << registerStore[14];
    outFile << std::setw(10) << "$t7 =" << std::setw(6) << registerStore[15] << "\n";
    outFile << std::setw(10) << "$s0 =" << std::setw(6) << registerStore[16];
    outFile << std::setw(10) << "$s1 =" << std::setw(6) << registerStore[17];
    outFile << std::setw(10) << "$s2 =" << std::setw(6) << registerStore[18];
    outFile << std::setw(10) << "$s3 =" << std::setw(6) << registerStore[19] << "\n";
    outFile << std::setw(10) << "$s4 =" << std::setw(6) << registerStore[20];
    outFile << std::setw(10) << "$s5 =" << std::setw(6) << registerStore[21];
    outFile << std::setw(10) << "$s6 =" << std::setw(6) << registerStore[22];
    outFile << std::setw(10) << "$s7 =" << std::setw(6) << registerStore[23] << "\n";
    outFile << std::setw(10) << "$t8 =" << std::setw(6) << registerStore[24];
    outFile << std::setw(10) << "$t9 =" << std::setw(6) << registerStore[25];
    outFile << std::setw(10) << "$k0 =" << std::setw(6) << registerStore[26];
    outFile << std::setw(10) << "$k1 =" << std::setw(6) << registerStore[27] << "\n";
    outFile << std::setw(10) << "$gp =" << std::setw(6) << registerStore[28];
    outFile << std::setw(10) << "$sp =" << std::setw(6) << registerStore[29];
    outFile << std::setw(10) << "$fp =" << std::setw(6) << registerStore[30];
    outFile << std::setw(10) << "$ra =" << std::setw(6) << registerStore[31] << "\n";
    outFile << std::setw(10) << "$lo =" << std::setw(6) << registerStore[32];
    outFile << std::setw(10) << "$hi =" << std::setw(6) << registerStore[33];
}

void printDataMemory(unsigned int * dataArray, size_t numWords, std::ofstream & outFile)
{
    outFile << std::left << "data memory:\n";
    outFile << std::right;
    
    for (size_t i = 0; i < numWords; ++i)
    {
        outFile << std::setw(8) << "data[";
        outFile << std::setw(3) << i;
        outFile << "] =";
        outFile << std::setw(6) << dataArray[i];
        outFile << "\n";
    }
}




















