#ifndef _SYSTEM_BUS_HPP
#define _SYSTEM_BUS_HPP

#include "../../../SharedCode/global.hpp"
#include <iostream>
#include <string>
#include <fstream>

#include "../../../SharedCode/datatypes.hpp"

#define DIR_CREATED 200000
#define DIR_CREATED_ERROR 200001
#define DIR_EXIST 200002
#define DELETE_DIR 200011
#define MOVE_DIR 200003
#define COPY_DIR 200004
#define OTHER_DIR_ERROR 200005
#define FILE_CREATED 200006
#define FILE_EXIST 200007
#define DELETE_FILE 200012
#define MOVE_FILE 200008
#define COPY_FILE 200009
#define OTHER_FILE_ERROR 200010
#define NO_IMPLEMENTED_ERROR 200110

#define SYSBUS_CREATE_DIR 1
#define SYSBUS_DELETE_DIR 2
#define SYSBUS_MOVE_DIR 3
#define SYSBUS_COPY_DIR 4
#define SYSBUS_CREATE_FILE 5
#define SYSBUS_DELETE_FILE 6
#define SYSBUS_MOVE_FILE 7
#define SYSBUS_COPY_FILE 8

class SYSBUS {
    public:
        int createFile(std::string fileName, VBYTE *dataToWrite, int dataSize)
        {
            std::fstream fs;
            fs.open(fileName.c_str(), std::fstream::in);
            if(fs)
            {
                fs.close();
                fs.open(fileName.c_str(), std::fstream::out | std::fstream:: app| std::fstream::binary);
            }
            else fs.open(fileName.c_str(), std::fstream::out | std::fstream::binary);
            fs.write((char*)dataToWrite, dataSize);
            fs.close();
            return 0;
        }
};

#endif