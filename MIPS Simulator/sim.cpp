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
#include <map>

const size_t MAXPROGRAM = 32768;

//typedef support for table mappings
typedef std::pair<const unsigned int,const std::string> codePair;
typedef std::pair<const unsigned int, const unsigned int> opcodeFunctPair;
typedef std::pair<const opcodeFunctPair, const std::string> codePairFunctTable;

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
    
    
    //prepare text output file
    std::ofstream outFile("log_test.txt",std::ios::out);
    outFile.seekp(std::ios::beg);
    
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
        outFile << i << ": " << opcodeFunctTable[opcodeFunctPair(opcode,funct)] << "\n";
    }
    
    //print data
    outFile << "\ndata:\n";
    
    for (size_t i = 0; i < numWords; ++i)
    {
        outFile << std::setw(4) << std::right;
        outFile << (i+numInst) << ": " << dataArray[i] << "\n";
    }
    
    outFile.close();

    std::cout << "Hello world!\n";
    
}
