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
    
    

    
    std::cout << "Hello world!\n";
    
    
}
