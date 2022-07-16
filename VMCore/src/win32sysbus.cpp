#include "../include/drivers/win32sysbus.hpp"

#ifdef _WIN32_DEV_ENVIRONMENT
    int WIN32::createDirectory(std::string dirName, int dirMode)
    {
        if (CreateDirectoryA(dirName.c_str(), NULL)) return DIR_CREATED;
        else if (ERROR_ALREADY_EXISTS == GetLastError()) return DIR_EXIST;
        else return OTHER_DIR_ERROR;
    }

    int WIN32::deleteDirectory(std::string dirName)
    {
        if(deleteDir((stringToWString(dirName)).c_str())) return DELETE_DIR;
        else return OTHER_DIR_ERROR;
    }

    int WIN32::moveDirectory(std::string dirSource, std::string dirDest)
    {
        if (!MoveFileExA(dirSource.c_str(), dirDest.c_str(), MOVEFILE_WRITE_THROUGH)) return OTHER_DIR_ERROR;
        else return MOVE_DIR;
    }

    int WIN32::copyDirectory(std::string dirSource, std::string dirDest)
    {
        if(copyDir((stringToWString(dirSource)).c_str()), (stringToWString(dirDest)).c_str())) return COPY_DIR;
        else return OTHER_DIR_ERROR;
    }

    int WIN32::deleteFile(std::string filePath)
    {
        try {
            DeleteFile(stringToWString(filePath).c_str());
            return DELETE_FILE;
        }
        catch(...) {
            return OTHER_FILE_ERROR;
        }
    }

    int WIN32::copyFile(std::string filePathSource, std::string filePathDest)
    {
        try {
            if (CopyFile(stringToWString(filePathSource).c_str(), stringToWString(filePathDest).c_str(), true)) return COPY_FILE;
            else return OTHER_FILE_ERROR;
        }
        catch (...) {
            return OTHER_FILE_ERROR;
        }
    }

    int WIN32::moveFile(std::string filePathSource, std::string filePathDest)
    {
        auto retval = copyFile(std::string filePathSource, std::string filePathDest);
        if(retval == OTHER_FILE_ERROR) return OTHER_FILE_ERROR;
        return deleteFile(std::string filePathSource);
    }
    
    bool WIN32::copyDir(std::wstring dirSource, std::wstring dirDest)
    {
        std::wstring new_sf = dirSource + L"\\*";
        WCHAR sf[MAX_PATH + 1];
        WCHAR tf[MAX_PATH + 1];

        wcscpy_s(sf, MAX_PATH, new_sf.c_str());
        wcscpy_s(tf, MAX_PATH, dirDest.c_str());

        sf[lstrlenW(sf) + 1] = 0;
        tf[lstrlenW(tf) + 1] = 0;

        SHFILEOPSTRUCTW s = { 0 };
        s.wFunc = FO_COPY;
        s.pTo = tf;
        s.pFrom = sf;
        s.fFlags = FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI;
        int res = SHFileOperationW(&s);

        return res == 0;
    }

    int WIN32::deleteDir(LPCTSTR lpszDir)
    {
        bool noRecycleBin = true;
        int len = _tcslen(lpszDir);
        TCHAR* pszFrom = new TCHAR[len + 2];
        _tcscpy_s(pszFrom, len + 2, lpszDir);
        pszFrom[len] = 0;
        pszFrom[len + 1] = 0;

        SHFILEOPSTRUCT fileop;
        fileop.hwnd = NULL;    // no status display
        fileop.wFunc = FO_DELETE;  // delete operation
        fileop.pFrom = pszFrom;  // source file name as double null terminated string
        fileop.pTo = NULL;    // no destination needed
        fileop.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;  // do not prompt the user

        if (!noRecycleBin) fileop.fFlags |= FOF_ALLOWUNDO;

        fileop.fAnyOperationsAborted = FALSE;
        fileop.lpszProgressTitle = NULL;
        fileop.hNameMappings = NULL;

        int ret = SHFileOperation(&fileop);
        delete[] pszFrom;
        return (ret == 0);
    }

    std::wstring WIN32::stringToWString(const std::string& s)
    {
        std::wstring ws;
        std::wstring wsTmp(s.begin(), s.end());
        ws = wsTmp;
        return ws;
    }
#endif