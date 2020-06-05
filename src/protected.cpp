#include "../include/main.hpp"
#include <string.h>
#include <fstream>

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
        
        if(codeSize > 51050)
        {
            fileBinToRead.close();
            throw 100011;
        }

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