#include "../include/main.hpp"
#include <string.h>
#include <fstream>
//#include <bitset>

BYTE* loadProtectedCode(int &mcsize, std::string fileName)
{
    int codeSize;
    BYTE *mc;
    std::ifstream fileBinToRead;
    
    fileBinToRead.open(fileName, std::ios::binary);

    if(fileBinToRead.is_open())
    {
        fileBinToRead.seekg(0, fileBinToRead.end);
        codeSize = fileBinToRead.tellg();
        fileBinToRead.seekg(0, fileBinToRead.beg);
        
        if(codeSize > 51202)
        {
            fileBinToRead.close();
            throw 100011;
        }

        WORD highByte = fileBinToRead.get();
        WORD lowByte = fileBinToRead.get();

        // std::cout << std::bitset<8>(highByte) << std::endl;
        // std::cout << std::bitset<8>(lowByte) << std::endl;
        // std::cout << std::hex << highByte << std::endl;
        // std::cout << std::hex << lowByte << std::endl;

        DWORD magicNumber = (highByte << 8) | lowByte;

        if(magicNumber != MAGIC_NUMBER)
        {
            fileBinToRead.close();
            throw 100012;
        }

        fileBinToRead.seekg(0, fileBinToRead.beg);

        mc = new BYTE[codeSize];
        char byte;
        int counter = 0;

        //std::cout << "[DEBUG] *************" << std::endl;
        while(fileBinToRead.get(byte))
        {
            mc[counter++] = byte;
            //std::cout << "\t<READ> : " << static_cast<uint16_t>(mc[counter - 1]) << std::endl;
        }
        //std::cout << "[DEBUG] *************" << std::endl;
        fileBinToRead.close();
    }
    else
    {
        throw 100010;
    }

    mcsize = codeSize;
    return mc;
}