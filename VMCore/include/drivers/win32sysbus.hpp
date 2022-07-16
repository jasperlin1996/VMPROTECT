#ifndef _WIN32_SYSTEM_BUS_HPP
#define _WIN32_SYSTEM_BUS_HPP

#include "sysbus.hpp"

#ifdef _WIN32_DEV_ENVIRONMENT
    #include <Windows.h>
    #include <tchar.h>
    #include <shellapi.h>

    class WIN32: public SYSBUS {
        public:
            WIN32() { };
            ~WIN32() { };
            int createDirectory(std::string, int);
            int deleteDirectory(std::string);
            int moveDirectory(std::string, std::string);
            int copyDirectory(std::string, std::string);
            int deleteFile(std::string);
            int moveFile(std::string, std::string);
            int copyFile(std::string, std::string);

        private:
            bool deleteDir(LPCTSTR);
            bool copyDir(std::wstring, std::wstring);
            std::wstringstringToWString(const std::string&);
    };
#endif

#endif