#ifndef _LINUX_SYSTEM_BUS_HPP
#define _LINUX_SYSTEM_BUS_HPP

#include "sysbus.hpp"

#ifdef _LINUX_DEV_ENVIRONMENT
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    class UNIX: public SYSBUS {
        public:
            UNIX() { };
            ~UNIX() { };
            int createDirectory(std::string, int);
            int deleteDirectory(std::string);
            int moveDirectory(std::string, std::string);
            int copyDirectory(std::string, std::string);
            int deleteFile(std::string);
            int moveFile(std::string, std::string);
            int copyFile(std::string, std::string);

        private:
            mode_t getMode(int);
    };
#endif

#endif