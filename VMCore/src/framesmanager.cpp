#include "../include/vmcpu.hpp"

void VMCPU::getDataFromCodeData(std::string &arg1, int startFrom)
{
    int counter = startFrom;
    int frameNumberToRestore = currentFrameNumber;
    std::stringstream ss;
    VBYTE b;
    while(true)
    {
        if(areFramesNeeded && (counter >= CODE_DATA_SIZE)) counter = loadFrame(counter);
        b = AS->codeData[counter++];
        if((b == 0x3) && (AS->codeData[counter] == 0xD)) break;
        ss << std::hex << b;
    }
    arg1 = ss.str();
    currentFrameNumber = frameNumberToRestore;
    restoreFrame();
    return;
} 

void VMCPU::writeByteIntoFrame(int bytePosition, int howManyBytes, std::vector<VBYTE> bytes)
{
    #ifdef V_DEBUG
        std::cout << "[DEBUG] Write bytes into frame" << std::endl;
    #endif
    auto sum = 0;
    auto frameNumber = -1;
    for (const auto& [key, value] : frameMap) {
        sum += value;
        if(sum >= (bytePosition + 1))
        {
            frameNumber = key;
            break;
        }
    }
    if(frameNumber == -1) goto error_getByteFromFrame;
    else
    {
        char byte;
        std::vector<VBYTE> readData;
        std::string frameToLoadName = ".cached." + std::to_string(frameNumber) + ".frame";
        std::ifstream fileBinToRead;
        std::ofstream fileBinToWrite;
        fileBinToRead.open(frameToLoadName, std::ios::binary);
        auto positionToGet = frameMap[frameNumber] - (sum - bytePosition);
        auto counter = 0;
        if(fileBinToRead.is_open())
        {
            while(fileBinToRead.get(byte))
            {
                ++counter;
                readData.push_back(byte);
            }
            fileBinToRead.close();
            
            for(auto i = 0; i < howManyBytes; i++) readData[positionToGet++] = bytes[i];
            
            fileBinToWrite.open(frameToLoadName.c_str(), std::fstream::out | std::ios::binary);
            VBYTE *dataToWrite = &readData[0];
            fileBinToWrite.write((char*)dataToWrite, counter-1);
            fileBinToWrite.close();
        }
        else goto error_getByteFromFrame;
    }
    goto ok_getByteFromFrame;

error_getByteFromFrame:
    #ifdef V_DEBUG
        std::cout << "[ERROR] Write bytes into frame" << std::endl;
    #endif
    isError = true;
    return;
ok_getByteFromFrame:
    isError = false;
    return;
}

std::vector<VBYTE> VMCPU::getByteFromFrame(int bytePosition, int howManyBytes)
{
    #ifdef V_DEBUG
        std::cout << "[DEBUG] Get a byte from frame" << std::endl;
    #endif
    std::vector<VBYTE> readBytes;
    char byte;
    auto sum = 0;
    auto frameNumber = -1;
    for (const auto& [key, value] : frameMap) {
        sum += value;
        if(sum >= (bytePosition + 1))
        {
            frameNumber = key;
            break;
        }
    }
    if(frameNumber == -1) goto error_getByteFromFrame;
    else
    {
        std::string frameToLoadName = ".cached." + std::to_string(frameNumber) + ".frame";
        std::ifstream fileBinToRead;
        fileBinToRead.open(frameToLoadName, std::ios::binary);
        auto positionToGet = frameMap[frameNumber] - (sum - bytePosition);
        if(fileBinToRead.is_open())
        {
            auto counter = 0;
            while(fileBinToRead.get(byte))
            {
                if(counter == positionToGet)
                { 
                    readBytes.push_back(byte);
                    --howManyBytes;
                    ++positionToGet;
                    if(howManyBytes == 0) break;
                }
                ++counter;
            }
            fileBinToRead.close();
        }
        else goto error_getByteFromFrame;
    }
    goto ok_getByteFromFrame;

error_getByteFromFrame:
    #ifdef V_DEBUG
        std::cout << "[ERROR] Failed get a byte from frame" << std::endl;
    #endif
    isError = true;
    readBytes.push_back(0x0);
    return readBytes;
ok_getByteFromFrame:
    isError = false;
    return readBytes;
}

int VMCPU::loadFrame(int pc)
{
    #ifdef V_DEBUG
        std::cout << "[DEBUG] Load a frame" << std::endl;
    #endif
    auto sum = 0;
    auto frameNumber = -1;
    for (const auto& [key, value] : frameMap) {
        sum += value;
        if(sum >= (pc+1))
        {
            frameNumber = key;
            break;
        }
    }

    if(frameMap[currentFrameNumber] == pc)
    {
        frameNumber = currentFrameNumber + 1;
        pc = 0;
        sum = frameMap[frameNumber];
    }

    #ifdef V_DEBUG
        std::cout << "[DEBUG] Frame number: " << frameNumber << std::endl;
    #endif
    if(frameNumber == -1) goto error_loadFrame;
    else
    {
        std::string frameToLoadName = ".cached." + std::to_string(frameNumber) + ".frame";
        std::ifstream fileBinToRead;
        fileBinToRead.open(frameToLoadName, std::ios::binary);
        if(fileBinToRead.is_open())
        {
            char vb;
            auto counter = 0;
            memset(AS->codeData, 0, CODE_DATA_SIZE*sizeof(*(AS->codeData)));
            while(fileBinToRead.get(vb)) AS->codeData[counter++] = vb;
            fileBinToRead.close();
        }
        else goto error_loadFrame;
    }
    goto ok_loadFrame;

error_loadFrame:
    #ifdef V_DEBUG
        std::cout << "[ERROR] Failed load a frame" << std::endl;
    #endif
    isError = true;
    return 0;

ok_loadFrame:
    currentFrameNumber = frameNumber;
    isError = false;
    return (frameMap[frameNumber] - (sum - pc));
}

void VMCPU::restoreFrame()
{
    #ifdef V_DEBUG
        std::cout << "[DEBUG] Restore a frame" << std::endl;
    #endif
    std::string frameToLoadName = ".cached." + std::to_string(currentFrameNumber) + ".frame";
    std::ifstream fileBinToRead;
    fileBinToRead.open(frameToLoadName, std::ios::binary);
    if(fileBinToRead.is_open())
    {
        char vb;
        auto counter = 0;
        memset(AS->codeData, 0, CODE_DATA_SIZE*sizeof(*(AS->codeData)));
        while(fileBinToRead.get(vb)) AS->codeData[counter++] = vb;
        fileBinToRead.close();
    }
    else goto error_restoreFrame;
    goto ok_restoreFrame;

error_restoreFrame:
    #ifdef V_DEBUG
        std::cout << "[ERROR] Failed restore a frame" << std::endl;
    #endif
    isError = true;
    return;
ok_restoreFrame:
    isError = false;
    return;
}