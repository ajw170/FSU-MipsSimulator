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
#include <fstream>
#include <string>

const size_t MAXPROGRAM = 32768;

void printInstSummary();

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
            unsigned int encoding;
        } u;
    } instructions[MAXPROGRAM] = {0}; //accessed by instructions.u.rFormat, etc.
    
    //put program instructions into struct to allow parsing of bit field values
    for (size_t i = 0; i < numInst; ++i)
    {
        instructions[i].u.encoding = progInstructions[i];
    }
    
    printInstSummary();
    
    
    
    

    
    std::cout << "Hello world!\n";
    
}
